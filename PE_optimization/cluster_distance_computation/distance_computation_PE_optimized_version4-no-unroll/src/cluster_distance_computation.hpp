#pragma once

#include "constants.hpp"
#include "types.hpp"

////////////////////    Function to call in top-level     //////////////////// 
template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float> &s_centroid_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<dist_cell_ID_t> &s_cell_distance);
////////////////////    Function to call in top-level     //////////////////// 

void partition_centroid_vectors(
    hls::stream<float> &s_centroid_vectors, 
    hls::stream<float> (&s_partial_centroid_vectors)[PE_NUM_CLUSTER_DISTANCE_COMPUTATION]) {
    
    // each PE is assigned for 8 columns of the entire IVF index, 
    //   PE0 row 0 ~ N - 1, PE 1 row N ~ 2N - 1, etc.

    for (int s = 0; s < PE_NUM_CLUSTER_DISTANCE_COMPUTATION; s++) {

        for (int c = 0; c < CENTROIDS_PER_DISTANCE_COMPUTATION_PE * D; c++) {
#pragma HLS pipeline II=1
            s_partial_centroid_vectors[s].write(s_centroid_vectors.read());
        }
    }
}

template<const int query_num>
void broadcast_query_vectors(
    hls::stream<float> &s_query_vectors, 
    hls::stream<float> (&s_replicated_query_vectors)[PE_NUM_CLUSTER_DISTANCE_COMPUTATION]) {

    // broadcast query vector to all PEs

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors.read();
            for (int s = 0; s < PE_NUM_CLUSTER_DISTANCE_COMPUTATION; s++) {
#pragma HLS UNROLL
                s_replicated_query_vectors[s].write(reg);
            }
        }
    }
}

template<const int query_num>
void compute_cell_distance(
    int start_cell_ID,
    hls::stream<float> &s_centroid_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<dist_cell_ID_t> &s_partial_cell_distance) {

    // what is the speed needed to compute these distances? need to match HBM
    // python performance_estimation_three_nodes_IVF.py --nlist=8192 --nprobe=32
    // Estimated HBM Latency: 23.58 us -> 3K cycles
    // each URAM -> 64bit port x 2 -> 4 elements per cycle -> 64 cycles per vec
    //   -> at most 50 vectors per URAM (actually 1 URAM can only fit 32)
    // My strategy: 32 Partitions, each responsible for 256 vectors, 
    //   each vector finish in 10 cycles
    float cell_centroids_partition[CENTROIDS_PER_DISTANCE_COMPUTATION_PE * D];
// #pragma HLS array_partition variable=cell_centroids_partition cyclic factor=8 
#pragma HLS resource variable=cell_centroids_partition core=RAM_2P_URAM

    float local_query_buffer[D];
#pragma HLS array_partition variable=local_query_buffer cyclic factor=8

    // init before any computation
    for (int c = 0; c < CENTROIDS_PER_DISTANCE_COMPUTATION_PE; c++) {
        for (int d = 0; d < D; d++) {
        #pragma HLS pipeline II=1
            cell_centroids_partition[c * D + d] = s_centroid_vectors.read();
        }
    }

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vec
        for (int d = 0; d < D; d++) {
        #pragma HLS pipeline II=1
            local_query_buffer[d] = s_query_vectors.read();
        }

#define CLUSTER_DISTANCE_COMPUTATION_TILE_SIZE 32 // to hide the pipeline latency

        for (int t_outer = 0; t_outer < CENTROIDS_PER_DISTANCE_COMPUTATION_PE / CLUSTER_DISTANCE_COMPUTATION_TILE_SIZE; t_outer++) {
            
            float distance[CLUSTER_DISTANCE_COMPUTATION_TILE_SIZE];
// Wenqi: here, if I partition the distance array, the compute loop will fail to have II=1, even if 
//   the dependency is turned off
//#pragma HLS array_partition variable=distance complete
            
            for (int d = 0; d < D; d++) {
#pragma HLS loop_flatten off
                
                for (int t_inner = 0; t_inner < CLUSTER_DISTANCE_COMPUTATION_TILE_SIZE; t_inner++) {
#pragma HLS pipeline II=1
                    const float prev = (t_outer == 0)? 0 : distance[t_inner];
                    int c = t_outer * CLUSTER_DISTANCE_COMPUTATION_TILE_SIZE + t_inner;
                    float scalar_dist = local_query_buffer[d] - cell_centroids_partition[c + d];
                    float square_dist = scalar_dist * scalar_dist; 
                    distance[t_inner] = prev + square_dist;
#pragma HLS dependence variable=distance false
                }
            }

            for (int t_inner = 0; t_inner < CLUSTER_DISTANCE_COMPUTATION_TILE_SIZE; t_inner++) {
#pragma HLS pipeline II=1
                int c = t_outer * CLUSTER_DISTANCE_COMPUTATION_TILE_SIZE + t_inner;

                dist_cell_ID_t out;
                out.dist = distance[t_inner];
                out.cell_ID = start_cell_ID + c;

                s_partial_cell_distance.write(out);
            }
        }
    }
}


template<const int query_num>
void gather_cell_distance(
    hls::stream<dist_cell_ID_t> (&s_partial_cell_distance)[PE_NUM_CLUSTER_DISTANCE_COMPUTATION],
    hls::stream<dist_cell_ID_t> &s_cell_distance) {
    // Take the results from all 16 PEs as input, gather them as by add tree

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int c = 0; c < CENTROIDS_PER_DISTANCE_COMPUTATION_PE; c++) {

            for (int s = 0; s < PE_NUM_CLUSTER_DISTANCE_COMPUTATION; s++) {
#pragma HLS pipeline II=1
                s_cell_distance.write(s_partial_cell_distance[s].read());
            }
        }
    }
}


template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float> &s_centroid_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<dist_cell_ID_t> &s_cell_distance) {
#pragma HLS inline

    hls::stream<float> s_partial_centroid_vectors[PE_NUM_CLUSTER_DISTANCE_COMPUTATION];
#pragma HLS stream variable=s_partial_centroid_vectors depth=2
#pragma HLS array_partition variable=s_partial_centroid_vectors complete

    hls::stream<float> s_replicated_query_vectors[PE_NUM_CLUSTER_DISTANCE_COMPUTATION];
#pragma HLS stream variable=s_replicated_query_vectors depth=2
#pragma HLS array_partition variable=s_replicated_query_vectors complete

    hls::stream<dist_cell_ID_t> s_partial_cell_distance[PE_NUM_CLUSTER_DISTANCE_COMPUTATION];
#pragma HLS stream variable=s_partial_cell_distance depth=8
#pragma HLS array_partition variable=s_partial_cell_distance complete

    partition_centroid_vectors(
        s_centroid_vectors, 
        s_partial_centroid_vectors);

    broadcast_query_vectors<query_num>(
        s_query_vectors, 
        s_replicated_query_vectors);

    for (int s = 0; s < PE_NUM_CLUSTER_DISTANCE_COMPUTATION; s++) {
#pragma HLS UNROLL
        compute_cell_distance<QUERY_NUM>(
            s * CENTROIDS_PER_DISTANCE_COMPUTATION_PE, 
            s_partial_centroid_vectors[s], 
            s_replicated_query_vectors[s], 
            s_partial_cell_distance[s]);
    }

    gather_cell_distance<query_num>(
        s_partial_cell_distance,
        s_cell_distance);
}
