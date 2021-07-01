#pragma once 

#include "constants.hpp"
#include "types.hpp"

////////////////////     init      ////////////////////

template<const int query_num>
void load_query_vectors(
    const float* DRAM_query_vector,
    hls::stream<float> (&s_query_vectors_distance_computation_PE)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<float>& s_query_vectors_lookup_PE);

void load_center_vectors(
    const float* table_HBM1, 
    hls::stream<float> (&s_center_vectors_init_distance_computation_PE)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<float> &s_center_vectors_init_lookup_PE);

void load_PQ_quantizer(
    const float* DRAM_PQ_quantizer,
    hls::stream<float> (&s_PQ_quantizer_init)[PE_NUM_TABLE_CONSTRUCTION]);

////////////////////     Center Distance Computation     ////////////////////  

template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float> (&s_centroid_vectors)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<float> (&s_query_vectors)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<dist_cell_ID_t> (&s_partial_cell_distance)[PE_NUM_CENTER_DIST_COMP]);

template<const int query_num>
void compute_cell_distance(
    int start_cell_ID,
    hls::stream<float>& s_centroid_vectors,
    hls::stream<float>& s_query_vectors,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance);

////////////////////     Select Scanned Cells     //////////////////// 

template<const int query_num>
void split_cell_ID(
    hls::stream<dist_cell_ID_t>& s_merge_output, 
    hls::stream<int>& s_searched_cell_id_lookup_PE, 
    hls::stream<int>& s_searched_cell_id_scan_controller);

////////////////////     Center Vector Lookup     ////////////////////   

template<const int query_num>
void lookup_center_vectors(
    hls::stream<float> &s_center_vectors_init_lookup_PE,
    hls::stream<int>& s_searched_cell_id_lookup_PE,
    hls::stream<float>& s_center_vectors_lookup_PE);

////////////////////     Distance Lookup Table Construction     ////////////////////   

template<const int query_num>
void query_vectors_dispatcher(
    hls::stream<float>& s_query_vectors,
    hls::stream<float> (&s_query_vectors_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION]);

template<const int query_num>
void center_vectors_dispatcher(
    hls::stream<float>& s_center_vectors_lookup_PE,
    hls::stream<float> (&s_center_vectors_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION]);

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_wrapper(
    hls::stream<float> (&s_PQ_quantizer_init)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<float> (&s_center_vectors_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<float> (&s_query_vectors_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<distance_LUT_PQ16_t> (&s_result_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION]);

template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_PE(
    hls::stream<float>& s_PQ_quantizer_init,
    hls::stream<float>& s_center_vectors_table_construction_PE,
    hls::stream<float>& s_query_vectors_table_construction_PE,
    hls::stream<distance_LUT_PQ16_t>& s_result_table_construction_PE);

template<const int query_num, const int nprobe_per_PE>
void gather_lookup_table(
    hls::stream<distance_LUT_PQ16_t> (&s_result_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<distance_LUT_PQ16_t> &s_result_all_distance_lookup_table);


////////////////////     init      ////////////////////

template<const int query_num>
void load_query_vectors(
    const float* DRAM_query_vector,
    hls::stream<float> (&s_query_vectors_distance_computation_PE)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<float>& s_query_vectors_lookup_PE) {

    // Data type: suppose each vector = 128 D, FPGA freq = 200 MHz
    //   then it takes 640 + 200 ns < 1 us to load a query vector, 
    //   much faster than computing distance and constructing LUT (> 10 us)

    // query vector (1 vector per NPROBE=32 center vector)
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = DRAM_query_vector[query_id * D + d];
            s_query_vectors_lookup_PE.write(reg);
            for (int s = 0; s < PE_NUM_CENTER_DIST_COMP; s++) {
#pragma HLS UNROLL
                s_query_vectors_distance_computation_PE[s].write(reg);
            }
        }
    }
}

void load_center_vectors(
    const float* table_HBM1, 
    hls::stream<float> (&s_center_vectors_init_distance_computation_PE)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<float> &s_center_vectors_init_lookup_PE) {

    // e.g., CENTROIDS_PER_PARTITION = 256, CENTROID_PARTITIONS = 32
    //    first 256 elements -> stream 0
    //    second 256 elements -> stream 1, so on and so forth
    for (int s = 0; s < PE_NUM_CENTER_DIST_COMP; s++) {
        
        for (int i = 0; i < CENTROIDS_PER_PARTITION * D; i++) {
#pragma HLS pipeline II=1
            float reg = table_HBM1[s * CENTROIDS_PER_PARTITION * D + i];
            s_center_vectors_init_distance_computation_PE[s].write(reg);
            s_center_vectors_init_lookup_PE.write(reg);
        }
    }
}

void load_PQ_quantizer(
    const float* DRAM_PQ_quantizer,
    hls::stream<float> (&s_PQ_quantizer_init)[PE_NUM_TABLE_CONSTRUCTION]) {

    // load PQ quantizer centers from HBM
    for (int i = 0; i < K * D; i++) {
#pragma HLS pipeline II=1
        float reg = DRAM_PQ_quantizer[i];
        for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION; s++) {
#pragma HLS UNROLL
            s_PQ_quantizer_init[s].write(reg);
        }
    }
}

////////////////////     Center Distance Computation     ////////////////////  

template<const int query_num>
void compute_cell_distance_wrapper(
    hls::stream<float> (&s_centroid_vectors)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<float> (&s_query_vectors)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<dist_cell_ID_t> (&s_partial_cell_distance)[PE_NUM_CENTER_DIST_COMP]) {
#pragma HLS inline

    for (int s = 0; s < PE_NUM_CENTER_DIST_COMP; s++) {
#pragma HLS UNROLL
        compute_cell_distance<QUERY_NUM>(
            s * CENTROIDS_PER_PARTITION, s_centroid_vectors[s], 
            s_query_vectors[s], s_partial_cell_distance[s]);
    }
}

template<const int query_num>
void compute_cell_distance(
    int start_cell_ID,
    hls::stream<float>& s_centroid_vectors,
    hls::stream<float>& s_query_vectors,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance) {

    // what is the speed needed to compute these distances? need to match HBM
    // python performance_estimation_three_nodes_IVF.py --nlist=8192 --nprobe=32
    // Estimated HBM Latency: 23.58 us -> 3K cycles
    // each URAM -> 64bit port x 2 -> 4 elements per cycle -> 64 cycles per vec
    //   -> at most 50 vectors per URAM (actually 1 URAM can only fit 32)
    // My strategy: 32 Partitions, each responsible for 256 vectors, 
    //   each vector finish in 10 cycles
    float cell_centroids_partition[CENTROIDS_PER_PARTITION * D];
#pragma HLS array_partition variable=cell_centroids_partition cyclic factor=8 
#pragma HLS resource variable=cell_centroids_partition core=RAM_2P_URAM

    float local_query_buffer[D];
#pragma HLS array_partition variable=local_query_buffer cyclic factor=8

    // init before any computation
    for (int c = 0; c < CENTROIDS_PER_PARTITION; c++) {
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

        // compute distance and write results out
        for (int c = 0; c < CENTROIDS_PER_PARTITION; c++) {

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
            s_partial_cell_distance.write(out);
        }
    }
}



////////////////////     Select Scanned Cells     ////////////////////

template<const int query_num>
void split_cell_ID(
    hls::stream<dist_cell_ID_t>& s_merge_output, 
    hls::stream<int>& s_searched_cell_id_lookup_PE, 
    hls::stream<int>& s_searched_cell_id_scan_controller) {
// NOTE! Here, input_stream_len must be NPROBE === 32

    for (int query_id = 0; query_id < query_num; query_id++) {
        
        dist_cell_ID_t tmp;
        int searched_cell_id_local[NPROBE];
#pragma HLS array_partition variable=searched_cell_id_local dim=1

        for (int i = 0; i < NPROBE; i++) {
#pragma HLS pipeline II=1
            tmp = s_merge_output.read();
            searched_cell_id_local[i] = tmp.cell_ID;
            s_searched_cell_id_lookup_PE.write(searched_cell_id_local[i]);
            s_searched_cell_id_scan_controller.write(searched_cell_id_local[i]);
        }
    }
}

////////////////////     Center Vector Lookup     ////////////////////   

template<const int query_num>
void lookup_center_vectors(
    hls::stream<float> &s_center_vectors_init_lookup_PE,
    hls::stream<int>& s_searched_cell_id_lookup_PE,
    hls::stream<float>& s_center_vectors_lookup_PE) {

    float center_vector_local[NLIST * D];
#pragma HLS resource variable=center_vector_local core=RAM_2P_URAM

    // init: load center vectors from DRAM 
    for (int i = 0; i < NLIST * D; i++) {
#pragma HLS pipeline II=1
        center_vector_local[i] = s_center_vectors_init_lookup_PE.read();
    }

    //  lookup center vectors given ID
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < NPROBE; nprobe_id++) {

            int vec_id = s_searched_cell_id_lookup_PE.read();
            int start_addr = vec_id * D;

            for (int i = 0; i < D; i++) {
#pragma HLS pipeline II=1
                s_center_vectors_lookup_PE.write(center_vector_local[start_addr + i]);
            }
        }
    }
}

////////////////////     Distance Lookup Table Construction     ////////////////////   

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

        for (int interleave_iter = 0; interleave_iter < NPROBE_PER_TABLE_CONSTRUCTION_PE; interleave_iter++) {

            for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION; s++) {

                for (int n = 0; n < D; n++) {
    #pragma HLS pipeline II=1
                    s_center_vectors_table_construction_PE[s].write(s_center_vectors_lookup_PE.read());
                }
            }
        }
    }
}


template<const int query_num, const int nprobe_per_PE>
void lookup_table_construction_wrapper(
    hls::stream<float> (&s_PQ_quantizer_init)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<float> (&s_center_vectors_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<float> (&s_query_vectors_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<distance_LUT_PQ16_t> (&s_result_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION]) {
#pragma HLS inline

    for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION; s++) {
#pragma HLS UNROLL
        lookup_table_construction_PE<query_num, nprobe_per_PE>(
            s_PQ_quantizer_init[s], s_center_vectors_table_construction_PE[s], 
            s_query_vectors_table_construction_PE[s], s_result_table_construction_PE[s]);
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
#pragma HLS pipeline
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
#pragma HLS UNROLL factor=2
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

template<const int query_num, const int nprobe_per_PE>
void gather_lookup_table(
    hls::stream<distance_LUT_PQ16_t> (&s_result_table_construction_PE)[PE_NUM_TABLE_CONSTRUCTION],
    hls::stream<distance_LUT_PQ16_t> &s_result_all_distance_lookup_table) {

    // Gather in a round-robin manner
    // PE0: 0, 4, 8 ...
    // PE1: 1, 5, 9 ...
    // PE2: 2, 6, 10 ...
    // PE3: 3, 7, 11 ...
    for (int query_id = 0; query_id < query_num; query_id++) {
        
        for (int interleave_iter = 0; interleave_iter < nprobe_per_PE; interleave_iter++) {

            for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION; s++) {
                // each lookup table: K rows
                for (int t = 0; t < K; t++) {
#pragma HLS pipeline II=1
                    s_result_all_distance_lookup_table.write(s_result_table_construction_PE[s].read());
                }
            }
        }
    }
}

