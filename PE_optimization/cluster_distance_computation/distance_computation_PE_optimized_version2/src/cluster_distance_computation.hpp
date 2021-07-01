#pragma once

#include "constants.hpp"
#include "types.hpp"

template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float>& s_centroid_vectors,
    hls::stream<float>& s_query_vectors,
    hls::stream<dist_cell_ID_t>& s_cell_distance);

void parallel_L2_distance(
    float in_array_A[D], 
    ap_uint<64> in_array_B[D / 2], 
    float out_array[D]);

template<typename T, const int data_width, const int parallel_degree>
T parallel_add_tree(T array[data_width]);

void parallel_L2_distance(
    float in_array_A[D], 
    ap_uint<64> in_array_B[D / 2], 
    float out_array[D]) {
#pragma HLS inline
    // Note: the input and output array must already be partitioned

    for (int i = 0; i < D / 2; i++) {
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
float parallel_add_tree<float, 128, 2>(float array[128]) {
#pragma HLS inline
    // here, every level reduces number by 2 (parallel_degree)
    // Note: the input array must already be partitioned

    float intermediate_result_level1[64];
    float intermediate_result_level2[32];
    float intermediate_result_level3[16];
    float intermediate_result_level4[8];
    float intermediate_result_level5[4];
    float intermediate_result_level6[2];
    float intermediate_result;

#pragma HLS array_partition variable=intermediate_result_level1 complete
#pragma HLS array_partition variable=intermediate_result_level2 complete
#pragma HLS array_partition variable=intermediate_result_level3 complete
#pragma HLS array_partition variable=intermediate_result_level4 complete
#pragma HLS array_partition variable=intermediate_result_level5 complete
#pragma HLS array_partition variable=intermediate_result_level6 complete

    for (int i = 0; i < 64; i++) {
#pragma HLS UNROLL
        intermediate_result_level1[i] = array[2 * i] + array[2 * i + 1];
    }
    for (int i = 0; i < 32; i++) {
#pragma HLS UNROLL
        intermediate_result_level2[i] = 
            intermediate_result_level1[2 * i] + intermediate_result_level1[2 * i + 1];
    }
    for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
        intermediate_result_level3[i] = 
            intermediate_result_level2[2 * i] + intermediate_result_level2[2 * i + 1];
    }
    for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
        intermediate_result_level4[i] = 
            intermediate_result_level3[2 * i] + intermediate_result_level3[2 * i + 1];
    }
    for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
        intermediate_result_level5[i] = 
            intermediate_result_level4[2 * i] + intermediate_result_level4[2 * i + 1];
    }
    for (int i = 0; i < 2; i++) {
#pragma HLS UNROLL
        intermediate_result_level6[i] = 
            intermediate_result_level5[2 * i] + intermediate_result_level5[2 * i + 1];
    }
    intermediate_result = intermediate_result_level6[0] + intermediate_result_level6[1];

    return intermediate_result;
}

template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float>& s_centroid_vectors,
    hls::stream<float>& s_query_vectors,
    hls::stream<dist_cell_ID_t>& s_cell_distance) {

    // Given all vector quantizers, compute the distance to each of them

    // use ap_uint to maximize the memory usage percentage of URAM (64/72 bits)
    // use float or struct of 2 floats will double the resource usage, since HLS
    //   will treat them as separate arrays
    ap_uint<64> cell_centroids[NLIST][D / 2];
#pragma HLS array_partition variable=cell_centroids dim=2
#pragma HLS resource variable=cell_centroids core=RAM_2P_URAM

    float query_vector_buffer[D];
#pragma HLS array_partition variable=query_vector_buffer complete

    // init before any computation
    for (int c = 0; c < NLIST; c++) {
        for (int d = 0; d < D / 2; d++) {
#pragma HLS pipeline II=2
            float tmp_0_float = s_centroid_vectors.read();
            float tmp_1_float = s_centroid_vectors.read();
            ap_uint<32> tmp_0_ap = *((ap_uint<32>*) (&tmp_0_float));
            ap_uint<32> tmp_1_ap = *((ap_uint<32>*) (&tmp_1_float));
            cell_centroids[c][d].range(31, 0) = tmp_0_ap;
            cell_centroids[c][d].range(63, 32) = tmp_1_ap;
        }
    }

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vec
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            query_vector_buffer[d] = s_query_vectors.read();
        }

        // compute distance and write results out
        for (int c = 0; c < NLIST; c++) {
#pragma HLS pipeline II=1 

            float L2_distance_buffer[D];
#pragma HLS array_partition variable=L2_distance_buffer complete

            parallel_L2_distance(
                query_vector_buffer, 
                cell_centroids[c],
                L2_distance_buffer);

            dist_cell_ID_t out;
            out.cell_ID = c;
            out.dist = parallel_add_tree<float, 128, 2>(L2_distance_buffer);

            s_cell_distance.write(out);
        }
    }
}
