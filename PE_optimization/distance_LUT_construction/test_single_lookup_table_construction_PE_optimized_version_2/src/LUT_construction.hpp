#pragma once

#include "constants.hpp"
#include "types.hpp"

void compute_residual_vector(
    hls::stream<float> &s_center_vectors_table_construction_PE,
    float query_vector_local[M][D / M],
    float residual_center_vector[M][D / M]);

template<const int array_len>
float square_sum(float array[array_len]);

void construct_single_distance_LUT(
    float product_quantizer[M][K][D / M],
    float residual_center_vector[M][D / M],
    hls::stream<distance_LUT_PQ16_t>& s_result_table_construction_PE);

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_PE(
    hls::stream<float>& s_product_quantizer_init,
    hls::stream<struct_16_float_t>& s_center_vectors_table_construction_PE,
    hls::stream<struct_16_float_t>& s_query_vectors_table_construction_PE,
    hls::stream<distance_LUT_PQ16_t>& s_result_table_construction_PE);


void compute_residual_vector(
    hls::stream<float> &s_center_vectors_table_construction_PE,
    float query_vector_local[M][D / M],
    float residual_center_vector[M][D / M]) {
        
    float center_vector_local[M][D / M];

    for (int m = 0; m < M; m++) {
        for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
            center_vector_local[m][j] = s_center_vectors_table_construction_PE.read();
            residual_center_vector[m][j] = query_vector_local[m][j] - center_vector_local[m][j];
        }
    }
}

template<>
float square_sum<8>(float array[8]) {
#pragma HLS inline

    float result = 
        array[0] * array[0] + 
        array[1] * array[1] + 
        array[2] * array[2] + 
        array[3] * array[3] + 
        array[4] * array[4] + 
        array[5] * array[5] + 
        array[6] * array[6] + 
        array[7] * array[7];

    return result;
}


void construct_single_distance_LUT(
    float product_quantizer[M][K][D / M],
    float residual_center_vector[M][D / M],
    hls::stream<distance_LUT_PQ16_t>& s_result_table_construction_PE) {

    for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=1

        // the L1 diff between residual_center_vector annd sub-quantizers
        float L1_dist[M][D/M];
#pragma HLS array_partition variable=L1_dist complete

        for (int m = 0; m < M; m++) {
            for (int j = 0; j < D / M; j++) {
#pragma HLS UNROLL
                L1_dist[m][j] = residual_center_vector[m][j] - product_quantizer[m][k][j];
            }
        }

        distance_LUT_PQ16_t dist_row;

        // square distance
        dist_row.dist_0 = square_sum<8>(L1_dist[0]);
        dist_row.dist_1 = square_sum<8>(L1_dist[1]);
        dist_row.dist_2 = square_sum<8>(L1_dist[2]);
        dist_row.dist_3 = square_sum<8>(L1_dist[3]);
        dist_row.dist_4 = square_sum<8>(L1_dist[4]);
        dist_row.dist_5 = square_sum<8>(L1_dist[5]);
        dist_row.dist_6 = square_sum<8>(L1_dist[6]);
        dist_row.dist_7 = square_sum<8>(L1_dist[7]);
        dist_row.dist_8 = square_sum<8>(L1_dist[8]);
        dist_row.dist_9 = square_sum<8>(L1_dist[9]);
        dist_row.dist_10 = square_sum<8>(L1_dist[10]);
        dist_row.dist_11 = square_sum<8>(L1_dist[11]);
        dist_row.dist_12 = square_sum<8>(L1_dist[12]);
        dist_row.dist_13 = square_sum<8>(L1_dist[13]);
        dist_row.dist_14 = square_sum<8>(L1_dist[14]);
        dist_row.dist_15 = square_sum<8>(L1_dist[15]);
        
        s_result_table_construction_PE.write(dist_row);
    }
}

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_PE(
    hls::stream<float>& s_product_quantizer_init,
    hls::stream<struct_16_float_t>& s_center_vectors_table_construction_PE,
    hls::stream<float>& s_query_vectors_table_construction_PE,
    hls::stream<distance_LUT_PQ16_t>& s_result_table_construction_PE) {

    /* output format:
     *   lookup table dim: (K x M)
     *   sending first row, then second row, and so on...
     *   store in distance_LUT_PQ16_t, each represent an entire row (M=16)
     *   256 distance_LUT_PQ16_t is an entire lookup table
     */

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    // first and last dimension needs to be partitioned such that we can
    //    compute a row of product quantizer LUT in parallel
    float product_quantizer[M][K][D / M];
#pragma HLS array_partition variable=product_quantizer dim=1
#pragma HLS array_partition variable=product_quantizer dim=3
// #pragma HLS resource variable=product_quantizer core=RAM_2P_BRAM

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int m = 0; m < M; m++) {
        for (int k = 0; k < K; k++) {
            for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
                product_quantizer[m][k][j] = s_product_quantizer_init.read();
            }
        }
    }

    // store query and center vector into the format of M sub-vectors
    float query_vector_local[M][D / M];
#pragma HLS array_partition variable=query_vector_local complete

    float residual_center_vector[M][D / M]; // query_vector - center_vector
#pragma HLS array_partition variable=residual_center_vector complete

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vector, cut it into M su-vectors
        for (int m = 0; m < M; m++) {
            for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
                query_vector_local[m][j] = s_query_vectors_table_construction_PE.read();
            }
        }

        // for each nprobe, construct LUT
        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {

            // Note! Here the dimension is hard-coded, 8 iteration reads an center vector
            for (int i = 0; i < 8; i++) {
#pragma HLS pipeline II=1
                
                // partial center vector
                struct_16_float_t reg = s_center_vectors_table_construction_PE.read();

                residual_center_vector[i * 2][0] = query_vector_local[i * 2][0] - reg.val_0;
                residual_center_vector[i * 2][1] = query_vector_local[i * 2][1] - reg.val_1;
                residual_center_vector[i * 2][2] = query_vector_local[i * 2][2] - reg.val_2;
                residual_center_vector[i * 2][3] = query_vector_local[i * 2][3] - reg.val_3;
                residual_center_vector[i * 2][4] = query_vector_local[i * 2][4] - reg.val_4;
                residual_center_vector[i * 2][5] = query_vector_local[i * 2][5] - reg.val_5;
                residual_center_vector[i * 2][6] = query_vector_local[i * 2][6] - reg.val_6;
                residual_center_vector[i * 2][7] = query_vector_local[i * 2][7] - reg.val_7;

                residual_center_vector[i * 2 + 1][0] = query_vector_local[i * 2 + 1][0] - reg.val_8;
                residual_center_vector[i * 2 + 1][1] = query_vector_local[i * 2 + 1][1] - reg.val_9;
                residual_center_vector[i * 2 + 1][2] = query_vector_local[i * 2 + 1][2] - reg.val_10;
                residual_center_vector[i * 2 + 1][3] = query_vector_local[i * 2 + 1][3] - reg.val_11;
                residual_center_vector[i * 2 + 1][4] = query_vector_local[i * 2 + 1][4] - reg.val_12;
                residual_center_vector[i * 2 + 1][5] = query_vector_local[i * 2 + 1][5] - reg.val_13;
                residual_center_vector[i * 2 + 1][6] = query_vector_local[i * 2 + 1][6] - reg.val_14;
                residual_center_vector[i * 2 + 1][7] = query_vector_local[i * 2 + 1][7] - reg.val_15;
            }

            for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=1

                // the L1 diff between residual_center_vector annd sub-quantizers
                float L1_dist[M][D/M];
#pragma HLS array_partition variable=L1_dist complete

                for (int m = 0; m < M; m++) {
#pragma HLS UNROLL
                    for (int j = 0; j < D / M; j++) {
#pragma HLS UNROLL
                        L1_dist[m][j] = residual_center_vector[m][j] - product_quantizer[m][k][j];
                    }
                }

                distance_LUT_PQ16_t dist_row;

                // square distance
                dist_row.dist_0 = square_sum<8>(L1_dist[0]);
                dist_row.dist_1 = square_sum<8>(L1_dist[1]);
                dist_row.dist_2 = square_sum<8>(L1_dist[2]);
                dist_row.dist_3 = square_sum<8>(L1_dist[3]);
                dist_row.dist_4 = square_sum<8>(L1_dist[4]);
                dist_row.dist_5 = square_sum<8>(L1_dist[5]);
                dist_row.dist_6 = square_sum<8>(L1_dist[6]);
                dist_row.dist_7 = square_sum<8>(L1_dist[7]);
                dist_row.dist_8 = square_sum<8>(L1_dist[8]);
                dist_row.dist_9 = square_sum<8>(L1_dist[9]);
                dist_row.dist_10 = square_sum<8>(L1_dist[10]);
                dist_row.dist_11 = square_sum<8>(L1_dist[11]);
                dist_row.dist_12 = square_sum<8>(L1_dist[12]);
                dist_row.dist_13 = square_sum<8>(L1_dist[13]);
                dist_row.dist_14 = square_sum<8>(L1_dist[14]);
                dist_row.dist_15 = square_sum<8>(L1_dist[15]);
                
                s_result_table_construction_PE.write(dist_row);
            }
        }
    }
}
