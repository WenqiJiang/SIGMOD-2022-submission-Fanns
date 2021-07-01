#pragma once 

#include "constants.hpp"
#include "types.hpp"

////////////////////     Function to call in top-level     ////////////////////
template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float> &s_centroid_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<dist_cell_ID_t> &s_cell_distance);

////////////////////     Function to call in top-level     ////////////////////


template<const int query_num, const int centroids_per_partition, const int total_centriods>
void compute_cell_distance_middle(
    const int systolic_array_id,
    hls::stream<float>& s_centroid_vectors_in,
    hls::stream<float>& s_centroid_vectors_out,
    hls::stream<float>& s_query_vectors_in,
    hls::stream<float>& s_query_vectors_out,
    hls::stream<dist_cell_ID_t>& s_partial_cell_PE_result) {

    // systolic_array_id should be the middle ids, 
    //   e.g., for systolic array of 16 PEs, one head, one tail, then middle should be 1~14

    // what is the speed needed to compute these distances? need to match HBM
    // python performance_estimation_three_nodes_IVF.py --nlist=8192 --nprobe=32
    // Estimated HBM Latency: 23.58 us -> 3K cycles
    // each URAM -> 64bit port x 2 -> 4 elements per cycle -> 64 cycles per vec
    //   -> at most 50 vectors per URAM (actually 1 URAM can only fit 32)
    // My strategy: 32 Partitions, each responsible for 256 vectors, 
    //   each vector finish in 10 cycles
    float cell_centroids_partition[centroids_per_partition * D];
#pragma HLS array_partition variable=cell_centroids_partition cyclic factor=8 
#pragma HLS resource variable=cell_centroids_partition core=RAM_2P_URAM

    float local_query_buffer[D];
#pragma HLS array_partition variable=local_query_buffer cyclic factor=8

    int start_cell_ID = centroids_per_partition * systolic_array_id;

    for (int c = 0; c < total_centriods; c++) {
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_centroid_vectors_in.read();
            s_centroid_vectors_out.write(reg);
            if ((c >= systolic_array_id * centroids_per_partition) && (c < (systolic_array_id + 1) * centroids_per_partition)) {
                cell_centroids_partition[(c - systolic_array_id * centroids_per_partition) * D + d] = reg;
            }
        }
    }

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vec
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors_in.read();
            local_query_buffer[d] = reg;
            s_query_vectors_out.write(reg);
        }

        // compute distance and write results out
        for (int c = 0; c < centroids_per_partition; c++) {

            float distance = 0;

            // Manually unroll 16, auto-unroll doesn't work well
            for (int d = 0; d < D / 16; d++) {
#pragma HLS pipeline II=1
            //#pragma HLS UNROLL factor=16
                float scalar_dist_0 = local_query_buffer[d * 16 + 0] - cell_centroids_partition[c * D + d * 16 + 0];
                float scalar_dist_1 = local_query_buffer[d * 16 + 1] - cell_centroids_partition[c * D + d * 16 + 1];
                float scalar_dist_2 = local_query_buffer[d * 16 + 2] - cell_centroids_partition[c * D + d * 16 + 2];
                float scalar_dist_3 = local_query_buffer[d * 16 + 3] - cell_centroids_partition[c * D + d * 16 + 3];
                float scalar_dist_4 = local_query_buffer[d * 16 + 4] - cell_centroids_partition[c * D + d * 16 + 4];
                float scalar_dist_5 = local_query_buffer[d * 16 + 5] - cell_centroids_partition[c * D + d * 16 + 5];
                float scalar_dist_6 = local_query_buffer[d * 16 + 6] - cell_centroids_partition[c * D + d * 16 + 6];
                float scalar_dist_7 = local_query_buffer[d * 16 + 7] - cell_centroids_partition[c * D + d * 16 + 7];
                float scalar_dist_8 = local_query_buffer[d * 16 + 8] - cell_centroids_partition[c * D + d * 16 + 8];
                float scalar_dist_9 = local_query_buffer[d * 16 + 9] - cell_centroids_partition[c * D + d * 16 + 9];
                float scalar_dist_10 = local_query_buffer[d * 16 + 10] - cell_centroids_partition[c * D + d * 16 + 10];
                float scalar_dist_11 = local_query_buffer[d * 16 + 11] - cell_centroids_partition[c * D + d * 16 + 11];
                float scalar_dist_12 = local_query_buffer[d * 16 + 12] - cell_centroids_partition[c * D + d * 16 + 12];
                float scalar_dist_13 = local_query_buffer[d * 16 + 13] - cell_centroids_partition[c * D + d * 16 + 13];
                float scalar_dist_14 = local_query_buffer[d * 16 + 14] - cell_centroids_partition[c * D + d * 16 + 14];
                float scalar_dist_15 = local_query_buffer[d * 16 + 15] - cell_centroids_partition[c * D + d * 16 + 15];

                float square_dist_0 = scalar_dist_0 * scalar_dist_0;
                float square_dist_1 = scalar_dist_1 * scalar_dist_1;
                float square_dist_2 = scalar_dist_2 * scalar_dist_2;
                float square_dist_3 = scalar_dist_3 * scalar_dist_3;
                float square_dist_4 = scalar_dist_4 * scalar_dist_4;
                float square_dist_5 = scalar_dist_5 * scalar_dist_5;
                float square_dist_6 = scalar_dist_6 * scalar_dist_6;
                float square_dist_7 = scalar_dist_7 * scalar_dist_7;
                float square_dist_8 = scalar_dist_8 * scalar_dist_8;
                float square_dist_9 = scalar_dist_9 * scalar_dist_9;
                float square_dist_10 = scalar_dist_10 * scalar_dist_10;
                float square_dist_11 = scalar_dist_11 * scalar_dist_11;
                float square_dist_12 = scalar_dist_12 * scalar_dist_12;
                float square_dist_13 = scalar_dist_13 * scalar_dist_13;
                float square_dist_14 = scalar_dist_14 * scalar_dist_14;
                float square_dist_15 = scalar_dist_15 * scalar_dist_15;

                distance += square_dist_0 + square_dist_1 + square_dist_2 + square_dist_3 + 
                    square_dist_4 + square_dist_5 + square_dist_6 + square_dist_7 + 
                    square_dist_8 + square_dist_9 + square_dist_10 + square_dist_11 + 
                    square_dist_12 + square_dist_13 + square_dist_14 + square_dist_15; 
            }

            dist_cell_ID_t out;
            out.dist = distance;
            out.cell_ID = start_cell_ID + c;

            s_partial_cell_PE_result.write(out);
        }
    }
}


template<const int query_num, const int centroids_per_partition>
void forward_cell_distance_middle(
    const int systolic_array_id,
    hls::stream<dist_cell_ID_t>& s_partial_cell_PE_result,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance_in,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance_out) {


    for (int query_id = 0; query_id < query_num; query_id++) {

        // compute distance and write results out
        for (int c = 0; c < centroids_per_partition; c++) {

            // when the PE with larger ID finished computing, previous PE should already finished them
            for (int forward_iter = 0; forward_iter < systolic_array_id + 1; forward_iter++) {
#pragma HLS pipeline II=1
                dist_cell_ID_t out;
                if (forward_iter < systolic_array_id) {
                    out = s_partial_cell_distance_in.read();
                }
                else{
                    out = s_partial_cell_PE_result.read();
                }
                s_partial_cell_distance_out.write(out);
            }
        }
    }
}


template<const int query_num, const int centroids_per_partition, const int total_centriods>
void compute_cell_distance_middle_PE(
    const int systolic_array_id,
    hls::stream<float>& s_centroid_vectors_in,
    hls::stream<float>& s_centroid_vectors_out,
    hls::stream<float>& s_query_vectors_in,
    hls::stream<float>& s_query_vectors_out,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance_in,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance_out) {

#pragma HLS dataflow

    hls::stream<dist_cell_ID_t> s_partial_cell_PE_result;
#pragma HLS stream variable=s_partial_cell_PE_result depth=8

    compute_cell_distance_middle<query_num, centroids_per_partition, total_centriods>(
        systolic_array_id,
        s_centroid_vectors_in,
        s_centroid_vectors_out,
        s_query_vectors_in,
        s_query_vectors_out,
        s_partial_cell_PE_result);

    forward_cell_distance_middle<query_num, centroids_per_partition>(
        systolic_array_id,
        s_partial_cell_PE_result,
        s_partial_cell_distance_in,
        s_partial_cell_distance_out);
}


template<const int query_num, const int centroids_per_partition, const int total_centriods>
void compute_cell_distance_head_PE(
    const int systolic_array_id,
    hls::stream<float>& s_centroid_vectors_in,
    hls::stream<float>& s_centroid_vectors_out,
    hls::stream<float>& s_query_vectors_in,
    hls::stream<float>& s_query_vectors_out,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance_out) {

    // systolic_array_id should be the middle ids, 
    //   e.g., for systolic array of 16 PEs, one head, one tail, then middle should be 1~14

    // what is the speed needed to compute these distances? need to match HBM
    // python performance_estimation_three_nodes_IVF.py --nlist=8192 --nprobe=32
    // Estimated HBM Latency: 23.58 us -> 3K cycles
    // each URAM -> 64bit port x 2 -> 4 elements per cycle -> 64 cycles per vec
    //   -> at most 50 vectors per URAM (actually 1 URAM can only fit 32)
    // My strategy: 32 Partitions, each responsible for 256 vectors, 
    //   each vector finish in 10 cycles
    float cell_centroids_partition[centroids_per_partition * D];
#pragma HLS array_partition variable=cell_centroids_partition cyclic factor=8 
#pragma HLS resource variable=cell_centroids_partition core=RAM_2P_URAM

    float local_query_buffer[D];
#pragma HLS array_partition variable=local_query_buffer cyclic factor=8

    for (int c = 0; c < total_centriods; c++) {
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_centroid_vectors_in.read();
            s_centroid_vectors_out.write(reg);
            if ((c >= systolic_array_id * centroids_per_partition) && (c < (systolic_array_id + 1) * centroids_per_partition)) {
                cell_centroids_partition[(c - systolic_array_id * centroids_per_partition) * D + d] = reg;
            }
        }
    }

    int start_cell_ID = centroids_per_partition * systolic_array_id;

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vec
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors_in.read();
            local_query_buffer[d] = reg;
            s_query_vectors_out.write(reg);
        }

        // compute distance and write results out
        for (int c = 0; c < centroids_per_partition; c++) {

            float distance = 0;

            // Manually unroll 16, auto-unroll doesn't work well
            for (int d = 0; d < D / 16; d++) {
#pragma HLS pipeline II=1
            //#pragma HLS UNROLL factor=16
                float scalar_dist_0 = local_query_buffer[d * 16 + 0] - cell_centroids_partition[c * D + d * 16 + 0];
                float scalar_dist_1 = local_query_buffer[d * 16 + 1] - cell_centroids_partition[c * D + d * 16 + 1];
                float scalar_dist_2 = local_query_buffer[d * 16 + 2] - cell_centroids_partition[c * D + d * 16 + 2];
                float scalar_dist_3 = local_query_buffer[d * 16 + 3] - cell_centroids_partition[c * D + d * 16 + 3];
                float scalar_dist_4 = local_query_buffer[d * 16 + 4] - cell_centroids_partition[c * D + d * 16 + 4];
                float scalar_dist_5 = local_query_buffer[d * 16 + 5] - cell_centroids_partition[c * D + d * 16 + 5];
                float scalar_dist_6 = local_query_buffer[d * 16 + 6] - cell_centroids_partition[c * D + d * 16 + 6];
                float scalar_dist_7 = local_query_buffer[d * 16 + 7] - cell_centroids_partition[c * D + d * 16 + 7];
                float scalar_dist_8 = local_query_buffer[d * 16 + 8] - cell_centroids_partition[c * D + d * 16 + 8];
                float scalar_dist_9 = local_query_buffer[d * 16 + 9] - cell_centroids_partition[c * D + d * 16 + 9];
                float scalar_dist_10 = local_query_buffer[d * 16 + 10] - cell_centroids_partition[c * D + d * 16 + 10];
                float scalar_dist_11 = local_query_buffer[d * 16 + 11] - cell_centroids_partition[c * D + d * 16 + 11];
                float scalar_dist_12 = local_query_buffer[d * 16 + 12] - cell_centroids_partition[c * D + d * 16 + 12];
                float scalar_dist_13 = local_query_buffer[d * 16 + 13] - cell_centroids_partition[c * D + d * 16 + 13];
                float scalar_dist_14 = local_query_buffer[d * 16 + 14] - cell_centroids_partition[c * D + d * 16 + 14];
                float scalar_dist_15 = local_query_buffer[d * 16 + 15] - cell_centroids_partition[c * D + d * 16 + 15];

                float square_dist_0 = scalar_dist_0 * scalar_dist_0;
                float square_dist_1 = scalar_dist_1 * scalar_dist_1;
                float square_dist_2 = scalar_dist_2 * scalar_dist_2;
                float square_dist_3 = scalar_dist_3 * scalar_dist_3;
                float square_dist_4 = scalar_dist_4 * scalar_dist_4;
                float square_dist_5 = scalar_dist_5 * scalar_dist_5;
                float square_dist_6 = scalar_dist_6 * scalar_dist_6;
                float square_dist_7 = scalar_dist_7 * scalar_dist_7;
                float square_dist_8 = scalar_dist_8 * scalar_dist_8;
                float square_dist_9 = scalar_dist_9 * scalar_dist_9;
                float square_dist_10 = scalar_dist_10 * scalar_dist_10;
                float square_dist_11 = scalar_dist_11 * scalar_dist_11;
                float square_dist_12 = scalar_dist_12 * scalar_dist_12;
                float square_dist_13 = scalar_dist_13 * scalar_dist_13;
                float square_dist_14 = scalar_dist_14 * scalar_dist_14;
                float square_dist_15 = scalar_dist_15 * scalar_dist_15;

                distance += square_dist_0 + square_dist_1 + square_dist_2 + square_dist_3 + 
                    square_dist_4 + square_dist_5 + square_dist_6 + square_dist_7 + 
                    square_dist_8 + square_dist_9 + square_dist_10 + square_dist_11 + 
                    square_dist_12 + square_dist_13 + square_dist_14 + square_dist_15; 
            }

            dist_cell_ID_t out;
            out.dist = distance;
            out.cell_ID = start_cell_ID + c;

            s_partial_cell_distance_out.write(out);
        }
    }
}

// centroids_per_partition_last_PE must < centroids_per_partition
template<const int query_num, const int centroids_per_partition, 
    const int centroids_per_partition_last_PE, const int total_centriods>
void compute_cell_distance_tail(
    const int systolic_array_id,
    hls::stream<float>& s_centroid_vectors_in,
    hls::stream<float>& s_query_vectors_in,
    hls::stream<dist_cell_ID_t>& s_partial_cell_PE_result) {

    // systolic_array_id should be the middle ids, 
    //   e.g., for systolic array of 16 PEs, one head, one tail, then middle should be 1~14

    // what is the speed needed to compute these distances? need to match HBM
    // python performance_estimation_three_nodes_IVF.py --nlist=8192 --nprobe=32
    // Estimated HBM Latency: 23.58 us -> 3K cycles
    // each URAM -> 64bit port x 2 -> 4 elements per cycle -> 64 cycles per vec
    //   -> at most 50 vectors per URAM (actually 1 URAM can only fit 32)
    // My strategy: 32 Partitions, each responsible for 256 vectors, 
    //   each vector finish in 10 cycles
    float cell_centroids_partition[centroids_per_partition_last_PE * D];
#pragma HLS array_partition variable=cell_centroids_partition cyclic factor=8 
#pragma HLS resource variable=cell_centroids_partition core=RAM_2P_URAM

    float local_query_buffer[D];
#pragma HLS array_partition variable=local_query_buffer cyclic factor=8

    // init before any computation
    for (int c = 0; c < total_centriods; c++) {
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_centroid_vectors_in.read();
            if ((c >= systolic_array_id * centroids_per_partition) && (c < systolic_array_id * centroids_per_partition + centroids_per_partition_last_PE)) {
                cell_centroids_partition[(c - systolic_array_id * centroids_per_partition) * D + d] = reg;
            }
        }
    }

    int start_cell_ID = centroids_per_partition * systolic_array_id;

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vec
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors_in.read();
            local_query_buffer[d] = reg;
        }

        // compute distance and write results out
        for (int c = 0; c < centroids_per_partition_last_PE; c++) {

            float distance = 0;

            // Manually unroll 16, auto-unroll doesn't work well
            for (int d = 0; d < D / 16; d++) {
#pragma HLS pipeline II=1
            //#pragma HLS UNROLL factor=16
                float scalar_dist_0 = local_query_buffer[d * 16 + 0] - cell_centroids_partition[c * D + d * 16 + 0];
                float scalar_dist_1 = local_query_buffer[d * 16 + 1] - cell_centroids_partition[c * D + d * 16 + 1];
                float scalar_dist_2 = local_query_buffer[d * 16 + 2] - cell_centroids_partition[c * D + d * 16 + 2];
                float scalar_dist_3 = local_query_buffer[d * 16 + 3] - cell_centroids_partition[c * D + d * 16 + 3];
                float scalar_dist_4 = local_query_buffer[d * 16 + 4] - cell_centroids_partition[c * D + d * 16 + 4];
                float scalar_dist_5 = local_query_buffer[d * 16 + 5] - cell_centroids_partition[c * D + d * 16 + 5];
                float scalar_dist_6 = local_query_buffer[d * 16 + 6] - cell_centroids_partition[c * D + d * 16 + 6];
                float scalar_dist_7 = local_query_buffer[d * 16 + 7] - cell_centroids_partition[c * D + d * 16 + 7];
                float scalar_dist_8 = local_query_buffer[d * 16 + 8] - cell_centroids_partition[c * D + d * 16 + 8];
                float scalar_dist_9 = local_query_buffer[d * 16 + 9] - cell_centroids_partition[c * D + d * 16 + 9];
                float scalar_dist_10 = local_query_buffer[d * 16 + 10] - cell_centroids_partition[c * D + d * 16 + 10];
                float scalar_dist_11 = local_query_buffer[d * 16 + 11] - cell_centroids_partition[c * D + d * 16 + 11];
                float scalar_dist_12 = local_query_buffer[d * 16 + 12] - cell_centroids_partition[c * D + d * 16 + 12];
                float scalar_dist_13 = local_query_buffer[d * 16 + 13] - cell_centroids_partition[c * D + d * 16 + 13];
                float scalar_dist_14 = local_query_buffer[d * 16 + 14] - cell_centroids_partition[c * D + d * 16 + 14];
                float scalar_dist_15 = local_query_buffer[d * 16 + 15] - cell_centroids_partition[c * D + d * 16 + 15];

                float square_dist_0 = scalar_dist_0 * scalar_dist_0;
                float square_dist_1 = scalar_dist_1 * scalar_dist_1;
                float square_dist_2 = scalar_dist_2 * scalar_dist_2;
                float square_dist_3 = scalar_dist_3 * scalar_dist_3;
                float square_dist_4 = scalar_dist_4 * scalar_dist_4;
                float square_dist_5 = scalar_dist_5 * scalar_dist_5;
                float square_dist_6 = scalar_dist_6 * scalar_dist_6;
                float square_dist_7 = scalar_dist_7 * scalar_dist_7;
                float square_dist_8 = scalar_dist_8 * scalar_dist_8;
                float square_dist_9 = scalar_dist_9 * scalar_dist_9;
                float square_dist_10 = scalar_dist_10 * scalar_dist_10;
                float square_dist_11 = scalar_dist_11 * scalar_dist_11;
                float square_dist_12 = scalar_dist_12 * scalar_dist_12;
                float square_dist_13 = scalar_dist_13 * scalar_dist_13;
                float square_dist_14 = scalar_dist_14 * scalar_dist_14;
                float square_dist_15 = scalar_dist_15 * scalar_dist_15;

                distance += square_dist_0 + square_dist_1 + square_dist_2 + square_dist_3 + 
                    square_dist_4 + square_dist_5 + square_dist_6 + square_dist_7 + 
                    square_dist_8 + square_dist_9 + square_dist_10 + square_dist_11 + 
                    square_dist_12 + square_dist_13 + square_dist_14 + square_dist_15; 
            }

            dist_cell_ID_t out;
            out.dist = distance;
            out.cell_ID = start_cell_ID + c;

            s_partial_cell_PE_result.write(out);
        }
    }
}


// centroids_per_partition_last_PE must < centroids_per_partition
template<const int query_num, const int centroids_per_partition, 
    const int centroids_per_partition_last_PE, const int total_centriods>
void forward_cell_distance_tail(
    const int systolic_array_id,
    hls::stream<dist_cell_ID_t>& s_partial_cell_PE_result,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance_in,
    hls::stream<dist_cell_ID_t>& s_cell_distance_out) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        // compute distance and write results out
        for (int c = 0; c < centroids_per_partition_last_PE; c++) {

            // when the PE with larger ID finished computing, previous PE should already finished them
            for (int forward_iter = 0; forward_iter < systolic_array_id + 1; forward_iter++) {
#pragma HLS pipeline II=1
                dist_cell_ID_t out;
                if (forward_iter < systolic_array_id) {
                    out = s_partial_cell_distance_in.read();
                }
                else{
                    out = s_partial_cell_PE_result.read();
                }
                s_cell_distance_out.write(out);
            }
        }

        // forward the rest
        for (int c = 0; c < centroids_per_partition - centroids_per_partition_last_PE; c++) {
            for (int forward_iter = 0; forward_iter < systolic_array_id; forward_iter++) {
#pragma HLS pipeline II=1
                s_cell_distance_out.write(s_partial_cell_distance_in.read());
            }
        }
    }
}

// centroids_per_partition_last_PE must < centroids_per_partition
template<const int query_num, const int centroids_per_partition, 
    const int centroids_per_partition_last_PE, const int total_centriods>
void compute_cell_distance_tail_PE(
    const int systolic_array_id,
    hls::stream<float>& s_centroid_vectors_in,
    hls::stream<float>& s_query_vectors_in,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance_in,
    hls::stream<dist_cell_ID_t>& s_cell_distance_out) {
       
#pragma HLS dataflow

    hls::stream<dist_cell_ID_t> s_partial_cell_PE_result;
#pragma HLS stream variable=s_partial_cell_PE_result depth=8 

    compute_cell_distance_tail<query_num, centroids_per_partition, centroids_per_partition_last_PE, total_centriods>(
        systolic_array_id,
        s_centroid_vectors_in,
        s_query_vectors_in,
        s_partial_cell_PE_result);

    forward_cell_distance_tail<query_num, centroids_per_partition, centroids_per_partition_last_PE, total_centriods>(
        systolic_array_id,
        s_partial_cell_PE_result,
        s_partial_cell_distance_in,
        s_cell_distance_out);
}

template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float> &s_centroid_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<dist_cell_ID_t> &s_cell_distance) {
#pragma HLS inline

    hls::stream<float> s_query_vectors_forward[PE_NUM_CENTER_DIST_COMP_EVEN];
#pragma HLS stream variable=s_query_vectors_forward depth=8
    hls::stream<float> s_centroid_vectors_forward[PE_NUM_CENTER_DIST_COMP_EVEN];
#pragma HLS stream variable=s_centroid_vectors_forward depth=8

    hls::stream<dist_cell_ID_t> s_partial_cell_distance_forward[PE_NUM_CENTER_DIST_COMP_EVEN];
#pragma HLS stream variable=s_partial_cell_distance_forward depth=8

    // head
    compute_cell_distance_head_PE<QUERY_NUM, CENTROIDS_PER_PARTITION_EVEN, NLIST>(
        0,
        s_centroid_vectors,
        s_centroid_vectors_forward[0],
        s_query_vectors,
        s_query_vectors_forward[0],
        s_partial_cell_distance_forward[0]); 

    // middle 
    for (int s = 1; s < PE_NUM_CENTER_DIST_COMP_EVEN; s++) {
#pragma HLS UNROLL
        compute_cell_distance_middle_PE<QUERY_NUM, CENTROIDS_PER_PARTITION_EVEN, NLIST>(
            s,
            s_centroid_vectors_forward[s - 1],
            s_centroid_vectors_forward[s],
            s_query_vectors_forward[s - 1],
            s_query_vectors_forward[s],
            s_partial_cell_distance_forward[s - 1],
            s_partial_cell_distance_forward[s]);
    }

    // tail
    compute_cell_distance_tail_PE<QUERY_NUM, CENTROIDS_PER_PARTITION_EVEN, CENTROIDS_PER_PARTITION_LAST_PE, NLIST>(
        PE_NUM_CENTER_DIST_COMP_EVEN,
        s_centroid_vectors_forward[PE_NUM_CENTER_DIST_COMP_EVEN - 1],
        s_query_vectors_forward[PE_NUM_CENTER_DIST_COMP_EVEN - 1],
        s_partial_cell_distance_forward[PE_NUM_CENTER_DIST_COMP_EVEN - 1],
        s_cell_distance);
}