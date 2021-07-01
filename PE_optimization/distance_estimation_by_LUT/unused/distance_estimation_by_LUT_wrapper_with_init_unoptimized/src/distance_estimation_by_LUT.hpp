#include "constants.hpp"

template<const int query_num, const int nprobe>
void dummy_distance_LUT_sender(
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT);
 
void load_distance_LUT(
    float distance_LUT_original[8][512], 
    float distance_LUT[3 * HBM_CHANNEL_NUM][8][512]);

void PQ_lookup_computation(
    float distance_LUT[8][512],
    hls::stream<single_PQ>& s_single_PQ, 
    int scanned_entries_every_cell,
    int last_element_valid, 
    hls::stream<single_PQ_result>& s_single_PQ_result);

void dummy_PQ_result_sender(
    int scanned_entries_every_cell,
    hls::stream<single_PQ_result> &s_single_PQ_result);

void load_distance_LUT_original(
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT,
    float distance_LUT_original[8][512]);

void PQ_lookup_computation_wrapper(
    hls::stream<single_PQ> (&s_single_PQ)[3 * HBM_CHANNEL_NUM], 
    float distance_LUT_original[8][512],
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation, 
    hls::stream<int>& s_last_valid_channel, 
    hls::stream<single_PQ_result> (&s_single_PQ_result)[4][16]);

template<const int query_num, const int nprobe>
void distance_LUT_copy_and_PQ_computation_wrapper(
    hls::stream<single_PQ> (&s_single_PQ)[3 * HBM_CHANNEL_NUM], 
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT,
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation, 
    hls::stream<int>& s_last_valid_channel, 
    hls::stream<single_PQ_result> (&s_single_PQ_result)[4][16]);


template<const int query_num, const int nprobe>
void dummy_distance_LUT_sender(
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT) {

    distance_LUT_PQ16_t dist_row;

    dist_row.dist_0 = 0; 
    dist_row.dist_1 = 1; 
    dist_row.dist_2 = 2;
    dist_row.dist_3 = 3; 
    dist_row.dist_4 = 4; 
    dist_row.dist_5 = 5; 
    dist_row.dist_6 = 6; 
    dist_row.dist_7 = 7; 
    dist_row.dist_8 = 8; 
    dist_row.dist_9 = 9; 
    dist_row.dist_10 = 10; 
    dist_row.dist_11 = 11; 
    dist_row.dist_12 = 12; 
    dist_row.dist_13 = 13; 
    dist_row.dist_14 = 14; 
    dist_row.dist_15 = 15;

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            for (int row_id = 0; row_id < K; row_id++) {
#pragma HLS pipeline II=1
                s_distance_LUT.write(dist_row);
            }

        }
    }
    
}
 
void load_distance_LUT(
    float distance_LUT_original[8][512], 
    float distance_LUT[3 * HBM_CHANNEL_NUM][8][512]) {

    for (int row_id = 0; row_id < K; row_id++) {
#pragma HLS pipeline II=1

        // Using a tree structure to relieve the routing issue
        // 1 -> 2 -> 4 -> 8 -> 16 -> 32 -> 64
        volatile distance_LUT_PQ16_t dist_row_level_A; // = s_distance_LUT.read();
        dist_row_level_A.dist_0 = distance_LUT_original[0][row_id];
        dist_row_level_A.dist_1 = distance_LUT_original[1][row_id];
        dist_row_level_A.dist_2 = distance_LUT_original[2][row_id];
        dist_row_level_A.dist_3 = distance_LUT_original[3][row_id];
        dist_row_level_A.dist_4 = distance_LUT_original[4][row_id];
        dist_row_level_A.dist_5 = distance_LUT_original[5][row_id];
        dist_row_level_A.dist_6 = distance_LUT_original[6][row_id];
        dist_row_level_A.dist_7 = distance_LUT_original[7][row_id];

        dist_row_level_A.dist_8 = distance_LUT_original[0][row_id + 256];
        dist_row_level_A.dist_9 = distance_LUT_original[1][row_id + 256];
        dist_row_level_A.dist_10 = distance_LUT_original[2][row_id + 256];
        dist_row_level_A.dist_11 = distance_LUT_original[3][row_id + 256];
        dist_row_level_A.dist_12 = distance_LUT_original[4][row_id + 256];
        dist_row_level_A.dist_13 = distance_LUT_original[5][row_id + 256];
        dist_row_level_A.dist_14 = distance_LUT_original[6][row_id + 256];
        dist_row_level_A.dist_15 = distance_LUT_original[7][row_id + 256];
        
        // Note: for volatile variable, C++ doesn't provide default struct copy
        //  thus need to manually copy all the attributes
        volatile distance_LUT_PQ16_t dist_row_level_B[2];
#pragma HLS array_partition variable=dist_row_level_B complete
        for (int s = 0; s < 2; s++) {
#pragma HLS UNROLL
            dist_row_level_B[s].dist_0 = dist_row_level_A.dist_0;
            dist_row_level_B[s].dist_1 = dist_row_level_A.dist_1;
            dist_row_level_B[s].dist_2 = dist_row_level_A.dist_2;
            dist_row_level_B[s].dist_3 = dist_row_level_A.dist_3;
            dist_row_level_B[s].dist_4 = dist_row_level_A.dist_4;
            dist_row_level_B[s].dist_5 = dist_row_level_A.dist_5;
            dist_row_level_B[s].dist_6 = dist_row_level_A.dist_6;
            dist_row_level_B[s].dist_7 = dist_row_level_A.dist_7;
            dist_row_level_B[s].dist_8 = dist_row_level_A.dist_8;
            dist_row_level_B[s].dist_9 = dist_row_level_A.dist_9;
            dist_row_level_B[s].dist_10 = dist_row_level_A.dist_10;
            dist_row_level_B[s].dist_11 = dist_row_level_A.dist_11;
            dist_row_level_B[s].dist_12 = dist_row_level_A.dist_12;
            dist_row_level_B[s].dist_13 = dist_row_level_A.dist_13;
            dist_row_level_B[s].dist_14 = dist_row_level_A.dist_14;
            dist_row_level_B[s].dist_15 = dist_row_level_A.dist_15;
        }

        volatile distance_LUT_PQ16_t dist_row_level_C[4];
#pragma HLS array_partition variable=dist_row_level_C complete
        for (int s = 0; s < 4; s++) {
#pragma HLS UNROLL
            dist_row_level_C[s].dist_0 = dist_row_level_B[s / 2].dist_0;
            dist_row_level_C[s].dist_1 = dist_row_level_B[s / 2].dist_1;
            dist_row_level_C[s].dist_2 = dist_row_level_B[s / 2].dist_2;
            dist_row_level_C[s].dist_3 = dist_row_level_B[s / 2].dist_3;
            dist_row_level_C[s].dist_4 = dist_row_level_B[s / 2].dist_4;
            dist_row_level_C[s].dist_5 = dist_row_level_B[s / 2].dist_5;
            dist_row_level_C[s].dist_6 = dist_row_level_B[s / 2].dist_6;
            dist_row_level_C[s].dist_7 = dist_row_level_B[s / 2].dist_7;
            dist_row_level_C[s].dist_8 = dist_row_level_B[s / 2].dist_8;
            dist_row_level_C[s].dist_9 = dist_row_level_B[s / 2].dist_9;
            dist_row_level_C[s].dist_10 = dist_row_level_B[s / 2].dist_10;
            dist_row_level_C[s].dist_11 = dist_row_level_B[s / 2].dist_11;
            dist_row_level_C[s].dist_12 = dist_row_level_B[s / 2].dist_12;
            dist_row_level_C[s].dist_13 = dist_row_level_B[s / 2].dist_13;
            dist_row_level_C[s].dist_14 = dist_row_level_B[s / 2].dist_14;
            dist_row_level_C[s].dist_15 = dist_row_level_B[s / 2].dist_15;
        }

        volatile distance_LUT_PQ16_t dist_row_level_D[8];
#pragma HLS array_partition variable=dist_row_level_D complete
        for (int s = 0; s < 8; s++) {
#pragma HLS UNROLL
            dist_row_level_D[s].dist_0 = dist_row_level_C[s / 2].dist_0;
            dist_row_level_D[s].dist_1 = dist_row_level_C[s / 2].dist_1;
            dist_row_level_D[s].dist_2 = dist_row_level_C[s / 2].dist_2;
            dist_row_level_D[s].dist_3 = dist_row_level_C[s / 2].dist_3;
            dist_row_level_D[s].dist_4 = dist_row_level_C[s / 2].dist_4;
            dist_row_level_D[s].dist_5 = dist_row_level_C[s / 2].dist_5;
            dist_row_level_D[s].dist_6 = dist_row_level_C[s / 2].dist_6;
            dist_row_level_D[s].dist_7 = dist_row_level_C[s / 2].dist_7;
            dist_row_level_D[s].dist_8 = dist_row_level_C[s / 2].dist_8;
            dist_row_level_D[s].dist_9 = dist_row_level_C[s / 2].dist_9;
            dist_row_level_D[s].dist_10 = dist_row_level_C[s / 2].dist_10;
            dist_row_level_D[s].dist_11 = dist_row_level_C[s / 2].dist_11;
            dist_row_level_D[s].dist_12 = dist_row_level_C[s / 2].dist_12;
            dist_row_level_D[s].dist_13 = dist_row_level_C[s / 2].dist_13;
            dist_row_level_D[s].dist_14 = dist_row_level_C[s / 2].dist_14;
            dist_row_level_D[s].dist_15 = dist_row_level_C[s / 2].dist_15;
        }

        volatile distance_LUT_PQ16_t dist_row_level_E[16];
#pragma HLS array_partition variable=dist_row_level_E complete
        for (int s = 0; s < 16; s++) {
#pragma HLS UNROLL
            dist_row_level_E[s].dist_0 = dist_row_level_D[s / 2].dist_0;
            dist_row_level_E[s].dist_1 = dist_row_level_D[s / 2].dist_1;
            dist_row_level_E[s].dist_2 = dist_row_level_D[s / 2].dist_2;
            dist_row_level_E[s].dist_3 = dist_row_level_D[s / 2].dist_3;
            dist_row_level_E[s].dist_4 = dist_row_level_D[s / 2].dist_4;
            dist_row_level_E[s].dist_5 = dist_row_level_D[s / 2].dist_5;
            dist_row_level_E[s].dist_6 = dist_row_level_D[s / 2].dist_6;
            dist_row_level_E[s].dist_7 = dist_row_level_D[s / 2].dist_7;
            dist_row_level_E[s].dist_8 = dist_row_level_D[s / 2].dist_8;
            dist_row_level_E[s].dist_9 = dist_row_level_D[s / 2].dist_9;
            dist_row_level_E[s].dist_10 = dist_row_level_D[s / 2].dist_10;
            dist_row_level_E[s].dist_11 = dist_row_level_D[s / 2].dist_11;
            dist_row_level_E[s].dist_12 = dist_row_level_D[s / 2].dist_12;
            dist_row_level_E[s].dist_13 = dist_row_level_D[s / 2].dist_13;
            dist_row_level_E[s].dist_14 = dist_row_level_D[s / 2].dist_14;
            dist_row_level_E[s].dist_15 = dist_row_level_D[s / 2].dist_15;
        }

        volatile distance_LUT_PQ16_t dist_row_level_F[32];
#pragma HLS array_partition variable=dist_row_level_F complete
        for (int s = 0; s < 32; s++) {
#pragma HLS UNROLL
            dist_row_level_F[s].dist_0 = dist_row_level_E[s / 2].dist_0;
            dist_row_level_F[s].dist_1 = dist_row_level_E[s / 2].dist_1;
            dist_row_level_F[s].dist_2 = dist_row_level_E[s / 2].dist_2;
            dist_row_level_F[s].dist_3 = dist_row_level_E[s / 2].dist_3;
            dist_row_level_F[s].dist_4 = dist_row_level_E[s / 2].dist_4;
            dist_row_level_F[s].dist_5 = dist_row_level_E[s / 2].dist_5;
            dist_row_level_F[s].dist_6 = dist_row_level_E[s / 2].dist_6;
            dist_row_level_F[s].dist_7 = dist_row_level_E[s / 2].dist_7;
            dist_row_level_F[s].dist_8 = dist_row_level_E[s / 2].dist_8;
            dist_row_level_F[s].dist_9 = dist_row_level_E[s / 2].dist_9;
            dist_row_level_F[s].dist_10 = dist_row_level_E[s / 2].dist_10;
            dist_row_level_F[s].dist_11 = dist_row_level_E[s / 2].dist_11;
            dist_row_level_F[s].dist_12 = dist_row_level_E[s / 2].dist_12;
            dist_row_level_F[s].dist_13 = dist_row_level_E[s / 2].dist_13;
            dist_row_level_F[s].dist_14 = dist_row_level_E[s / 2].dist_14;
            dist_row_level_F[s].dist_15 = dist_row_level_E[s / 2].dist_15;
        }
        
        for (int s = 0; s < 3 * HBM_CHANNEL_NUM; s++) {
#pragma HLS UNROLL
            // col 0 ~ 7
            distance_LUT[s][0][row_id] = dist_row_level_F[s / 2].dist_0; 
            distance_LUT[s][1][row_id] = dist_row_level_F[s / 2].dist_1; 
            distance_LUT[s][2][row_id] = dist_row_level_F[s / 2].dist_2;
            distance_LUT[s][3][row_id] = dist_row_level_F[s / 2].dist_3; 
            distance_LUT[s][4][row_id] = dist_row_level_F[s / 2].dist_4; 
            distance_LUT[s][5][row_id] = dist_row_level_F[s / 2].dist_5; 
            distance_LUT[s][6][row_id] = dist_row_level_F[s / 2].dist_6; 
            distance_LUT[s][7][row_id] = dist_row_level_F[s / 2].dist_7; 

            // col 8 ~ 15
            distance_LUT[s][0][row_id + 256] = dist_row_level_F[s / 2].dist_8; 
            distance_LUT[s][1][row_id + 256] = dist_row_level_F[s / 2].dist_9; 
            distance_LUT[s][2][row_id + 256] = dist_row_level_F[s / 2].dist_10; 
            distance_LUT[s][3][row_id + 256] = dist_row_level_F[s / 2].dist_11; 
            distance_LUT[s][4][row_id + 256] = dist_row_level_F[s / 2].dist_12; 
            distance_LUT[s][5][row_id + 256] = dist_row_level_F[s / 2].dist_13; 
            distance_LUT[s][6][row_id + 256] = dist_row_level_F[s / 2].dist_14; 
            distance_LUT[s][7][row_id + 256] = dist_row_level_F[s / 2].dist_15;
        }
    }
}

void PQ_lookup_computation(
    float distance_LUT[8][512],
    hls::stream<single_PQ>& s_single_PQ, 
    int scanned_entries_every_cell,
    int last_element_valid, 
    hls::stream<single_PQ_result>& s_single_PQ_result) {

    for (int entry_id = 0; entry_id < scanned_entries_every_cell; entry_id++) {
#pragma HLS pipeline II=1

        single_PQ PQ_local = s_single_PQ.read();

        single_PQ_result out; 
        out.vec_ID = PQ_local.vec_ID;
        out.dist = 
            distance_LUT[0][PQ_local.PQ_code[0]] + 
            distance_LUT[1][PQ_local.PQ_code[1]] + 
            distance_LUT[2][PQ_local.PQ_code[2]] + 
            distance_LUT[3][PQ_local.PQ_code[3]] + 
            distance_LUT[4][PQ_local.PQ_code[4]] + 
            distance_LUT[5][PQ_local.PQ_code[5]] + 
            distance_LUT[6][PQ_local.PQ_code[6]] + 
            distance_LUT[7][PQ_local.PQ_code[7]] + 

            distance_LUT[0][((int)PQ_local.PQ_code[8]) + 256] + 
            distance_LUT[1][((int)PQ_local.PQ_code[9]) + 256] + 
            distance_LUT[2][((int)PQ_local.PQ_code[10]) + 256] + 
            distance_LUT[3][((int)PQ_local.PQ_code[11]) + 256] + 
            distance_LUT[4][((int)PQ_local.PQ_code[12]) + 256] + 
            distance_LUT[5][((int)PQ_local.PQ_code[13]) + 256] + 
            distance_LUT[6][((int)PQ_local.PQ_code[14]) + 256] + 
            distance_LUT[7][((int)PQ_local.PQ_code[15]) + 256];

        // for padded element, replace its distance by large number
        if ((entry_id == (scanned_entries_every_cell - 1)) && (last_element_valid == 0)) {
            out.vec_ID = -1;
            out.dist = LARGE_NUM;
        }
        s_single_PQ_result.write(out);
    }
}

void dummy_PQ_result_sender(
    int scanned_entries_every_cell,
    hls::stream<single_PQ_result> &s_single_PQ_result) {

    single_PQ_result out; 
    out.vec_ID = -1;
    out.dist = LARGE_NUM;
    
    for (int entry_id = 0; entry_id < scanned_entries_every_cell; entry_id++) {
#pragma HLS pipeline II=1
        s_single_PQ_result.write(out);
    }
}


void load_distance_LUT_original(
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT,
    float distance_LUT_original[8][512]) {

    for (int row_id = 0; row_id < K; row_id++) {
#pragma HLS pipeline II=1
        distance_LUT_PQ16_t dist_row = s_distance_LUT.read();
        
        distance_LUT_original[0][row_id] = dist_row.dist_0; 
        distance_LUT_original[1][row_id] = dist_row.dist_1; 
        distance_LUT_original[2][row_id] = dist_row.dist_2;
        distance_LUT_original[3][row_id] = dist_row.dist_3; 
        distance_LUT_original[4][row_id] = dist_row.dist_4; 
        distance_LUT_original[5][row_id] = dist_row.dist_5; 
        distance_LUT_original[6][row_id] = dist_row.dist_6; 
        distance_LUT_original[7][row_id] = dist_row.dist_7; 

        // col 8 ~ 15
        distance_LUT_original[0][row_id + 256] = dist_row.dist_8; 
        distance_LUT_original[1][row_id + 256] = dist_row.dist_9; 
        distance_LUT_original[2][row_id + 256] = dist_row.dist_10; 
        distance_LUT_original[3][row_id + 256] = dist_row.dist_11; 
        distance_LUT_original[4][row_id + 256] = dist_row.dist_12; 
        distance_LUT_original[5][row_id + 256] = dist_row.dist_13; 
        distance_LUT_original[6][row_id + 256] = dist_row.dist_14; 
        distance_LUT_original[7][row_id + 256] = dist_row.dist_15;
    }
}

void PQ_lookup_computation_wrapper(
    hls::stream<single_PQ> (&s_single_PQ)[3 * HBM_CHANNEL_NUM], 
    float distance_LUT_original[8][512],
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation, 
    hls::stream<int>& s_last_valid_channel, 
    hls::stream<single_PQ_result> (&s_single_PQ_result)[4][16]) {

// Wenqi: Here, can enable (improve performance) or disable dataflow (save resources)
#pragma HLS dataflow

    float distance_LUT[3 * HBM_CHANNEL_NUM][8][512];
#pragma HLS array_partition variable=distance_LUT dim=1
#pragma HLS array_partition variable=distance_LUT dim=2
#pragma HLS resource variable=distance_LUT core=RAM_2P_BRAM

    int last_element_valid[3 * HBM_CHANNEL_NUM];
#pragma HLS array_partition variable=last_element_valid complete

    int scanned_entries_every_cell_replicated[64];
#pragma HLS array_partition variable=scanned_entries_every_cell_replicated complete


    // per nprobe init
    int scanned_entries_every_cell = 
        s_scanned_entries_every_cell_PQ_lookup_computation.read();
    int last_valid_channel = s_last_valid_channel.read();
    
    for (int s = 0; s < 3 * HBM_CHANNEL_NUM; s++) {
#pragma HLS UNROLL 
        if (s > last_valid_channel) {
            last_element_valid[s] = 0;
        }
        else {
            last_element_valid[s] = 1;
        }
    }
    for (int s = 0; s < 64; s++) {
#pragma HLS UNROLL
        scanned_entries_every_cell_replicated[s] = scanned_entries_every_cell;
    }

    // load distance LUT
    load_distance_LUT(distance_LUT_original, distance_LUT);

    // Computer & Send results 
    for (int i = 0; i < 3; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < 16; j++) {
#pragma HLS UNROLL
            PQ_lookup_computation(
                distance_LUT[i * 16 + j],
                s_single_PQ[i * 16 + j], 
                scanned_entries_every_cell_replicated[i * 16 + j], 
                last_element_valid[i * 16 + j], 
                s_single_PQ_result[i][j]);
        }
    }

    for (int j = 0; j < 15; j++) {
#pragma HLS UNROLL
        PQ_lookup_computation(
            distance_LUT[48 + j],
            s_single_PQ[48+ j], 
            scanned_entries_every_cell_replicated[48 + j], 
            last_element_valid[48 + j], 
            s_single_PQ_result[3][j]);
    }

    dummy_PQ_result_sender(
        scanned_entries_every_cell_replicated[63], s_single_PQ_result[3][15]);
}


template<const int query_num, const int nprobe>
void distance_LUT_copy_and_PQ_computation_wrapper(
    hls::stream<single_PQ> (&s_single_PQ)[3 * HBM_CHANNEL_NUM], 
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT,
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation, 
    hls::stream<int>& s_last_valid_channel, 
    hls::stream<single_PQ_result> (&s_single_PQ_result)[4][16]
) {

    float distance_LUT_original[8][512];
#pragma HLS array_partition variable=distance_LUT_original dim=1
#pragma HLS resource variable=distance_LUT_original core=RAM_2P_BRAM

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {
#pragma HLS dataflow
// ERROR: [XFORM 203-711] Internal stream 's_single_PQ_result[0][2].V.vec_ID' failed dataflow checking: it must be produced and consumed exactly once.
            load_distance_LUT_original(
                s_distance_LUT, 
                distance_LUT_original);

            PQ_lookup_computation_wrapper(
                s_single_PQ, 
                distance_LUT_original,
                s_scanned_entries_every_cell_PQ_lookup_computation,
                s_last_valid_channel,
                s_single_PQ_result);
        }
    }
}
