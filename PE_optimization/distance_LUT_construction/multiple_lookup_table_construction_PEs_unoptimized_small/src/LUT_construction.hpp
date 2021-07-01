#pragma once 

#include "constants.hpp"
#include "types.hpp"

////////////////////     Function to call in top-level     ////////////////////
template<const int query_num>
void lookup_table_construction_wrapper(
    hls::stream<float> (&s_PQ_quantizer_init)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<float> &s_center_vectors_lookup_PE,
    hls::stream<float> &s_query_vectors_lookup_PE,
    hls::stream<distance_LUT_PQ16_t> &s_distance_LUT);
    
////////////////////     Function to call in top-level     ////////////////////

template<const int query_num>
void query_vectors_dispatcher(
    hls::stream<float>& s_query_vectors,
    hls::stream<float> (&s_query_vectors_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION]) {

    // Given an input stream of query vectors, broadcast it to all 
    //   distance table construction PEs

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors.read(); 
            for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION; s++) {
#pragma HLS UNROLL
                s_query_vectors_table_construction_PE[s].write(reg);
            }
        }
    }
}

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
void lookup_table_construction_PE(
    hls::stream<float>& s_PQ_quantizer_init,
    hls::stream<float>& s_center_vectors_table_construction_PE,
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
        sub_quantizer_0[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_1[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_2[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_3[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_4[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_5[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_6[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_7[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_8[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_9[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_10[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_11[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_12[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_13[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_14[i] = s_PQ_quantizer_init.read();
    }
    for (int i = 0; i < K * D / M; i++) {
        sub_quantizer_15[i] = s_PQ_quantizer_init.read();
    }


    float query_vector_local[D];
    float center_vector_local[D];
    float residual_center_vector[D]; // query_vector - center_vector
#pragma HLS array_partition variable=residual_center_vector block factor=16

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vector
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            query_vector_local[d] = s_query_vectors_table_construction_PE.read();
        }

        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {

            // load center vector
            residual_center_vectors:
            for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
                center_vector_local[d] = s_center_vectors_table_construction_PE.read();
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
                
                s_result_table_construction_PE.write(dist_row);
            }
        }
    }
}

template<const int query_num>
void gather_lookup_table(
    hls::stream<distance_LUT_PQ16_t> (&s_result_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<distance_LUT_PQ16_t> &s_result_all_distance_lookup_table) {

    // Gather in a round-robin manner
    // PE0: 0, 4, 8 ...
    // PE1: 1, 5, 9 ...
    // PE2: 2, 6, 10 ...
    // PE3: 3, 7, 11 ...
    for (int query_id = 0; query_id < query_num; query_id++) {
        
        // all PEs, common part
        for (int interleave_iter = 0; interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE_SMALLER; interleave_iter++) {

            for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION; s++) {
                // each lookup table: K rows
                for (int t = 0; t < K; t++) {
#pragma HLS pipeline II=1
                    s_result_all_distance_lookup_table.write(s_result_table_construction_PE[s].read());
                }
            }
        }

        // the PEs that are in the last round
        for (int interleave_iter = 0; interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE_LARGER - NPROBE_PER_TABLE_CONSTRUCTION_PE_SMALLER; interleave_iter++) {

            for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION_LARGER; s++) {
                // each lookup table: K rows
                for (int t = 0; t < K; t++) {
#pragma HLS pipeline II=1
                    s_result_all_distance_lookup_table.write(s_result_table_construction_PE[s].read());
                }
            }
        }
    }
}

template<const int query_num>
void lookup_table_construction_wrapper(
    hls::stream<float> (&s_PQ_quantizer_init)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<float> &s_center_vectors_lookup_PE,
    hls::stream<float> &s_query_vectors_lookup_PE,
    hls::stream<distance_LUT_PQ16_t> &s_distance_LUT) {

#pragma HLS inline

    hls::stream<float> s_query_vectors_table_construction_PE[PE_NUM_TABLE_CONSTRUCTION];
#pragma HLS stream variable=s_query_vectors_table_construction_PE depth=512
// #pragma HLS resource variable=s_query_vectors_table_construction_PE core=FIFO_BRAM
#pragma HLS array_partition variable=s_query_vectors_table_construction_PE complete

    query_vectors_dispatcher<QUERY_NUM>(
        s_query_vectors_lookup_PE, 
        s_query_vectors_table_construction_PE);

    hls::stream<float> s_center_vectors_table_construction_PE[PE_NUM_TABLE_CONSTRUCTION];
#pragma HLS stream variable=s_center_vectors_table_construction_PE depth=512
// #pragma HLS resource variable=s_center_vectors_table_construction_PE core=FIFO_BRAM
#pragma HLS array_partition variable=s_center_vectors_table_construction_PE complete

    center_vectors_dispatcher<QUERY_NUM>(
        s_center_vectors_lookup_PE, 
        s_center_vectors_table_construction_PE);

    // PE0 write 256 rows to s_distance_LUT, then PE1 write 256 rows
    // thus need a deep FIFO to make sure each PE can cache enough results
    // 32 * 512 = 16384 bits, BRAM = 18K bits
    // The FIFO must be long enough (each PE -> output K=256 ap_uint<512> for each cell)
    hls::stream<distance_LUT_PQ16_t> s_result_table_construction_PE[PE_NUM_TABLE_CONSTRUCTION];
#pragma HLS stream variable=s_result_table_construction_PE depth=512
// #pragma HLS resource variable=s_result_table_construction_PE core=FIFO_BRAM
#pragma HLS array_partition variable=s_result_table_construction_PE complete

    for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION_LARGER; s++) {
#pragma HLS UNROLL
        lookup_table_construction_PE<query_num, NPROBE_PER_TABLE_CONSTRUCTION_PE_LARGER>(
            s_PQ_quantizer_init[s], 
            s_center_vectors_table_construction_PE[s], 
            s_query_vectors_table_construction_PE[s], 
            s_result_table_construction_PE[s]);
    }

    for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION_SMALLER; s++) {
#pragma HLS UNROLL
        lookup_table_construction_PE<query_num, NPROBE_PER_TABLE_CONSTRUCTION_PE_SMALLER>(
            s_PQ_quantizer_init[PE_NUM_TABLE_CONSTRUCTION_LARGER + s], 
            s_center_vectors_table_construction_PE[PE_NUM_TABLE_CONSTRUCTION_LARGER + s], 
            s_query_vectors_table_construction_PE[PE_NUM_TABLE_CONSTRUCTION_LARGER + s], 
            s_result_table_construction_PE[PE_NUM_TABLE_CONSTRUCTION_LARGER + s]);
    }

    gather_lookup_table<QUERY_NUM>(
        s_result_table_construction_PE, 
        s_distance_LUT); 
}
