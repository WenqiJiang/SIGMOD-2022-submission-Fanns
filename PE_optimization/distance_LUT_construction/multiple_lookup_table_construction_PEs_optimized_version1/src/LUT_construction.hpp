#pragma once

#include "constants.hpp"
#include "types.hpp"


template<const int pe_num_table_construction>
void replicate_product_quantizer(
    hls::stream<float> &s_product_quantizer_init,
    hls::stream<float> (&s_product_quantizer_init_replicated)[pe_num_table_construction]);

template<const int query_num, const int pe_num_table_construction>
void replicate_query_vectors(
    hls::stream<float>& s_query_vectors,
    hls::stream<float> (&s_query_vectors_replicated)[pe_num_table_construction]);

template<const int query_num, const int pe_num_table_construction>
void replicate_center_vectors(
    hls::stream<float>& s_center_vectors_lookup_PE,
    hls::stream<float> (&s_center_vectors_replicated)[pe_num_table_construction]);

template<const int array_len>
float square_sum(float array[array_len]);

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_PE(
    hls::stream<float>& s_product_quantizer_init,
    hls::stream<float>& s_center_vectors_table_construction_PE,
    hls::stream<float>& s_query_vectors_table_construction_PE,
    hls::stream<distance_LUT_PQ16_t>& s_result_table_construction_PE);

template<const int query_num, const int pe_num_table_construction, const int nprobe_per_PE>
void gather_lookup_table(
    hls::stream<distance_LUT_PQ16_t> (&s_partial_distance_LUT)[pe_num_table_construction],
    hls::stream<distance_LUT_PQ16_t> &s_distance_LUT);

template<const int query_num, const int pe_num_table_construction, const int nprobe_per_PE>
void lookup_table_construction_wrapper(
    hls::stream<float> &s_product_quantizer_init,
    hls::stream<float> &s_center_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<distance_LUT_PQ16_t> &s_distance_LUT);




template<const int pe_num_table_construction>
void replicate_product_quantizer(
    hls::stream<float> &s_product_quantizer_init,
    hls::stream<float> (&s_product_quantizer_init_replicated)[pe_num_table_construction]) {

    // load PQ quantizer centers from HBM
    for (int i = 0; i < K * D; i++) {
#pragma HLS pipeline II=1

        float reg = s_product_quantizer_init.read();
        for (int s = 0; s < pe_num_table_construction; s++) {
#pragma HLS UNROLL
            s_product_quantizer_init_replicated[s].write(reg);
        }
    }
}


template<const int query_num, const int pe_num_table_construction>
void replicate_query_vectors(
    hls::stream<float>& s_query_vectors,
    hls::stream<float> (&s_query_vectors_replicated)[pe_num_table_construction]) {

    // Given an input stream of query vectors, broadcast it to all 
    //   distance table construction PEs

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors.read(); 
            for (int s = 0; s < pe_num_table_construction; s++) {
#pragma HLS UNROLL
                s_query_vectors_replicated[s].write(reg);
            }
        }
    }
}

template<const int query_num, const int pe_num_table_construction>
void replicate_center_vectors(
    hls::stream<float>& s_center_vectors_lookup_PE,
    hls::stream<float> (&s_center_vectors_replicated)[pe_num_table_construction]) {

    // Given an input stream of center vectors, interleave it to all 
    //   distance table construction PEs in a round-robin manner 
    //   e.g., 4 PEs, vector 0,4,8 -> PE0, 1,5,9 -> PE1, etc.
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int interleave_iter = 0; interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE; interleave_iter++) {

            for (int s = 0; s < pe_num_table_construction; s++) {

                for (int n = 0; n < D; n++) {
#pragma HLS pipeline II=1
                    s_center_vectors_replicated[s].write(s_center_vectors_lookup_PE.read());
                }
            }
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

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_PE(
    hls::stream<float>& s_product_quantizer_init,
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
    float center_vector_local[M][D / M];

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

            // load center vector
            residual_center_vectors:
            for (int m = 0; m < M; m++) {
                for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
                    center_vector_local[m][j] = s_center_vectors_table_construction_PE.read();
                    residual_center_vector[m][j] = query_vector_local[m][j] - center_vector_local[m][j];
                }
            }

            // construct distance lookup table
            single_row_lookup_table_construction:
            for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=1

                // the L1 diff between residual_center_vector and sub-quantizers
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
    }
}


template<const int query_num, const int pe_num_table_construction, const int nprobe_per_PE>
void gather_lookup_table(
    hls::stream<distance_LUT_PQ16_t> (&s_partial_distance_LUT)[pe_num_table_construction],
    hls::stream<distance_LUT_PQ16_t> &s_distance_LUT) {

    // Gather in a round-robin manner, e.g., for the case of 4 table construction PEs
    // PE0: 0, 4, 8 ...
    // PE1: 1, 5, 9 ...
    // PE2: 2, 6, 10 ...
    // PE3: 3, 7, 11 ...
    for (int query_id = 0; query_id < query_num; query_id++) {
        
        for (int interleave_iter = 0; interleave_iter < nprobe_per_PE; interleave_iter++) {

            for (int s = 0; s < pe_num_table_construction; s++) {
                // each lookup table: K rows
                for (int t = 0; t < K; t++) {
#pragma HLS pipeline II=1
                    s_distance_LUT.write(s_partial_distance_LUT[s].read());
                }
            }
        }
    }
}


template<const int query_num, const int pe_num_table_construction, const int nprobe_per_PE>
void lookup_table_construction_wrapper(
    hls::stream<float> &s_product_quantizer_init,
    hls::stream<float> &s_center_vectors,
    hls::stream<float> &s_query_vectors,
    hls::stream<distance_LUT_PQ16_t> &s_distance_LUT) {
#pragma HLS dataflow 


    hls::stream<float> s_product_quantizer_init_replicated[pe_num_table_construction]; 
#pragma HLS stream variable=s_product_quantizer_init_replicated depth=4
// #pragma HLS resource variable=s_product_quantizer_init_replicated core=FIFO_SRL
#pragma HLS array_partition variable=s_product_quantizer_init_replicated complete

    replicate_product_quantizer<pe_num_table_construction>(
        s_product_quantizer_init,
        s_product_quantizer_init_replicated);

    hls::stream<float> s_query_vectors_replicated[pe_num_table_construction];
#pragma HLS stream variable=s_query_vectors_replicated depth=512
// #pragma HLS resource variable=s_query_vectors_replicated core=FIFO_BRAM
#pragma HLS array_partition variable=s_query_vectors_replicated complete

    replicate_query_vectors<query_num, pe_num_table_construction>(
        s_query_vectors,
        s_query_vectors_replicated);

    hls::stream<float> s_center_vectors_replicated[pe_num_table_construction];
#pragma HLS stream variable=s_center_vectors_replicated depth=512
// #pragma HLS resource variable=s_center_vectors_replicated core=FIFO_BRAM
#pragma HLS array_partition variable=s_center_vectors_replicated complete

    replicate_center_vectors<query_num, pe_num_table_construction>(
        s_center_vectors,
        s_center_vectors_replicated); 

    hls::stream<distance_LUT_PQ16_t> s_partial_distance_LUT[pe_num_table_construction];
#pragma HLS stream variable=s_partial_distance_LUT depth=512
// #pragma HLS resource variable=s_partial_distance_LUT core=FIFO_BRAM
#pragma HLS array_partition variable=s_partial_distance_LUT complete

    for (int s = 0; s < pe_num_table_construction; s++) {
#pragma HLS UNROLL
        lookup_table_construction_PE<query_num, nprobe_per_PE>(
            s_product_quantizer_init_replicated[s], 
            s_center_vectors_replicated[s], 
            s_query_vectors_replicated[s], 
            s_partial_distance_LUT[s]);
    }

    gather_lookup_table<query_num, pe_num_table_construction, nprobe_per_PE>(
        s_partial_distance_LUT, 
        s_distance_LUT);
}
