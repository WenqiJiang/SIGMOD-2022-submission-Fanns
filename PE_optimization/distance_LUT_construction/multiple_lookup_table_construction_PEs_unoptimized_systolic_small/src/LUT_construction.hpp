#pragma once 

#include "constants.hpp"
#include "types.hpp"

////////////////////     Function to call in top-level     ////////////////////

template<const int query_num>
void lookup_table_construction_wrapper(
    hls::stream<float> &s_PQ_quantizer_init,
    hls::stream<float> &s_center_vectors_lookup_PE,
    hls::stream<float> &s_query_vectors_lookup_PE,
    hls::stream<distance_LUT_PQ16_t> &s_distance_LUT);
    
////////////////////     Function to call in top-level     ////////////////////

template<const int query_num>
void center_vectors_dispatcher(
    hls::stream<float>& s_center_vectors_lookup_PE,
    hls::stream<float> (&s_center_vectors_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION]) {

    // Given an input stream of center vectors, interleave it to all 
    //   distance table construction PEs in a round-robin manner 
    //   e.g., 4 PEs, vector 0,4,8 -> PE0, 1,5,9 -> PE1, etc.
    for (int query_id = 0; query_id < query_num; query_id++) {

        // first, interleave the common part of all PEs (decided by the PE of smaller scanned cells)
        for (int interleave_iter = 0; interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE_SMALLER; interleave_iter++) {

            for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION; s++) {

                for (int n = 0; n < D; n++) {
#pragma HLS pipeline II=1
                    s_center_vectors_table_construction_PE[s].write(s_center_vectors_lookup_PE.read());
                }
            }
        }

        // then, interleave the rest workload
        for (int interleave_iter = 0; interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE_LARGER - NPROBE_PER_TABLE_CONSTRUCTION_PE_SMALLER; interleave_iter++) {

            for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION_LARGER; s++) {

                for (int n = 0; n < D; n++) {
#pragma HLS pipeline II=1
                    s_center_vectors_table_construction_PE[s].write(s_center_vectors_lookup_PE.read());
                }
            }
        }
    }
}


template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_head_PE(
    hls::stream<float>& s_PQ_quantizer_init_in,
    hls::stream<float>& s_PQ_quantizer_init_out,
    hls::stream<float>& s_center_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_out,
    hls::stream<distance_LUT_PQ16_t>& s_partial_result_table_construction_PE) {

    /* output format:
     *   lookup table dim: (K x M)
     *   sending first row, then second row, and so on...
     *   store in distance_LUT_PQ16_t, each represent an entire row (M=16)
     *   256 distance_LUT_PQ16_t is an entire lookup table
     */

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    float sub_quantizer_0[K * (D / M)];
    float sub_quantizer_1[K * (D / M)];
    float sub_quantizer_2[K * (D / M)];
    float sub_quantizer_3[K * (D / M)];
    float sub_quantizer_4[K * (D / M)];
    float sub_quantizer_5[K * (D / M)];
    float sub_quantizer_6[K * (D / M)];
    float sub_quantizer_7[K * (D / M)];
    float sub_quantizer_8[K * (D / M)];
    float sub_quantizer_9[K * (D / M)];
    float sub_quantizer_10[K * (D / M)];
    float sub_quantizer_11[K * (D / M)];
    float sub_quantizer_12[K * (D / M)];
    float sub_quantizer_13[K * (D / M)];
    float sub_quantizer_14[K * (D / M)];
    float sub_quantizer_15[K * (D / M)];
#pragma HLS resource variable=sub_quantizer_0 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_1 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_2 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_3 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_4 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_5 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_6 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_7 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_8 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_9 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_10 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_11 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_12 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_13 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_14 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_15 core=RAM_2P_URAM

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_0[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_1[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_2[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_3[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_4[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_5[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_6[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_7[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_8[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_9[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_10[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_11[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_12[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_13[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_14[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_15[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }


    float query_vector_local[D];
    float center_vector_local[D];
    float residual_center_vector[D]; // query_vector - center_vector
#pragma HLS array_partition variable=residual_center_vector block factor=16

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vector
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors_table_construction_PE_in.read();
            query_vector_local[d] = reg;
            s_query_vectors_table_construction_PE_out.write(reg);
        }

        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {

            // load center vector
            residual_center_vectors:
            for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
                center_vector_local[d] = s_center_vectors_table_construction_PE_in.read();
                residual_center_vector[d] = query_vector_local[d] - center_vector_local[d];
            }

            // construct distance lookup table
            single_row_lookup_table_construction:
            for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=16
                float dist_0[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_1[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_2[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_3[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_4[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_5[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_6[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_7[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_8[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_9[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_10[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_11[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_12[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_13[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_14[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_15[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};

                compute_L1_distance: 
                for (int sub_q_col = 0; sub_q_col < D / M; sub_q_col++) {
// #pragma HLS UNROLL factor=2
                    // can HLS identify residual_center_vector can be accessed in parallel? 
                    dist_0[sub_q_col] = 
                        residual_center_vector[0 * (D / M) + sub_q_col] - 
                        sub_quantizer_0[k * (D / M) + sub_q_col];
                    dist_1[sub_q_col] = 
                        residual_center_vector[1 * (D / M) + sub_q_col] - 
                        sub_quantizer_1[k * (D / M) + sub_q_col];
                    dist_2[sub_q_col] = 
                        residual_center_vector[2 * (D / M) + sub_q_col] - 
                        sub_quantizer_2[k * (D / M) + sub_q_col];
                    dist_3[sub_q_col] = 
                        residual_center_vector[3 * (D / M) + sub_q_col] - 
                        sub_quantizer_3[k * (D / M) + sub_q_col];
                    dist_4[sub_q_col] = 
                        residual_center_vector[4 * (D / M) + sub_q_col] - 
                        sub_quantizer_4[k * (D / M) + sub_q_col];
                    dist_5[sub_q_col] = 
                        residual_center_vector[5 * (D / M) + sub_q_col] - 
                        sub_quantizer_5[k * (D / M) + sub_q_col];
                    dist_6[sub_q_col] = 
                        residual_center_vector[6 * (D / M) + sub_q_col] - 
                        sub_quantizer_6[k * (D / M) + sub_q_col];
                    dist_7[sub_q_col] = 
                        residual_center_vector[7 * (D / M) + sub_q_col] - 
                        sub_quantizer_7[k * (D / M) + sub_q_col];
                    dist_8[sub_q_col] = 
                        residual_center_vector[8 * (D / M) + sub_q_col] - 
                        sub_quantizer_8[k * (D / M) + sub_q_col];
                    dist_9[sub_q_col] = 
                        residual_center_vector[9 * (D / M) + sub_q_col] - 
                        sub_quantizer_9[k * (D / M) + sub_q_col];
                    dist_10[sub_q_col] = 
                        residual_center_vector[10 * (D / M) + sub_q_col] - 
                        sub_quantizer_10[k * (D / M) + sub_q_col];
                    dist_11[sub_q_col] = 
                        residual_center_vector[11 * (D / M) + sub_q_col] - 
                        sub_quantizer_11[k * (D / M) + sub_q_col];
                    dist_12[sub_q_col] = 
                        residual_center_vector[12 * (D / M) + sub_q_col] - 
                        sub_quantizer_12[k * (D / M) + sub_q_col];
                    dist_13[sub_q_col] = 
                        residual_center_vector[13 * (D / M) + sub_q_col] - 
                        sub_quantizer_13[k * (D / M) + sub_q_col];
                    dist_14[sub_q_col] = 
                        residual_center_vector[14 * (D / M) + sub_q_col] - 
                        sub_quantizer_14[k * (D / M) + sub_q_col];
                    dist_15[sub_q_col] = 
                        residual_center_vector[15 * (D / M) + sub_q_col] - 
                        sub_quantizer_15[k * (D / M) + sub_q_col];
                }

                distance_LUT_PQ16_t dist_row;
                // square distance
                dist_row.dist_0 = 
                    (dist_0[0] * dist_0[0]) + (dist_0[1] * dist_0[1]) +
                    (dist_0[2] * dist_0[2]) + (dist_0[3] * dist_0[3]) +
                    (dist_0[4] * dist_0[4]) + (dist_0[5] * dist_0[5]) + 
                    (dist_0[6] * dist_0[6]) + (dist_0[7] * dist_0[7]);
                dist_row.dist_1 = 
                    (dist_1[0] * dist_1[0]) + (dist_1[1] * dist_1[1]) +
                    (dist_1[2] * dist_1[2]) + (dist_1[3] * dist_1[3]) +
                    (dist_1[4] * dist_1[4]) + (dist_1[5] * dist_1[5]) + 
                    (dist_1[6] * dist_1[6]) + (dist_1[7] * dist_1[7]);
                dist_row.dist_2 = 
                    (dist_2[0] * dist_2[0]) + (dist_2[1] * dist_2[1]) +
                    (dist_2[2] * dist_2[2]) + (dist_2[3] * dist_2[3]) +
                    (dist_2[4] * dist_2[4]) + (dist_2[5] * dist_2[5]) + 
                    (dist_2[6] * dist_2[6]) + (dist_2[7] * dist_2[7]);
                dist_row.dist_3 = 
                    (dist_3[0] * dist_3[0]) + (dist_3[1] * dist_3[1]) +
                    (dist_3[2] * dist_3[2]) + (dist_3[3] * dist_3[3]) +
                    (dist_3[4] * dist_3[4]) + (dist_3[5] * dist_3[5]) + 
                    (dist_3[6] * dist_3[6]) + (dist_3[7] * dist_3[7]);
                dist_row.dist_4 = 
                    (dist_4[0] * dist_4[0]) + (dist_4[1] * dist_4[1]) +
                    (dist_4[2] * dist_4[2]) + (dist_4[3] * dist_4[3]) +
                    (dist_4[4] * dist_4[4]) + (dist_4[5] * dist_4[5]) + 
                    (dist_4[6] * dist_4[6]) + (dist_4[7] * dist_4[7]);
                dist_row.dist_5 = 
                    (dist_5[0] * dist_5[0]) + (dist_5[1] * dist_5[1]) +
                    (dist_5[2] * dist_5[2]) + (dist_5[3] * dist_5[3]) +
                    (dist_5[4] * dist_5[4]) + (dist_5[5] * dist_5[5]) + 
                    (dist_5[6] * dist_5[6]) + (dist_5[7] * dist_5[7]);
                dist_row.dist_6 = 
                    (dist_6[0] * dist_6[0]) + (dist_6[1] * dist_6[1]) +
                    (dist_6[2] * dist_6[2]) + (dist_6[3] * dist_6[3]) +
                    (dist_6[4] * dist_6[4]) + (dist_6[5] * dist_6[5]) + 
                    (dist_6[6] * dist_6[6]) + (dist_6[7] * dist_6[7]);
                dist_row.dist_7 = 
                    (dist_7[0] * dist_7[0]) + (dist_7[1] * dist_7[1]) +
                    (dist_7[2] * dist_7[2]) + (dist_7[3] * dist_7[3]) +
                    (dist_7[4] * dist_7[4]) + (dist_7[5] * dist_7[5]) + 
                    (dist_7[6] * dist_7[6]) + (dist_7[7] * dist_7[7]);
                dist_row.dist_8 = 
                    (dist_8[0] * dist_8[0]) + (dist_8[1] * dist_8[1]) +
                    (dist_8[2] * dist_8[2]) + (dist_8[3] * dist_8[3]) +
                    (dist_8[4] * dist_8[4]) + (dist_8[5] * dist_8[5]) + 
                    (dist_8[6] * dist_8[6]) + (dist_8[7] * dist_8[7]);
                dist_row.dist_9 = 
                    (dist_9[0] * dist_9[0]) + (dist_9[1] * dist_9[1]) +
                    (dist_9[2] * dist_9[2]) + (dist_9[3] * dist_9[3]) +
                    (dist_9[4] * dist_9[4]) + (dist_9[5] * dist_9[5]) + 
                    (dist_9[6] * dist_9[6]) + (dist_9[7] * dist_9[7]);
                dist_row.dist_10 = 
                    (dist_10[0] * dist_10[0]) + (dist_10[1] * dist_10[1]) +
                    (dist_10[2] * dist_10[2]) + (dist_10[3] * dist_10[3]) +
                    (dist_10[4] * dist_10[4]) + (dist_10[5] * dist_10[5]) + 
                    (dist_10[6] * dist_10[6]) + (dist_10[7] * dist_10[7]);
                dist_row.dist_11 = 
                    (dist_11[0] * dist_11[0]) + (dist_11[1] * dist_11[1]) +
                    (dist_11[2] * dist_11[2]) + (dist_11[3] * dist_11[3]) +
                    (dist_11[4] * dist_11[4]) + (dist_11[5] * dist_11[5]) + 
                    (dist_11[6] * dist_11[6]) + (dist_11[7] * dist_11[7]);
                dist_row.dist_12 = 
                    (dist_12[0] * dist_12[0]) + (dist_12[1] * dist_12[1]) +
                    (dist_12[2] * dist_12[2]) + (dist_12[3] * dist_12[3]) +
                    (dist_12[4] * dist_12[4]) + (dist_12[5] * dist_12[5]) + 
                    (dist_12[6] * dist_12[6]) + (dist_12[7] * dist_12[7]);
                dist_row.dist_13 = 
                    (dist_13[0] * dist_13[0]) + (dist_13[1] * dist_13[1]) +
                    (dist_13[2] * dist_13[2]) + (dist_13[3] * dist_13[3]) +
                    (dist_13[4] * dist_13[4]) + (dist_13[5] * dist_13[5]) + 
                    (dist_13[6] * dist_13[6]) + (dist_13[7] * dist_13[7]);
                dist_row.dist_14 = 
                    (dist_14[0] * dist_14[0]) + (dist_14[1] * dist_14[1]) +
                    (dist_14[2] * dist_14[2]) + (dist_14[3] * dist_14[3]) +
                    (dist_14[4] * dist_14[4]) + (dist_14[5] * dist_14[5]) + 
                    (dist_14[6] * dist_14[6]) + (dist_14[7] * dist_14[7]);
                dist_row.dist_15 = 
                    (dist_15[0] * dist_15[0]) + (dist_15[1] * dist_15[1]) +
                    (dist_15[2] * dist_15[2]) + (dist_15[3] * dist_15[3]) +
                    (dist_15[4] * dist_15[4]) + (dist_15[5] * dist_15[5]) + 
                    (dist_15[6] * dist_15[6]) + (dist_15[7] * dist_15[7]);
                
                s_partial_result_table_construction_PE.write(dist_row);
            }
        }
    }
}



template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_compute_midlle(
    hls::stream<float>& s_PQ_quantizer_init_in,
    hls::stream<float>& s_PQ_quantizer_init_out,
    hls::stream<float>& s_center_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_out,
    hls::stream<distance_LUT_PQ16_t>& s_partial_result_table_construction_PE) {

    /* output format:
     *   lookup table dim: (K x M)
     *   sending first row, then second row, and so on...
     *   store in distance_LUT_PQ16_t, each represent an entire row (M=16)
     *   256 distance_LUT_PQ16_t is an entire lookup table
     */

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    float sub_quantizer_0[K * (D / M)];
    float sub_quantizer_1[K * (D / M)];
    float sub_quantizer_2[K * (D / M)];
    float sub_quantizer_3[K * (D / M)];
    float sub_quantizer_4[K * (D / M)];
    float sub_quantizer_5[K * (D / M)];
    float sub_quantizer_6[K * (D / M)];
    float sub_quantizer_7[K * (D / M)];
    float sub_quantizer_8[K * (D / M)];
    float sub_quantizer_9[K * (D / M)];
    float sub_quantizer_10[K * (D / M)];
    float sub_quantizer_11[K * (D / M)];
    float sub_quantizer_12[K * (D / M)];
    float sub_quantizer_13[K * (D / M)];
    float sub_quantizer_14[K * (D / M)];
    float sub_quantizer_15[K * (D / M)];
#pragma HLS resource variable=sub_quantizer_0 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_1 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_2 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_3 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_4 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_5 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_6 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_7 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_8 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_9 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_10 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_11 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_12 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_13 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_14 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_15 core=RAM_2P_URAM

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_0[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_1[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_2[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_3[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_4[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_5[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_6[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_7[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_8[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_9[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_10[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_11[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_12[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_13[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_14[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_15[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }


    float query_vector_local[D];
    float center_vector_local[D];
    float residual_center_vector[D]; // query_vector - center_vector
#pragma HLS array_partition variable=residual_center_vector block factor=16

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vector
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors_table_construction_PE_in.read();
            query_vector_local[d] = reg;
            s_query_vectors_table_construction_PE_out.write(reg);
        }

        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {

            // load center vector
            residual_center_vectors:
            for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
                center_vector_local[d] = s_center_vectors_table_construction_PE_in.read();
                residual_center_vector[d] = query_vector_local[d] - center_vector_local[d];
            }

            // construct distance lookup table
            single_row_lookup_table_construction:
            for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=16
                float dist_0[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_1[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_2[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_3[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_4[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_5[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_6[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_7[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_8[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_9[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_10[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_11[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_12[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_13[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_14[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_15[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};

                compute_L1_distance: 
                for (int sub_q_col = 0; sub_q_col < D / M; sub_q_col++) {
// #pragma HLS UNROLL factor=2
                    // can HLS identify residual_center_vector can be accessed in parallel? 
                    dist_0[sub_q_col] = 
                        residual_center_vector[0 * (D / M) + sub_q_col] - 
                        sub_quantizer_0[k * (D / M) + sub_q_col];
                    dist_1[sub_q_col] = 
                        residual_center_vector[1 * (D / M) + sub_q_col] - 
                        sub_quantizer_1[k * (D / M) + sub_q_col];
                    dist_2[sub_q_col] = 
                        residual_center_vector[2 * (D / M) + sub_q_col] - 
                        sub_quantizer_2[k * (D / M) + sub_q_col];
                    dist_3[sub_q_col] = 
                        residual_center_vector[3 * (D / M) + sub_q_col] - 
                        sub_quantizer_3[k * (D / M) + sub_q_col];
                    dist_4[sub_q_col] = 
                        residual_center_vector[4 * (D / M) + sub_q_col] - 
                        sub_quantizer_4[k * (D / M) + sub_q_col];
                    dist_5[sub_q_col] = 
                        residual_center_vector[5 * (D / M) + sub_q_col] - 
                        sub_quantizer_5[k * (D / M) + sub_q_col];
                    dist_6[sub_q_col] = 
                        residual_center_vector[6 * (D / M) + sub_q_col] - 
                        sub_quantizer_6[k * (D / M) + sub_q_col];
                    dist_7[sub_q_col] = 
                        residual_center_vector[7 * (D / M) + sub_q_col] - 
                        sub_quantizer_7[k * (D / M) + sub_q_col];
                    dist_8[sub_q_col] = 
                        residual_center_vector[8 * (D / M) + sub_q_col] - 
                        sub_quantizer_8[k * (D / M) + sub_q_col];
                    dist_9[sub_q_col] = 
                        residual_center_vector[9 * (D / M) + sub_q_col] - 
                        sub_quantizer_9[k * (D / M) + sub_q_col];
                    dist_10[sub_q_col] = 
                        residual_center_vector[10 * (D / M) + sub_q_col] - 
                        sub_quantizer_10[k * (D / M) + sub_q_col];
                    dist_11[sub_q_col] = 
                        residual_center_vector[11 * (D / M) + sub_q_col] - 
                        sub_quantizer_11[k * (D / M) + sub_q_col];
                    dist_12[sub_q_col] = 
                        residual_center_vector[12 * (D / M) + sub_q_col] - 
                        sub_quantizer_12[k * (D / M) + sub_q_col];
                    dist_13[sub_q_col] = 
                        residual_center_vector[13 * (D / M) + sub_q_col] - 
                        sub_quantizer_13[k * (D / M) + sub_q_col];
                    dist_14[sub_q_col] = 
                        residual_center_vector[14 * (D / M) + sub_q_col] - 
                        sub_quantizer_14[k * (D / M) + sub_q_col];
                    dist_15[sub_q_col] = 
                        residual_center_vector[15 * (D / M) + sub_q_col] - 
                        sub_quantizer_15[k * (D / M) + sub_q_col];
                }

                distance_LUT_PQ16_t dist_row;
                // square distance
                dist_row.dist_0 = 
                    (dist_0[0] * dist_0[0]) + (dist_0[1] * dist_0[1]) +
                    (dist_0[2] * dist_0[2]) + (dist_0[3] * dist_0[3]) +
                    (dist_0[4] * dist_0[4]) + (dist_0[5] * dist_0[5]) + 
                    (dist_0[6] * dist_0[6]) + (dist_0[7] * dist_0[7]);
                dist_row.dist_1 = 
                    (dist_1[0] * dist_1[0]) + (dist_1[1] * dist_1[1]) +
                    (dist_1[2] * dist_1[2]) + (dist_1[3] * dist_1[3]) +
                    (dist_1[4] * dist_1[4]) + (dist_1[5] * dist_1[5]) + 
                    (dist_1[6] * dist_1[6]) + (dist_1[7] * dist_1[7]);
                dist_row.dist_2 = 
                    (dist_2[0] * dist_2[0]) + (dist_2[1] * dist_2[1]) +
                    (dist_2[2] * dist_2[2]) + (dist_2[3] * dist_2[3]) +
                    (dist_2[4] * dist_2[4]) + (dist_2[5] * dist_2[5]) + 
                    (dist_2[6] * dist_2[6]) + (dist_2[7] * dist_2[7]);
                dist_row.dist_3 = 
                    (dist_3[0] * dist_3[0]) + (dist_3[1] * dist_3[1]) +
                    (dist_3[2] * dist_3[2]) + (dist_3[3] * dist_3[3]) +
                    (dist_3[4] * dist_3[4]) + (dist_3[5] * dist_3[5]) + 
                    (dist_3[6] * dist_3[6]) + (dist_3[7] * dist_3[7]);
                dist_row.dist_4 = 
                    (dist_4[0] * dist_4[0]) + (dist_4[1] * dist_4[1]) +
                    (dist_4[2] * dist_4[2]) + (dist_4[3] * dist_4[3]) +
                    (dist_4[4] * dist_4[4]) + (dist_4[5] * dist_4[5]) + 
                    (dist_4[6] * dist_4[6]) + (dist_4[7] * dist_4[7]);
                dist_row.dist_5 = 
                    (dist_5[0] * dist_5[0]) + (dist_5[1] * dist_5[1]) +
                    (dist_5[2] * dist_5[2]) + (dist_5[3] * dist_5[3]) +
                    (dist_5[4] * dist_5[4]) + (dist_5[5] * dist_5[5]) + 
                    (dist_5[6] * dist_5[6]) + (dist_5[7] * dist_5[7]);
                dist_row.dist_6 = 
                    (dist_6[0] * dist_6[0]) + (dist_6[1] * dist_6[1]) +
                    (dist_6[2] * dist_6[2]) + (dist_6[3] * dist_6[3]) +
                    (dist_6[4] * dist_6[4]) + (dist_6[5] * dist_6[5]) + 
                    (dist_6[6] * dist_6[6]) + (dist_6[7] * dist_6[7]);
                dist_row.dist_7 = 
                    (dist_7[0] * dist_7[0]) + (dist_7[1] * dist_7[1]) +
                    (dist_7[2] * dist_7[2]) + (dist_7[3] * dist_7[3]) +
                    (dist_7[4] * dist_7[4]) + (dist_7[5] * dist_7[5]) + 
                    (dist_7[6] * dist_7[6]) + (dist_7[7] * dist_7[7]);
                dist_row.dist_8 = 
                    (dist_8[0] * dist_8[0]) + (dist_8[1] * dist_8[1]) +
                    (dist_8[2] * dist_8[2]) + (dist_8[3] * dist_8[3]) +
                    (dist_8[4] * dist_8[4]) + (dist_8[5] * dist_8[5]) + 
                    (dist_8[6] * dist_8[6]) + (dist_8[7] * dist_8[7]);
                dist_row.dist_9 = 
                    (dist_9[0] * dist_9[0]) + (dist_9[1] * dist_9[1]) +
                    (dist_9[2] * dist_9[2]) + (dist_9[3] * dist_9[3]) +
                    (dist_9[4] * dist_9[4]) + (dist_9[5] * dist_9[5]) + 
                    (dist_9[6] * dist_9[6]) + (dist_9[7] * dist_9[7]);
                dist_row.dist_10 = 
                    (dist_10[0] * dist_10[0]) + (dist_10[1] * dist_10[1]) +
                    (dist_10[2] * dist_10[2]) + (dist_10[3] * dist_10[3]) +
                    (dist_10[4] * dist_10[4]) + (dist_10[5] * dist_10[5]) + 
                    (dist_10[6] * dist_10[6]) + (dist_10[7] * dist_10[7]);
                dist_row.dist_11 = 
                    (dist_11[0] * dist_11[0]) + (dist_11[1] * dist_11[1]) +
                    (dist_11[2] * dist_11[2]) + (dist_11[3] * dist_11[3]) +
                    (dist_11[4] * dist_11[4]) + (dist_11[5] * dist_11[5]) + 
                    (dist_11[6] * dist_11[6]) + (dist_11[7] * dist_11[7]);
                dist_row.dist_12 = 
                    (dist_12[0] * dist_12[0]) + (dist_12[1] * dist_12[1]) +
                    (dist_12[2] * dist_12[2]) + (dist_12[3] * dist_12[3]) +
                    (dist_12[4] * dist_12[4]) + (dist_12[5] * dist_12[5]) + 
                    (dist_12[6] * dist_12[6]) + (dist_12[7] * dist_12[7]);
                dist_row.dist_13 = 
                    (dist_13[0] * dist_13[0]) + (dist_13[1] * dist_13[1]) +
                    (dist_13[2] * dist_13[2]) + (dist_13[3] * dist_13[3]) +
                    (dist_13[4] * dist_13[4]) + (dist_13[5] * dist_13[5]) + 
                    (dist_13[6] * dist_13[6]) + (dist_13[7] * dist_13[7]);
                dist_row.dist_14 = 
                    (dist_14[0] * dist_14[0]) + (dist_14[1] * dist_14[1]) +
                    (dist_14[2] * dist_14[2]) + (dist_14[3] * dist_14[3]) +
                    (dist_14[4] * dist_14[4]) + (dist_14[5] * dist_14[5]) + 
                    (dist_14[6] * dist_14[6]) + (dist_14[7] * dist_14[7]);
                dist_row.dist_15 = 
                    (dist_15[0] * dist_15[0]) + (dist_15[1] * dist_15[1]) +
                    (dist_15[2] * dist_15[2]) + (dist_15[3] * dist_15[3]) +
                    (dist_15[4] * dist_15[4]) + (dist_15[5] * dist_15[5]) + 
                    (dist_15[6] * dist_15[6]) + (dist_15[7] * dist_15[7]);
                
                s_partial_result_table_construction_PE.write(dist_row);
            }
        }
    }
}


template<const int query_num>
void lookup_table_construction_forward_middle(
    const int systolic_array_id,
    hls::stream<distance_LUT_PQ16_t> &s_partial_result_table_construction_PE,
    hls::stream<distance_LUT_PQ16_t> &s_partial_result_table_construction_forward_in,
    hls::stream<distance_LUT_PQ16_t> &s_partial_result_table_construction_forward_out) {

    //////////  NOTE: the order of output LUT must be consistent of the center vector input  ///////// 
    // e.g., say nprobe=17, PE_num=4, then the first 3 PEs compute 5 tables while the last compute 2
    //  first 2 rounds 4 PEs, last 3 rounds 3 PEs
    // PE 0: 0, 4, 8, 11, 14
    // PE 1: 1, 5, 9, 12, 15
    // PE 2: 2, 6, 10, 13, 16
    // PE 3: 3, 7

    for (int query_id = 0; query_id < query_num; query_id++) {
        
        for (int interleave_iter = 0; interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE_LARGER; interleave_iter++) {

            // forward head / midlle PEs
            for (int s = 0; s < systolic_array_id; s++) {
                // each lookup table: K rows
                for (int t = 0; t < K; t++) {
#pragma HLS pipeline II=1
                    s_partial_result_table_construction_forward_out.write(s_partial_result_table_construction_forward_in.read());
                }
            }
            // result from the current PE
            for (int t = 0; t < K; t++) {
#pragma HLS pipeline II=1
                s_partial_result_table_construction_forward_out.write(s_partial_result_table_construction_PE.read());
            }
        }
    }
}



template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_middle_PE(
    const int systolic_array_id,
    hls::stream<float>& s_PQ_quantizer_init_in,
    hls::stream<float>& s_PQ_quantizer_init_out,
    hls::stream<float>& s_center_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_out,
    hls::stream<distance_LUT_PQ16_t> &s_partial_result_table_construction_forward_in,
    hls::stream<distance_LUT_PQ16_t> &s_partial_result_table_construction_forward_out) {

#pragma HLS dataflow

    hls::stream<distance_LUT_PQ16_t> s_partial_result_table_construction_PE;
#pragma HLS stream variable=s_partial_result_table_construction_PE depth=512

    lookup_table_construction_compute_midlle<query_num, nprobe_per_PE>(
        s_PQ_quantizer_init_in,
        s_PQ_quantizer_init_out,
        s_center_vectors_table_construction_PE_in,
        s_query_vectors_table_construction_PE_in,
        s_query_vectors_table_construction_PE_out,
        s_partial_result_table_construction_PE);

    lookup_table_construction_forward_middle<query_num>(
        systolic_array_id,
        s_partial_result_table_construction_PE,
        s_partial_result_table_construction_forward_in,
        s_partial_result_table_construction_forward_out);
}

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_compute_tail(
    hls::stream<float>& s_PQ_quantizer_init_in,
    hls::stream<float>& s_center_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_in,
    hls::stream<distance_LUT_PQ16_t>& s_partial_result_table_construction_PE) {

    /* output format:
     *   lookup table dim: (K x M)
     *   sending first row, then second row, and so on...
     *   store in distance_LUT_PQ16_t, each represent an entire row (M=16)
     *   256 distance_LUT_PQ16_t is an entire lookup table
     */

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    float sub_quantizer_0[K * (D / M)];
    float sub_quantizer_1[K * (D / M)];
    float sub_quantizer_2[K * (D / M)];
    float sub_quantizer_3[K * (D / M)];
    float sub_quantizer_4[K * (D / M)];
    float sub_quantizer_5[K * (D / M)];
    float sub_quantizer_6[K * (D / M)];
    float sub_quantizer_7[K * (D / M)];
    float sub_quantizer_8[K * (D / M)];
    float sub_quantizer_9[K * (D / M)];
    float sub_quantizer_10[K * (D / M)];
    float sub_quantizer_11[K * (D / M)];
    float sub_quantizer_12[K * (D / M)];
    float sub_quantizer_13[K * (D / M)];
    float sub_quantizer_14[K * (D / M)];
    float sub_quantizer_15[K * (D / M)];
#pragma HLS resource variable=sub_quantizer_0 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_1 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_2 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_3 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_4 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_5 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_6 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_7 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_8 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_9 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_10 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_11 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_12 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_13 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_14 core=RAM_2P_URAM
#pragma HLS resource variable=sub_quantizer_15 core=RAM_2P_URAM

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_0[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_1[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_2[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_3[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_4[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_5[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_6[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_7[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_8[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_9[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_10[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_11[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_12[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_13[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_14[i] = reg;
    }
    for (int i = 0; i < K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer_15[i] = reg;
    }


    float query_vector_local[D];
    float center_vector_local[D];
    float residual_center_vector[D]; // query_vector - center_vector
#pragma HLS array_partition variable=residual_center_vector block factor=16

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vector
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors_table_construction_PE_in.read();
            query_vector_local[d] = reg;
        }

        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {

            // load center vector
            residual_center_vectors:
            for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
                center_vector_local[d] = s_center_vectors_table_construction_PE_in.read();
                residual_center_vector[d] = query_vector_local[d] - center_vector_local[d];
            }

            // construct distance lookup table
            single_row_lookup_table_construction:
            for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=16
                float dist_0[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_1[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_2[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_3[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_4[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_5[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_6[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_7[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_8[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_9[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_10[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_11[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_12[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_13[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_14[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};
                float dist_15[D / M] = {0, 0, 0, 0, 0, 0, 0, 0};

                compute_L1_distance: 
                for (int sub_q_col = 0; sub_q_col < D / M; sub_q_col++) {
// #pragma HLS UNROLL factor=2
                    // can HLS identify residual_center_vector can be accessed in parallel? 
                    dist_0[sub_q_col] = 
                        residual_center_vector[0 * (D / M) + sub_q_col] - 
                        sub_quantizer_0[k * (D / M) + sub_q_col];
                    dist_1[sub_q_col] = 
                        residual_center_vector[1 * (D / M) + sub_q_col] - 
                        sub_quantizer_1[k * (D / M) + sub_q_col];
                    dist_2[sub_q_col] = 
                        residual_center_vector[2 * (D / M) + sub_q_col] - 
                        sub_quantizer_2[k * (D / M) + sub_q_col];
                    dist_3[sub_q_col] = 
                        residual_center_vector[3 * (D / M) + sub_q_col] - 
                        sub_quantizer_3[k * (D / M) + sub_q_col];
                    dist_4[sub_q_col] = 
                        residual_center_vector[4 * (D / M) + sub_q_col] - 
                        sub_quantizer_4[k * (D / M) + sub_q_col];
                    dist_5[sub_q_col] = 
                        residual_center_vector[5 * (D / M) + sub_q_col] - 
                        sub_quantizer_5[k * (D / M) + sub_q_col];
                    dist_6[sub_q_col] = 
                        residual_center_vector[6 * (D / M) + sub_q_col] - 
                        sub_quantizer_6[k * (D / M) + sub_q_col];
                    dist_7[sub_q_col] = 
                        residual_center_vector[7 * (D / M) + sub_q_col] - 
                        sub_quantizer_7[k * (D / M) + sub_q_col];
                    dist_8[sub_q_col] = 
                        residual_center_vector[8 * (D / M) + sub_q_col] - 
                        sub_quantizer_8[k * (D / M) + sub_q_col];
                    dist_9[sub_q_col] = 
                        residual_center_vector[9 * (D / M) + sub_q_col] - 
                        sub_quantizer_9[k * (D / M) + sub_q_col];
                    dist_10[sub_q_col] = 
                        residual_center_vector[10 * (D / M) + sub_q_col] - 
                        sub_quantizer_10[k * (D / M) + sub_q_col];
                    dist_11[sub_q_col] = 
                        residual_center_vector[11 * (D / M) + sub_q_col] - 
                        sub_quantizer_11[k * (D / M) + sub_q_col];
                    dist_12[sub_q_col] = 
                        residual_center_vector[12 * (D / M) + sub_q_col] - 
                        sub_quantizer_12[k * (D / M) + sub_q_col];
                    dist_13[sub_q_col] = 
                        residual_center_vector[13 * (D / M) + sub_q_col] - 
                        sub_quantizer_13[k * (D / M) + sub_q_col];
                    dist_14[sub_q_col] = 
                        residual_center_vector[14 * (D / M) + sub_q_col] - 
                        sub_quantizer_14[k * (D / M) + sub_q_col];
                    dist_15[sub_q_col] = 
                        residual_center_vector[15 * (D / M) + sub_q_col] - 
                        sub_quantizer_15[k * (D / M) + sub_q_col];
                }

                distance_LUT_PQ16_t dist_row;
                // square distance
                dist_row.dist_0 = 
                    (dist_0[0] * dist_0[0]) + (dist_0[1] * dist_0[1]) +
                    (dist_0[2] * dist_0[2]) + (dist_0[3] * dist_0[3]) +
                    (dist_0[4] * dist_0[4]) + (dist_0[5] * dist_0[5]) + 
                    (dist_0[6] * dist_0[6]) + (dist_0[7] * dist_0[7]);
                dist_row.dist_1 = 
                    (dist_1[0] * dist_1[0]) + (dist_1[1] * dist_1[1]) +
                    (dist_1[2] * dist_1[2]) + (dist_1[3] * dist_1[3]) +
                    (dist_1[4] * dist_1[4]) + (dist_1[5] * dist_1[5]) + 
                    (dist_1[6] * dist_1[6]) + (dist_1[7] * dist_1[7]);
                dist_row.dist_2 = 
                    (dist_2[0] * dist_2[0]) + (dist_2[1] * dist_2[1]) +
                    (dist_2[2] * dist_2[2]) + (dist_2[3] * dist_2[3]) +
                    (dist_2[4] * dist_2[4]) + (dist_2[5] * dist_2[5]) + 
                    (dist_2[6] * dist_2[6]) + (dist_2[7] * dist_2[7]);
                dist_row.dist_3 = 
                    (dist_3[0] * dist_3[0]) + (dist_3[1] * dist_3[1]) +
                    (dist_3[2] * dist_3[2]) + (dist_3[3] * dist_3[3]) +
                    (dist_3[4] * dist_3[4]) + (dist_3[5] * dist_3[5]) + 
                    (dist_3[6] * dist_3[6]) + (dist_3[7] * dist_3[7]);
                dist_row.dist_4 = 
                    (dist_4[0] * dist_4[0]) + (dist_4[1] * dist_4[1]) +
                    (dist_4[2] * dist_4[2]) + (dist_4[3] * dist_4[3]) +
                    (dist_4[4] * dist_4[4]) + (dist_4[5] * dist_4[5]) + 
                    (dist_4[6] * dist_4[6]) + (dist_4[7] * dist_4[7]);
                dist_row.dist_5 = 
                    (dist_5[0] * dist_5[0]) + (dist_5[1] * dist_5[1]) +
                    (dist_5[2] * dist_5[2]) + (dist_5[3] * dist_5[3]) +
                    (dist_5[4] * dist_5[4]) + (dist_5[5] * dist_5[5]) + 
                    (dist_5[6] * dist_5[6]) + (dist_5[7] * dist_5[7]);
                dist_row.dist_6 = 
                    (dist_6[0] * dist_6[0]) + (dist_6[1] * dist_6[1]) +
                    (dist_6[2] * dist_6[2]) + (dist_6[3] * dist_6[3]) +
                    (dist_6[4] * dist_6[4]) + (dist_6[5] * dist_6[5]) + 
                    (dist_6[6] * dist_6[6]) + (dist_6[7] * dist_6[7]);
                dist_row.dist_7 = 
                    (dist_7[0] * dist_7[0]) + (dist_7[1] * dist_7[1]) +
                    (dist_7[2] * dist_7[2]) + (dist_7[3] * dist_7[3]) +
                    (dist_7[4] * dist_7[4]) + (dist_7[5] * dist_7[5]) + 
                    (dist_7[6] * dist_7[6]) + (dist_7[7] * dist_7[7]);
                dist_row.dist_8 = 
                    (dist_8[0] * dist_8[0]) + (dist_8[1] * dist_8[1]) +
                    (dist_8[2] * dist_8[2]) + (dist_8[3] * dist_8[3]) +
                    (dist_8[4] * dist_8[4]) + (dist_8[5] * dist_8[5]) + 
                    (dist_8[6] * dist_8[6]) + (dist_8[7] * dist_8[7]);
                dist_row.dist_9 = 
                    (dist_9[0] * dist_9[0]) + (dist_9[1] * dist_9[1]) +
                    (dist_9[2] * dist_9[2]) + (dist_9[3] * dist_9[3]) +
                    (dist_9[4] * dist_9[4]) + (dist_9[5] * dist_9[5]) + 
                    (dist_9[6] * dist_9[6]) + (dist_9[7] * dist_9[7]);
                dist_row.dist_10 = 
                    (dist_10[0] * dist_10[0]) + (dist_10[1] * dist_10[1]) +
                    (dist_10[2] * dist_10[2]) + (dist_10[3] * dist_10[3]) +
                    (dist_10[4] * dist_10[4]) + (dist_10[5] * dist_10[5]) + 
                    (dist_10[6] * dist_10[6]) + (dist_10[7] * dist_10[7]);
                dist_row.dist_11 = 
                    (dist_11[0] * dist_11[0]) + (dist_11[1] * dist_11[1]) +
                    (dist_11[2] * dist_11[2]) + (dist_11[3] * dist_11[3]) +
                    (dist_11[4] * dist_11[4]) + (dist_11[5] * dist_11[5]) + 
                    (dist_11[6] * dist_11[6]) + (dist_11[7] * dist_11[7]);
                dist_row.dist_12 = 
                    (dist_12[0] * dist_12[0]) + (dist_12[1] * dist_12[1]) +
                    (dist_12[2] * dist_12[2]) + (dist_12[3] * dist_12[3]) +
                    (dist_12[4] * dist_12[4]) + (dist_12[5] * dist_12[5]) + 
                    (dist_12[6] * dist_12[6]) + (dist_12[7] * dist_12[7]);
                dist_row.dist_13 = 
                    (dist_13[0] * dist_13[0]) + (dist_13[1] * dist_13[1]) +
                    (dist_13[2] * dist_13[2]) + (dist_13[3] * dist_13[3]) +
                    (dist_13[4] * dist_13[4]) + (dist_13[5] * dist_13[5]) + 
                    (dist_13[6] * dist_13[6]) + (dist_13[7] * dist_13[7]);
                dist_row.dist_14 = 
                    (dist_14[0] * dist_14[0]) + (dist_14[1] * dist_14[1]) +
                    (dist_14[2] * dist_14[2]) + (dist_14[3] * dist_14[3]) +
                    (dist_14[4] * dist_14[4]) + (dist_14[5] * dist_14[5]) + 
                    (dist_14[6] * dist_14[6]) + (dist_14[7] * dist_14[7]);
                dist_row.dist_15 = 
                    (dist_15[0] * dist_15[0]) + (dist_15[1] * dist_15[1]) +
                    (dist_15[2] * dist_15[2]) + (dist_15[3] * dist_15[3]) +
                    (dist_15[4] * dist_15[4]) + (dist_15[5] * dist_15[5]) + 
                    (dist_15[6] * dist_15[6]) + (dist_15[7] * dist_15[7]);
                
                s_partial_result_table_construction_PE.write(dist_row);
            }
        }
    }
}

template<const int query_num>
void lookup_table_construction_forward_tail(
    hls::stream<distance_LUT_PQ16_t> &s_partial_result_table_construction_PE,
    hls::stream<distance_LUT_PQ16_t> &s_partial_result_table_construction_forward_in,
    hls::stream<distance_LUT_PQ16_t> &s_result_all_distance_lookup_table) {

    //////////  NOTE: the order of output LUT must be consistent of the center vector input  ///////// 
    // e.g., say nprobe=17, PE_num=4, then the first 3 PEs compute 5 tables while the last compute 2
    //  first 2 rounds 4 PEs, last 3 rounds 3 PEs
    // PE 0: 0, 4, 8, 11, 14
    // PE 1: 1, 5, 9, 12, 15
    // PE 2: 2, 6, 10, 13, 16
    // PE 3: 3, 7

    for (int query_id = 0; query_id < query_num; query_id++) {
        
        for (int interleave_iter = 0; interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE_LARGER; interleave_iter++) {

            // forward head / midlle PEs
            for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION_LARGER; s++) {
                // each lookup table: K rows
                for (int t = 0; t < K; t++) {
#pragma HLS pipeline II=1
                    s_result_all_distance_lookup_table.write(s_partial_result_table_construction_forward_in.read());
                }
            }
            if (interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE_SMALLER) {
                // result from the current PE
                for (int t = 0; t < K; t++) {
#pragma HLS pipeline II=1
                    s_result_all_distance_lookup_table.write(s_partial_result_table_construction_PE.read());
                }
            }
        }
    }
}

template<const int query_num, const int nprobe_per_PE_tail>
void lookup_table_construction_tail_PE(
    hls::stream<float>& s_PQ_quantizer_init_in,
    hls::stream<float>& s_center_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_in,
    hls::stream<distance_LUT_PQ16_t> &s_partial_result_table_construction_forward_in,
    hls::stream<distance_LUT_PQ16_t> &s_result_all_distance_lookup_table) {

#pragma HLS dataflow

    hls::stream<distance_LUT_PQ16_t> s_partial_result_table_construction_PE;
#pragma HLS stream variable=s_partial_result_table_construction_PE depth=512

    lookup_table_construction_compute_tail<query_num, nprobe_per_PE_tail>(
        s_PQ_quantizer_init_in,
        s_center_vectors_table_construction_PE_in,
        s_query_vectors_table_construction_PE_in,
        s_partial_result_table_construction_PE);

    lookup_table_construction_forward_tail<query_num>(
        s_partial_result_table_construction_PE,
        s_partial_result_table_construction_forward_in,
        s_result_all_distance_lookup_table);
}



template<const int query_num>
void lookup_table_construction_wrapper(
    hls::stream<float> &s_PQ_quantizer_init,
    hls::stream<float> &s_center_vectors_lookup_PE,
    hls::stream<float> &s_query_vectors_lookup_PE,
    hls::stream<distance_LUT_PQ16_t> &s_distance_LUT) {

#pragma HLS inline


    hls::stream<float> s_center_vectors_table_construction_PE[PE_NUM_TABLE_CONSTRUCTION];
#pragma HLS stream variable=s_center_vectors_table_construction_PE depth=512
// #pragma HLS resource variable=s_center_vectors_table_construction_PE core=FIFO_BRAM
#pragma HLS array_partition variable=s_center_vectors_table_construction_PE complete

    center_vectors_dispatcher<QUERY_NUM>(
        s_center_vectors_lookup_PE, 
        s_center_vectors_table_construction_PE);

    hls::stream<float> s_PQ_quantizer_init_forward[PE_NUM_TABLE_CONSTRUCTION_LARGER];
#pragma HLS stream variable=s_PQ_quantizer_init_forward depth=8
#pragma HLS array_partition variable=s_PQ_quantizer_init_forward complete

    hls::stream<float> s_query_vectors_table_construction_PE_forward[PE_NUM_TABLE_CONSTRUCTION_LARGER];
#pragma HLS stream variable=s_query_vectors_table_construction_PE_forward depth=8
#pragma HLS array_partition variable=s_query_vectors_table_construction_PE_forward complete

    hls::stream<distance_LUT_PQ16_t> s_partial_result_table_construction_PE_forward[PE_NUM_TABLE_CONSTRUCTION_LARGER];
#pragma HLS stream variable=s_partial_result_table_construction_PE_forward depth=8
#pragma HLS array_partition variable=s_partial_result_table_construction_PE_forward complete

    lookup_table_construction_head_PE<query_num, NPROBE_PER_TABLE_CONSTRUCTION_PE_LARGER>(
        s_PQ_quantizer_init,
        s_PQ_quantizer_init_forward[0],
        s_center_vectors_table_construction_PE[0],
        s_query_vectors_lookup_PE,
        s_query_vectors_table_construction_PE_forward[0],
        s_partial_result_table_construction_PE_forward[0]);

    // systolic array ID: e.g., 5 PEs, head = 0, middle = 1, 2, 3, tail = 4
    for (int s = 1; s < PE_NUM_TABLE_CONSTRUCTION_LARGER; s++) {
#pragma HLS UNROLL

        lookup_table_construction_middle_PE<query_num, NPROBE_PER_TABLE_CONSTRUCTION_PE_LARGER>(
            s,
            s_PQ_quantizer_init_forward[s - 1],
            s_PQ_quantizer_init_forward[s],
            s_center_vectors_table_construction_PE[s],
            s_query_vectors_table_construction_PE_forward[s - 1],
            s_query_vectors_table_construction_PE_forward[s],
            s_partial_result_table_construction_PE_forward[s - 1],
            s_partial_result_table_construction_PE_forward[s]);
    }

    // NOTE! PE_NUM_TABLE_CONSTRUCTION_SMALLER must === 1
    lookup_table_construction_tail_PE<query_num, NPROBE_PER_TABLE_CONSTRUCTION_PE_SMALLER>(
        s_PQ_quantizer_init_forward[PE_NUM_TABLE_CONSTRUCTION_LARGER - 1],
        s_center_vectors_table_construction_PE[PE_NUM_TABLE_CONSTRUCTION_LARGER],
        s_query_vectors_table_construction_PE_forward[PE_NUM_TABLE_CONSTRUCTION_LARGER - 1],
        s_partial_result_table_construction_PE_forward[PE_NUM_TABLE_CONSTRUCTION_LARGER - 1],
        s_distance_LUT);
}
