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
void gather_float_to_distance_LUT_PQ16(
    hls::stream<float>& s_partial_result_table_construction_individual,
    hls::stream<distance_LUT_PQ16_t>& s_partial_result_table_construction_PE) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {

            distance_LUT_PQ16_t out;
            for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=16
                out.dist_0 = s_partial_result_table_construction_individual.read();
                out.dist_1 = s_partial_result_table_construction_individual.read();
                out.dist_2 = s_partial_result_table_construction_individual.read();
                out.dist_3 = s_partial_result_table_construction_individual.read();
                out.dist_4 = s_partial_result_table_construction_individual.read();
                out.dist_5 = s_partial_result_table_construction_individual.read();
                out.dist_6 = s_partial_result_table_construction_individual.read();
                out.dist_7 = s_partial_result_table_construction_individual.read();
                out.dist_8 = s_partial_result_table_construction_individual.read();
                out.dist_9 = s_partial_result_table_construction_individual.read();
                out.dist_10 = s_partial_result_table_construction_individual.read();
                out.dist_11 = s_partial_result_table_construction_individual.read();
                out.dist_12 = s_partial_result_table_construction_individual.read();
                out.dist_13 = s_partial_result_table_construction_individual.read();
                out.dist_14 = s_partial_result_table_construction_individual.read();
                out.dist_15 = s_partial_result_table_construction_individual.read();

                s_partial_result_table_construction_PE.write(out);
            }
        }
    }
}

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_compute_head(
    hls::stream<float>& s_PQ_quantizer_init_in,
    hls::stream<float>& s_PQ_quantizer_init_out,
    hls::stream<float>& s_center_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_out,
    hls::stream<float>& s_partial_result_table_construction_individual) {

    /* output format:
     *   lookup table dim: (K x M)
     *   sending first row, then second row, and so on...
     *   store in distance_LUT_PQ16_t, each represent an entire row (M=16)
     *   256 distance_LUT_PQ16_t is an entire lookup table
     */

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    // [M][K][D/M] -> [16][256][8]
    float sub_quantizer[M * K * (D / M)];
#pragma HLS resource variable=sub_quantizer core=RAM_2P_URAM
#pragma HLS array_partition variable=sub_quantizer cyclic factor=8 dim=1

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int i = 0; i < M * K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }

    float query_vector_local[D];
    float center_vector_local[D];
    float residual_center_vector[D]; // query_vector - center_vector
#pragma HLS array_partition variable=residual_center_vector cyclic factor=16

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

                for (int m = 0; m < M; m++) {
#pragma HLS pipeline II=1

                    // no need to init to 0, the following logic will overwrite them
                    float L1_dist[D / M];
#pragma HLS array_partition variable=L1_dist complete
                    for (int simd_i = 0; simd_i < D / M; simd_i++) {
#pragma HLS UNROLL
                        L1_dist[simd_i] = 
                            residual_center_vector[m * (D / M) + simd_i] - 
                            sub_quantizer[m * K * (D / M) + k * (D / M) + simd_i];
                    }
                    float LUT_val = 
                    (L1_dist[0] * L1_dist[0]) + (L1_dist[1] * L1_dist[1]) +
                    (L1_dist[2] * L1_dist[2]) + (L1_dist[3] * L1_dist[3]) +
                    (L1_dist[4] * L1_dist[4]) + (L1_dist[5] * L1_dist[5]) + 
                    (L1_dist[6] * L1_dist[6]) + (L1_dist[7] * L1_dist[7]);
                    
                    s_partial_result_table_construction_individual.write(LUT_val);
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

#pragma HLS dataflow

    hls::stream<float> s_partial_result_table_construction_individual;
#pragma HLS stream variable=s_partial_result_table_construction_individual depth=512

    lookup_table_construction_compute_head<query_num, nprobe_per_PE>(
        s_PQ_quantizer_init_in,
        s_PQ_quantizer_init_out,
        s_center_vectors_table_construction_PE_in,
        s_query_vectors_table_construction_PE_in,
        s_query_vectors_table_construction_PE_out,
        s_partial_result_table_construction_individual);

    gather_float_to_distance_LUT_PQ16<query_num, nprobe_per_PE>(
        s_partial_result_table_construction_individual,
        s_partial_result_table_construction_PE);

}

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_compute_midlle(
    hls::stream<float>& s_PQ_quantizer_init_in,
    hls::stream<float>& s_PQ_quantizer_init_out,
    hls::stream<float>& s_center_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_in,
    hls::stream<float>& s_query_vectors_table_construction_PE_out,
    hls::stream<float>& s_partial_result_table_construction_individual) {

    /* output format:
     *   lookup table dim: (K x M)
     *   sending first row, then second row, and so on...
     *   store in distance_LUT_PQ16_t, each represent an entire row (M=16)
     *   256 distance_LUT_PQ16_t is an entire lookup table
     */

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    // [M][K][D/M] -> [16][256][8]
    float sub_quantizer[M * K * (D / M)];
#pragma HLS resource variable=sub_quantizer core=RAM_2P_URAM
#pragma HLS array_partition variable=sub_quantizer cyclic factor=8 dim=1

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int i = 0; i < M * K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer[i] = reg;
        s_PQ_quantizer_init_out.write(reg);
    }


    float query_vector_local[D];
    float center_vector_local[D];
    float residual_center_vector[D]; // query_vector - center_vector
#pragma HLS array_partition variable=residual_center_vector cyclic factor=16

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

                for (int m = 0; m < M; m++) {
#pragma HLS pipeline II=1

                    // no need to init to 0, the following logic will overwrite them
                    float L1_dist[D / M];
#pragma HLS array_partition variable=L1_dist complete
                    for (int simd_i = 0; simd_i < D / M; simd_i++) {
#pragma HLS UNROLL
                        L1_dist[simd_i] = 
                            residual_center_vector[m * (D / M) + simd_i] - 
                            sub_quantizer[m * K * (D / M) + k * (D / M) + simd_i];
                    }
                    float LUT_val = 
                    (L1_dist[0] * L1_dist[0]) + (L1_dist[1] * L1_dist[1]) +
                    (L1_dist[2] * L1_dist[2]) + (L1_dist[3] * L1_dist[3]) +
                    (L1_dist[4] * L1_dist[4]) + (L1_dist[5] * L1_dist[5]) + 
                    (L1_dist[6] * L1_dist[6]) + (L1_dist[7] * L1_dist[7]);
                    
                    s_partial_result_table_construction_individual.write(LUT_val);
                }
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

    hls::stream<float> s_partial_result_table_construction_individual;
#pragma HLS stream variable=s_partial_result_table_construction_individual depth=512

    hls::stream<distance_LUT_PQ16_t> s_partial_result_table_construction_PE;
#pragma HLS stream variable=s_partial_result_table_construction_PE depth=512

    lookup_table_construction_compute_midlle<query_num, nprobe_per_PE>(
        s_PQ_quantizer_init_in,
        s_PQ_quantizer_init_out,
        s_center_vectors_table_construction_PE_in,
        s_query_vectors_table_construction_PE_in,
        s_query_vectors_table_construction_PE_out,
        s_partial_result_table_construction_individual);

    gather_float_to_distance_LUT_PQ16<query_num, nprobe_per_PE>(
        s_partial_result_table_construction_individual,
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
    hls::stream<float>& s_partial_result_table_construction_individual) {

    /* output format:
     *   lookup table dim: (K x M)
     *   sending first row, then second row, and so on...
     *   store in distance_LUT_PQ16_t, each represent an entire row (M=16)
     *   256 distance_LUT_PQ16_t is an entire lookup table
     */

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    // [M][K][D/M] -> [16][256][8]
    float sub_quantizer[M * K * (D / M)];
#pragma HLS resource variable=sub_quantizer core=RAM_2P_URAM
#pragma HLS array_partition variable=sub_quantizer cyclic factor=8 dim=1

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int i = 0; i < M * K * D / M; i++) {
        float reg = s_PQ_quantizer_init_in.read();
        sub_quantizer[i] = reg;
    }


    float query_vector_local[D];
    float center_vector_local[D];
    float residual_center_vector[D]; // query_vector - center_vector
#pragma HLS array_partition variable=residual_center_vector cyclic factor=16

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

                for (int m = 0; m < M; m++) {
#pragma HLS pipeline II=1

                    // no need to init to 0, the following logic will overwrite them
                    float L1_dist[D / M];
#pragma HLS array_partition variable=L1_dist complete
                    for (int simd_i = 0; simd_i < D / M; simd_i++) {
#pragma HLS UNROLL
                        L1_dist[simd_i] = 
                            residual_center_vector[m * (D / M) + simd_i] - 
                            sub_quantizer[m * K * (D / M) + k * (D / M) + simd_i];
                    }
                    float LUT_val = 
                    (L1_dist[0] * L1_dist[0]) + (L1_dist[1] * L1_dist[1]) +
                    (L1_dist[2] * L1_dist[2]) + (L1_dist[3] * L1_dist[3]) +
                    (L1_dist[4] * L1_dist[4]) + (L1_dist[5] * L1_dist[5]) + 
                    (L1_dist[6] * L1_dist[6]) + (L1_dist[7] * L1_dist[7]);
                    
                    s_partial_result_table_construction_individual.write(LUT_val);
                }
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

    hls::stream<float> s_partial_result_table_construction_individual;
#pragma HLS stream variable=s_partial_result_table_construction_individual depth=512

    hls::stream<distance_LUT_PQ16_t> s_partial_result_table_construction_PE;
#pragma HLS stream variable=s_partial_result_table_construction_PE depth=512

    lookup_table_construction_compute_tail<query_num, nprobe_per_PE_tail>(
        s_PQ_quantizer_init_in,
        s_center_vectors_table_construction_PE_in,
        s_query_vectors_table_construction_PE_in,
        s_partial_result_table_construction_individual);

    gather_float_to_distance_LUT_PQ16<query_num, nprobe_per_PE_tail>(
        s_partial_result_table_construction_individual,
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
#pragma HLS stream variable=s_partial_result_table_construction_PE_forward depth=16
#pragma HLS array_partition variable=s_partial_result_table_construction_PE_forward complete
#pragma HLS resource variable=s_partial_result_table_construction_PE_forward core=FIFO_SRL


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
