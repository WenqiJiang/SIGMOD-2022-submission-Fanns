#pragma once

#include "constants.hpp"
#include "types.hpp"

////////////////////     Function to call in the top-level     ////////////////////
template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_PE(
    hls::stream<float> &s_product_quantizer_init,
    hls::stream<float> &s_center_vectors_table_construction_PE,
    hls::stream<float> &s_query_vectors_table_construction_PE,
    hls::stream<distance_LUT_PQ16_t> &s_result_table_construction_PE);
////////////////////     Function to call in the top-level     ////////////////////

void partition_product_quantizer_init(
        hls::stream<float> &s_product_quantizer_init,
        hls::stream<float> (&s_partial_product_quantizer_init)[M]) {

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    // float product_quantizer[M][K][D / M];

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int m = 0; m < M; m++) {
        for (int k = 0; k < K; k++) {
            for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
                s_partial_product_quantizer_init[m].write(
                    s_product_quantizer_init.read());
            }
        }
    } 
}

template<const int query_num, const int nprobe_per_PE>
void partition_center_vectors_table_construction_PE(
    hls::stream<float> &s_center_vectors_table_construction_PE,
    hls::stream<float> (&s_partial_center_vectors)[M]) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {

            // Note! Here the dimension is hard-coded, 8 iteration reads an center vector
            for (int m = 0; m < M; m++) {

                for (int d = 0; d < D / M; d++) {
#pragma HLS pipeline II=1
                    s_partial_center_vectors[m].write(
                        s_center_vectors_table_construction_PE.read());
                }
            }
        }
    }
}

template<const int query_num>
void partition_query_vectors_table_construction_PE(
        hls::stream<float> &s_query_vectors_table_construction_PE,
        hls::stream<float> (&s_partial_query_vectors)[M]) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vector, cut it into M sub-vectors
        for (int m = 0; m < M; m++) {

            for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
                s_partial_query_vectors[m].write(
                    s_query_vectors_table_construction_PE.read());
            }
        }
    }
}

template<const int query_num, const int nprobe_per_PE>
void partial_lookup_table_construction(
    hls::stream<float> &s_partial_product_quantizer_init,
    hls::stream<float> &s_partial_center_vectors,
    hls::stream<float> &s_partial_query_vectors,
    hls::stream<float> &s_partial_result) {

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
    // Each PE stores 1 of the 16 sub-quantizer
    float sub_quantizer[K][D / M];
#pragma HLS array_partition variable=sub_quantizer dim=2
#pragma HLS resource variable=sub_quantizer core=RAM_1P_BRAM

    // DRAM PQ quantizer format: 16 (M) x 256 (K) x 8 (D/M)
    for (int k = 0; k < K; k++) {
        for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
            sub_quantizer[k][j] = s_partial_product_quantizer_init.read();
        }
    }

    for (int query_id = 0; query_id < query_num; query_id++) {

        // store query and center vector into the format of M sub-vectors
        float partial_query_vector_local[D / M];
#pragma HLS array_partition variable=partial_query_vector_local complete

        float partial_residual_center_vector[D / M]; // query_vector - center_vector
#pragma HLS array_partition variable=partial_residual_center_vector complete

        // load query vector
        for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
            partial_query_vector_local[j] = s_partial_query_vectors.read();
        }

        // for each nprobe, construct LUT
        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {
                
            for (int j = 0; j < D / M; j++) {
#pragma HLS pipeline II=1
                partial_residual_center_vector[j] = 
                    partial_query_vector_local[j] - s_partial_center_vectors.read();
            }

            for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=1

                float L2_dist[D/M];
#pragma HLS array_partition variable=L2_dist complete

                for (int l = 0; l < D / M; l++) {
#pragma HLS UNROLL
                    float reg = partial_residual_center_vector[l] - sub_quantizer[k][l];
                    L2_dist[l] = reg * reg;
                }

                float partial_distance = 
                    L2_dist[0] + L2_dist[1] + L2_dist[2] + L2_dist[3] + 
                    L2_dist[4] + L2_dist[5] + L2_dist[6] + L2_dist[7];
                
                s_partial_result.write(partial_distance);
            }
        }
    }
}

template<const int query_num, const int nprobe_per_PE>
void gather_distance_LUT(
    hls::stream<float> (&s_partial_result)[M], 
    hls::stream<distance_LUT_PQ16_t> &s_result_table_construction_PE) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe_per_PE; nprobe_id++) {

            for (int k = 0; k < K; k++) {
#pragma HLS pipeline II=1

                distance_LUT_PQ16_t dist_row;

                // square distance
                dist_row.dist_0 = s_partial_result[0].read();
                dist_row.dist_1 = s_partial_result[1].read();
                dist_row.dist_2 = s_partial_result[2].read();
                dist_row.dist_3 = s_partial_result[3].read();
                dist_row.dist_4 = s_partial_result[4].read();
                dist_row.dist_5 = s_partial_result[5].read();
                dist_row.dist_6 = s_partial_result[6].read();
                dist_row.dist_7 = s_partial_result[7].read();
                dist_row.dist_8 = s_partial_result[8].read();
                dist_row.dist_9 = s_partial_result[9].read();
                dist_row.dist_10 = s_partial_result[10].read();
                dist_row.dist_11 = s_partial_result[11].read();
                dist_row.dist_12 = s_partial_result[12].read();
                dist_row.dist_13 = s_partial_result[13].read();
                dist_row.dist_14 = s_partial_result[14].read();
                dist_row.dist_15 = s_partial_result[15].read();
                
                s_result_table_construction_PE.write(dist_row);
            }
        }
    }
}

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_PE(
    hls::stream<float> &s_product_quantizer_init,
    hls::stream<float> &s_center_vectors_table_construction_PE,
    hls::stream<float> &s_query_vectors_table_construction_PE,
    hls::stream<distance_LUT_PQ16_t> &s_result_table_construction_PE) {

    /* output format:
     *   lookup table dim: (K x M)
     *   sending first row, then second row, and so on...
     *   store in distance_LUT_PQ16_t, each represent an entire row (M=16)
     *   256 distance_LUT_PQ16_t is an entire lookup table
     */

#pragma HLS dataflow

    hls::stream<float> s_partial_product_quantizer_init[M];
#pragma HLS stream variable=s_partial_product_quantizer_init depth=2
#pragma HLS array_partition variable=s_partial_product_quantizer_init complete

    hls::stream<float> s_partial_center_vectors[M];
#pragma HLS stream variable=s_partial_center_vectors depth=2
#pragma HLS array_partition variable=s_partial_center_vectors complete

    hls::stream<float> s_partial_query_vectors[M];
#pragma HLS stream variable=s_partial_query_vectors depth=2
#pragma HLS array_partition variable=s_partial_query_vectors complete

    hls::stream<float> s_partial_result[M];
#pragma HLS stream variable=s_partial_result depth=2
#pragma HLS array_partition variable=s_partial_result complete

    // local alignment: 16-sub quantizers
    //    each quantizer: 256 row, (128 / 16) col
    // first and last dimension needs to be partitioned such that we can
    //    compute a row of product quantizer LUT in parallel

    partition_product_quantizer_init(
        s_product_quantizer_init,
        s_partial_product_quantizer_init);

    partition_center_vectors_table_construction_PE<query_num, nprobe_per_PE>(
        s_center_vectors_table_construction_PE,
        s_partial_center_vectors);

    partition_query_vectors_table_construction_PE<query_num>(
        s_query_vectors_table_construction_PE,
        s_partial_query_vectors);

    for (int s = 0; s < M; s++) {
#pragma HLS UNROLL
        partial_lookup_table_construction<query_num, nprobe_per_PE>(
            s_partial_product_quantizer_init[s],
            s_partial_center_vectors[s],
            s_partial_query_vectors[s],
            s_partial_result[s]);
    }

    gather_distance_LUT<query_num, nprobe_per_PE>(
        s_partial_result, 
        s_result_table_construction_PE);

}
