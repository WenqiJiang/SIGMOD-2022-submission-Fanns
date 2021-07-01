#pragma once

#include "constants.hpp"
#include "types.hpp"

#define CLUSTER_DISTANCE_PE_WIDTH 8 // each PE for vectorized computation of 8 numbers
#define CLUSTER_DISTANCE_PE_NUM (D / CLUSTER_DISTANCE_PE_WIDTH)

////////////////////    Function to call in top-level     //////////////////// 
template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float> &s_centroid_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<dist_cell_ID_t> &s_cell_distance);
////////////////////    Function to call in top-level     //////////////////// 

template<const int data_width>
void parallel_L2_distance(
    float in_array_A[data_width], 
    ap_uint<64> in_array_B[data_width / 2], 
    float out_array[data_width]);
    
template<typename T, const int data_width>
T reduction_sum_cluster_distance_computation(T array[data_width]);

template<>
void parallel_L2_distance<CLUSTER_DISTANCE_PE_WIDTH>(
    float in_array_A[CLUSTER_DISTANCE_PE_WIDTH], 
    ap_uint<64> in_array_B[CLUSTER_DISTANCE_PE_WIDTH / 2], 
    float out_array[CLUSTER_DISTANCE_PE_WIDTH]) {
#pragma HLS inline
    // Note: the input and output array must already be partitioned

    for (int i = 0; i < CLUSTER_DISTANCE_PE_WIDTH / 2; i++) {
#pragma HLS UNROLL
        ap_uint<32> tmp_0_ap = in_array_B[i].range(31, 0);
        ap_uint<32> tmp_1_ap = in_array_B[i].range(63, 32);
        float tmp_0_float = *((float*) (&tmp_0_ap));
        float tmp_1_float = *((float*) (&tmp_1_ap));
        float diff_0 = in_array_A[2 * i] - tmp_0_float;
        float diff_1 = in_array_A[2 * i + 1] - tmp_1_float;
        out_array[2 * i] = diff_0 * diff_0;
        out_array[2 * i + 1] = diff_1 * diff_1;
    }
}

template<>
float reduction_sum_cluster_distance_computation<float, 8>(float array[8]) {
#pragma HLS inline
    return array[0] + array[1] + array[2] + array[3] + 
        array[4] + array[5] + array[6] + array[7];
}

template<>
float reduction_sum_cluster_distance_computation<float, 16>(float array[16]) {
#pragma HLS inline
    return array[0] + array[1] + array[2] + array[3] + 
        array[4] + array[5] + array[6] + array[7] + 
        array[8] + array[9] + array[10] + array[11] + 
        array[12] + array[13] + array[14] + array[15];
}

void partition_centroid_vectors(
    hls::stream<float> &s_centroid_vectors, 
    hls::stream<float> (&s_partial_centroid_vectors)[CLUSTER_DISTANCE_PE_NUM]) {
    
    // each PE is assigned for 8 columns of the entire IVF index, 
    //   PE0 col 0~7, PE1 col 8~15, ... PE15 col 120~127

    for (int c = 0; c < NLIST; c++) {

        for (int s = 0; s < CLUSTER_DISTANCE_PE_NUM; s++) {

            for (int d = 0; d < CLUSTER_DISTANCE_PE_WIDTH; d++) {
#pragma HLS pipeline II=1
                s_partial_centroid_vectors[s].write(s_centroid_vectors.read());
            }
        }
    }
}

template<const int query_num>
void partition_query_vectors(
    hls::stream<float> &s_query_vectors, 
    hls::stream<float> (&s_partial_query_vectors)[CLUSTER_DISTANCE_PE_NUM]) {

    // each PE is assigned for 8 columns of the query vector, 
    //   PE0 col 0~7, PE1 col 8~15, ... PE15 col 120~127
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int s = 0; s < CLUSTER_DISTANCE_PE_NUM; s++) {
            
            for (int d = 0; d < CLUSTER_DISTANCE_PE_WIDTH; d++) {
#pragma HLS pipeline II=1
                s_partial_query_vectors[s].write(s_query_vectors.read());
            }
        }
    }
}

template<const int query_num>
void compute_cell_distance(
    hls::stream<float> &s_partial_centroid_vectors,
    hls::stream<float> &s_partial_query_vectors,
    hls::stream<float> &s_partial_cell_distance) {

    // Given all vector quantizers, compute the distance to each of them

    // use ap_uint to maximize the memory usage percentage of URAM (64/72 bits)
    // use float or struct of 2 floats will double the resource usage, since HLS
    //   will treat them as separate arrays
    // 8 columns of the entire IVF index, PE0 col 0~7, PE1 col 8~15, ... PE15 col 120~127
    ap_uint<64> partial_cell_centroids[NLIST][CLUSTER_DISTANCE_PE_WIDTH / 2];
#pragma HLS array_partition variable=partial_cell_centroids dim=2
#pragma HLS resource variable=partial_cell_centroids core=RAM_2P_URAM

    float partial_query_vector_buffer[CLUSTER_DISTANCE_PE_WIDTH];
#pragma HLS array_partition variable=partial_query_vector_buffer complete

    // init before any computation
    for (int c = 0; c < NLIST; c++) {
        for (int d = 0; d < CLUSTER_DISTANCE_PE_WIDTH / 2; d++) {
#pragma HLS pipeline II=2
            float tmp_0_float = s_partial_centroid_vectors.read();
            float tmp_1_float = s_partial_centroid_vectors.read();
            ap_uint<32> tmp_0_ap = *((ap_uint<32>*) (&tmp_0_float));
            ap_uint<32> tmp_1_ap = *((ap_uint<32>*) (&tmp_1_float));
            partial_cell_centroids[c][d].range(31, 0) = tmp_0_ap;
            partial_cell_centroids[c][d].range(63, 32) = tmp_1_ap;
        }
    }

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load partial query vec
        for (int d = 0; d < CLUSTER_DISTANCE_PE_WIDTH; d++) {
#pragma HLS pipeline II=1
            partial_query_vector_buffer[d] = s_partial_query_vectors.read();
        }

        // compute distance and write results out
        for (int c = 0; c < NLIST; c++) {
#pragma HLS pipeline II=1 

            float partial_L2_distance_buffer[CLUSTER_DISTANCE_PE_WIDTH];
#pragma HLS array_partition variable=partial_L2_distance_buffer complete

            parallel_L2_distance<CLUSTER_DISTANCE_PE_WIDTH>(
                partial_query_vector_buffer, 
                partial_cell_centroids[c],
                partial_L2_distance_buffer);

            float partial_out = reduction_sum_cluster_distance_computation<float, CLUSTER_DISTANCE_PE_WIDTH>(
                partial_L2_distance_buffer);

            // float partial_out = parallel_add_tree<float, CLUSTER_DISTANCE_PE_WIDTH, 2>(
            //     partial_L2_distance_buffer);

            s_partial_cell_distance.write(partial_out);
        }
    }
}

template<const int query_num>
void gather_cell_distance(
    hls::stream<float> (&s_partial_cell_distance)[CLUSTER_DISTANCE_PE_NUM],
    hls::stream<dist_cell_ID_t> &s_cell_distance) {
    // Take the results from all 16 PEs as input, gather them as by add tree

    for (int query_id = 0; query_id < query_num; query_id++) {

        // compute distance and write results out
        for (int c = 0; c < NLIST; c++) {
#pragma HLS pipeline II=1

            float partial_distances[CLUSTER_DISTANCE_PE_NUM];
#pragma HLS array_partition variable=partial_distances complete

            for (int s = 0; s < CLUSTER_DISTANCE_PE_NUM; s++) {
#pragma HLS UNROLL
                partial_distances[s] = s_partial_cell_distance[s].read();
            }

            dist_cell_ID_t out;
            out.cell_ID = c;
            out.dist = reduction_sum_cluster_distance_computation<float, CLUSTER_DISTANCE_PE_NUM>(
                partial_distances);
            // out.dist = parallel_add_tree<float, CLUSTER_DISTANCE_PE_NUM, 2>(
            //     partial_distances);

            s_cell_distance.write(out);
        }
    }
}

template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float> &s_centroid_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<dist_cell_ID_t> &s_cell_distance) {
    // Given all vector quantizers, compute the distance to each of them
#pragma HLS dataflow

    hls::stream<float> s_partial_centroid_vectors[CLUSTER_DISTANCE_PE_NUM];
#pragma HLS stream variable=s_partial_centroid_vectors depth=2
#pragma HLS array_partition variable=s_partial_centroid_vectors complete

    hls::stream<float> s_partial_query_vectors[CLUSTER_DISTANCE_PE_NUM];
#pragma HLS stream variable=s_partial_query_vectors depth=2
#pragma HLS array_partition variable=s_partial_query_vectors complete

    hls::stream<float> s_partial_cell_distance[CLUSTER_DISTANCE_PE_NUM];
#pragma HLS stream variable=s_partial_cell_distance depth=8
#pragma HLS array_partition variable=s_partial_cell_distance complete

    partition_centroid_vectors(
        s_centroid_vectors, 
        s_partial_centroid_vectors);

    partition_query_vectors<query_num>(
        s_query_vectors, 
        s_partial_query_vectors);

    for (int s = 0; s < CLUSTER_DISTANCE_PE_NUM; s++) {
#pragma HLS UNROLL
        compute_cell_distance<query_num>(
            s_partial_centroid_vectors[s],
            s_partial_query_vectors[s],
            s_partial_cell_distance[s]);
    }

    gather_cell_distance<query_num>(
        s_partial_cell_distance,
        s_cell_distance);
}
