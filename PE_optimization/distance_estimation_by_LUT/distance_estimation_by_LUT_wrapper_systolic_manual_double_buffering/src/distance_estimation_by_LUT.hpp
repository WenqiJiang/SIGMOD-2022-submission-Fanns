#include "constants.hpp"

template<const int query_num, const int nprobe>
void dummy_distance_LUT_sender(
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT);

template<const int query_num, const int nprobe>
void PQ_lookup_computation(
    // input streams
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT_in,
    hls::stream<single_PQ>& s_single_PQ,
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation,
    hls::stream<int>& s_last_element_valid_PQ_lookup_computation, 
    // output streams
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT_out,
    hls::stream<single_PQ_result>& s_single_PQ_result);

template<const int query_num, const int nprobe>
void dummy_PQ_result_sender(
    hls::stream<int>& s_scanned_entries_every_cell_Dummy,
    hls::stream<single_PQ_result> &s_single_PQ_result);

template<const int query_num, const int nprobe>
void replicate_s_scanned_entries_every_cell_PQ_lookup_computation(
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation,
    hls::stream<int> (&s_scanned_entries_every_cell_PQ_lookup_computation_replicated)[3 * HBM_CHANNEL_NUM]);

template<const int query_num, const int nprobe>
void send_s_last_element_valid_PQ_lookup_computation(
    hls::stream<int>& s_last_valid_channel,
    hls::stream<int> (&s_last_element_valid_PQ_lookup_computation)[3 * HBM_CHANNEL_NUM]);

template<const int query_num, const int nprobe>
void dummy_distance_LUT_consumer(
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT);

template<const int query_num, const int nprobe>
void PQ_lookup_computation_wrapper(
    hls::stream<single_PQ> (&s_single_PQ)[3 * HBM_CHANNEL_NUM], 
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT,
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation, 
    hls::stream<int>& s_scanned_entries_every_cell_Dummy,
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


template<const int query_num, const int nprobe>
void PQ_lookup_computation(
    // input streams
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT_in,
    hls::stream<single_PQ>& s_single_PQ,
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation,
    hls::stream<int>& s_last_element_valid_PQ_lookup_computation, 
    // output streams
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT_out,
    hls::stream<single_PQ_result>& s_single_PQ_result) {

    // Manual control on the double buffer, first 256 is buffer A, second 256 is buffer B
    //    2D array will fail to infer the dependency, II=2 even if pragma dependency 
    //    is turned off, thus use 16 1-D arrays
    float distance_LUT_0[512];
#pragma HLS array_partition variable=distance_LUT_0 block factor=2 dim=1
    float distance_LUT_1[512];
#pragma HLS array_partition variable=distance_LUT_1 block factor=2 dim=1
    float distance_LUT_2[512];
#pragma HLS array_partition variable=distance_LUT_2 block factor=2 dim=1
    float distance_LUT_3[512];
#pragma HLS array_partition variable=distance_LUT_3 block factor=2 dim=1
    float distance_LUT_4[512];
#pragma HLS array_partition variable=distance_LUT_4 block factor=2 dim=1
    float distance_LUT_5[512];
#pragma HLS array_partition variable=distance_LUT_5 block factor=2 dim=1
    float distance_LUT_6[512];
#pragma HLS array_partition variable=distance_LUT_6 block factor=2 dim=1
    float distance_LUT_7[512];
#pragma HLS array_partition variable=distance_LUT_7 block factor=2 dim=1
    float distance_LUT_8[512];
#pragma HLS array_partition variable=distance_LUT_8 block factor=2 dim=1
    float distance_LUT_9[512];
#pragma HLS array_partition variable=distance_LUT_9 block factor=2 dim=1
    float distance_LUT_10[512];
#pragma HLS array_partition variable=distance_LUT_10 block factor=2 dim=1
    float distance_LUT_11[512];
#pragma HLS array_partition variable=distance_LUT_11 block factor=2 dim=1
    float distance_LUT_12[512];
#pragma HLS array_partition variable=distance_LUT_12 block factor=2 dim=1
    float distance_LUT_13[512];
#pragma HLS array_partition variable=distance_LUT_13 block factor=2 dim=1
    float distance_LUT_14[512];
#pragma HLS array_partition variable=distance_LUT_14 block factor=2 dim=1
    float distance_LUT_15[512];
#pragma HLS array_partition variable=distance_LUT_15 block factor=2 dim=1

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe + 1; nprobe_id++) {

            int scanned_entries_every_cell = 
                s_scanned_entries_every_cell_PQ_lookup_computation.read();
            int last_element_valid = 
                s_last_element_valid_PQ_lookup_computation.read();

            int max_iter = scanned_entries_every_cell > K?
                scanned_entries_every_cell : K;

            int nprobe_mod = nprobe_id % 2;

            for (unsigned int common_iter = 0; common_iter < max_iter; common_iter++) {
#pragma HLS pipeline II=1

// if read A, then write B, no read A & write A situation

#pragma HLS dependence variable=distance_LUT_0 false
#pragma HLS dependence variable=distance_LUT_1 false
#pragma HLS dependence variable=distance_LUT_2 false
#pragma HLS dependence variable=distance_LUT_3 false
#pragma HLS dependence variable=distance_LUT_4 false
#pragma HLS dependence variable=distance_LUT_5 false
#pragma HLS dependence variable=distance_LUT_6 false
#pragma HLS dependence variable=distance_LUT_7 false
#pragma HLS dependence variable=distance_LUT_8 false
#pragma HLS dependence variable=distance_LUT_9 false
#pragma HLS dependence variable=distance_LUT_10 false
#pragma HLS dependence variable=distance_LUT_11 false
#pragma HLS dependence variable=distance_LUT_12 false
#pragma HLS dependence variable=distance_LUT_13 false
#pragma HLS dependence variable=distance_LUT_14 false
#pragma HLS dependence variable=distance_LUT_15 false

                // load part
                // last iter not read 
                if (nprobe_id < nprobe) { 

                    // load or not
                    if (common_iter < K) { 

                        distance_LUT_PQ16_t dist_row = s_distance_LUT_in.read();
                        s_distance_LUT_out.write(dist_row);

                        // even: load to buffer A, odd: load to buffer B
                        unsigned int idx;
                        if (nprobe_mod == 0) { 
                            idx = common_iter;
                        }
                        else { 
                            idx = 256 + common_iter;
                        }

                        distance_LUT_0[idx] = dist_row.dist_0; 
                        distance_LUT_1[idx] = dist_row.dist_1; 
                        distance_LUT_2[idx] = dist_row.dist_2;
                        distance_LUT_3[idx] = dist_row.dist_3; 
                        distance_LUT_4[idx] = dist_row.dist_4; 
                        distance_LUT_5[idx] = dist_row.dist_5; 
                        distance_LUT_6[idx] = dist_row.dist_6; 
                        distance_LUT_7[idx] = dist_row.dist_7; 
                        distance_LUT_8[idx] = dist_row.dist_8; 
                        distance_LUT_9[idx] = dist_row.dist_9; 
                        distance_LUT_10[idx] = dist_row.dist_10; 
                        distance_LUT_11[idx] = dist_row.dist_11; 
                        distance_LUT_12[idx] = dist_row.dist_12; 
                        distance_LUT_13[idx] = dist_row.dist_13; 
                        distance_LUT_14[idx] = dist_row.dist_14; 
                        distance_LUT_15[idx] = dist_row.dist_15; 
                    }
                }

                // compute part
                // first iter not compute
                if (nprobe_id > 0) { 

                    // compute or not
                    if (common_iter < scanned_entries_every_cell) { 

                        single_PQ PQ_local = s_single_PQ.read();

                        single_PQ_result out; 
                        out.vec_ID = PQ_local.vec_ID;                       

                        unsigned int idx_0;
                        unsigned int idx_1;
                        unsigned int idx_2;
                        unsigned int idx_3;
                        unsigned int idx_4;
                        unsigned int idx_5;
                        unsigned int idx_6;
                        unsigned int idx_7;
                        unsigned int idx_8;
                        unsigned int idx_9;
                        unsigned int idx_10;
                        unsigned int idx_11;
                        unsigned int idx_12;
                        unsigned int idx_13;
                        unsigned int idx_14;
                        unsigned int idx_15;

                        // if odd, compute using buffer A
                        // if even, compute using buffer B

                        if (nprobe_mod == 0)  {
                            idx_0 = ((unsigned int) 256) + PQ_local.PQ_code[0];
                            idx_1 = ((unsigned int) 256) + PQ_local.PQ_code[1];
                            idx_2 = ((unsigned int) 256) + PQ_local.PQ_code[2];
                            idx_3 = ((unsigned int) 256) + PQ_local.PQ_code[3];
                            idx_4 = ((unsigned int) 256) + PQ_local.PQ_code[4];
                            idx_5 = ((unsigned int) 256) + PQ_local.PQ_code[5];
                            idx_6 = ((unsigned int) 256) + PQ_local.PQ_code[6];
                            idx_7 = ((unsigned int) 256) + PQ_local.PQ_code[7];
                            idx_8 = ((unsigned int) 256) + PQ_local.PQ_code[8];
                            idx_9 = ((unsigned int) 256) + PQ_local.PQ_code[9];
                            idx_10 = ((unsigned int) 256) + PQ_local.PQ_code[10];
                            idx_11 = ((unsigned int) 256) + PQ_local.PQ_code[11];
                            idx_12 = ((unsigned int) 256) + PQ_local.PQ_code[12];
                            idx_13 = ((unsigned int) 256) + PQ_local.PQ_code[13];
                            idx_14 = ((unsigned int) 256) + PQ_local.PQ_code[14];
                            idx_15 = ((unsigned int) 256) + PQ_local.PQ_code[15];
                        }
                        else {
                            idx_0 = PQ_local.PQ_code[0];
                            idx_1 = PQ_local.PQ_code[1];
                            idx_2 = PQ_local.PQ_code[2];
                            idx_3 = PQ_local.PQ_code[3];
                            idx_4 = PQ_local.PQ_code[4];
                            idx_5 = PQ_local.PQ_code[5];
                            idx_6 = PQ_local.PQ_code[6];
                            idx_7 = PQ_local.PQ_code[7];
                            idx_8 = PQ_local.PQ_code[8];
                            idx_9 = PQ_local.PQ_code[9];
                            idx_10 = PQ_local.PQ_code[10];
                            idx_11 = PQ_local.PQ_code[11];
                            idx_12 = PQ_local.PQ_code[12];
                            idx_13 = PQ_local.PQ_code[13];
                            idx_14 = PQ_local.PQ_code[14];
                            idx_15 = PQ_local.PQ_code[15];
                        }
                        out.dist = 
                            distance_LUT_0[idx_0] + 
                            distance_LUT_1[idx_1] + 
                            distance_LUT_2[idx_2] + 
                            distance_LUT_3[idx_3] + 
                            distance_LUT_4[idx_4] + 
                            distance_LUT_5[idx_5] + 
                            distance_LUT_6[idx_6] + 
                            distance_LUT_7[idx_7] + 
                            distance_LUT_8[idx_8] + 
                            distance_LUT_9[idx_9] + 
                            distance_LUT_10[idx_10] + 
                            distance_LUT_11[idx_11] + 
                            distance_LUT_12[idx_12] + 
                            distance_LUT_13[idx_13] + 
                            distance_LUT_14[idx_14] + 
                            distance_LUT_15[idx_15];
                    
                        // for padded element, replace its distance by large number
                        if ((common_iter == (scanned_entries_every_cell - 1)) && (last_element_valid == 0)) {
                            out.vec_ID = -1;
                            out.dist = LARGE_NUM;
                        }
                        s_single_PQ_result.write(out);
                    }
                }
            }
        }
    }
}


template<const int query_num, const int nprobe>
void dummy_PQ_result_sender(
    hls::stream<int>& s_scanned_entries_every_cell_Dummy,
    hls::stream<single_PQ_result> &s_single_PQ_result) {

    single_PQ_result out; 
    out.vec_ID = -1;
    out.dist = LARGE_NUM;
    
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int scanned_entries_every_cell = 
                s_scanned_entries_every_cell_Dummy.read();

            for (int entry_id = 0; entry_id < scanned_entries_every_cell; entry_id++) {
#pragma HLS pipeline II=1
                s_single_PQ_result.write(out);
            }
        }
    }
}

template<const int query_num, const int nprobe>
void replicate_s_scanned_entries_every_cell_PQ_lookup_computation(
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation,
    hls::stream<int> (&s_scanned_entries_every_cell_PQ_lookup_computation_replicated)[3 * HBM_CHANNEL_NUM]) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int scanned_entries_every_cell_PQ_lookup_computation = 
                s_scanned_entries_every_cell_PQ_lookup_computation.read();

            for (int s = 0; s < 3 * HBM_CHANNEL_NUM; s++) {
#pragma HLS UNROLL
                s_scanned_entries_every_cell_PQ_lookup_computation_replicated[s].write(
                    scanned_entries_every_cell_PQ_lookup_computation);
            }
        }
    }
}


template<const int query_num, const int nprobe>
void send_s_last_element_valid_PQ_lookup_computation(
    hls::stream<int>& s_last_valid_channel,
    hls::stream<int> (&s_last_element_valid_PQ_lookup_computation)[3 * HBM_CHANNEL_NUM]) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int last_valid_channel = s_last_valid_channel.read(); 

            for (int s = 0; s < 3 * HBM_CHANNEL_NUM; s++) {
#pragma HLS UNROLL
                if (s <= last_valid_channel) {
                    s_last_element_valid_PQ_lookup_computation[s].write(1); // valid, not padding
                }
                else {
                    s_last_element_valid_PQ_lookup_computation[s].write(0); // invalid, padded element
                }
            }
        }
    }
}


template<const int query_num, const int nprobe>
void dummy_distance_LUT_consumer(
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT) {

    distance_LUT_PQ16_t dist_row;

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            for (int row_id = 0; row_id < K; row_id++) {
#pragma HLS pipeline II=1
                dist_row = s_distance_LUT.read();
            }

        }
    }
}

template<const int query_num, const int nprobe>
void PQ_lookup_computation_wrapper(
    hls::stream<single_PQ> (&s_single_PQ)[3 * HBM_CHANNEL_NUM], 
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT,
    hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation, 
    hls::stream<int>& s_scanned_entries_every_cell_Dummy,
    hls::stream<int>& s_last_valid_channel, 
    hls::stream<single_PQ_result> (&s_single_PQ_result)[4][16]) {
#pragma HLS dataflow

    hls::stream<distance_LUT_PQ16_t> s_distance_LUT_systolic[3 * HBM_CHANNEL_NUM];
#pragma HLS stream variable=s_distance_LUT_systolic depth=8
#pragma HLS array_partition variable=s_distance_LUT_systolic complete
// #pragma HLS RESOURCE variable=s_distance_LUT_systolic core=FIFO_SRL

    hls::stream<int> s_scanned_entries_every_cell_PQ_lookup_computation_replicated[3 * HBM_CHANNEL_NUM];
#pragma HLS stream variable=s_scanned_entries_every_cell_PQ_lookup_computation_replicated depth=8
#pragma HLS array_partition variable=s_scanned_entries_every_cell_PQ_lookup_computation_replicated complete
// #pragma HLS RESOURCE variable=s_scanned_entries_every_cell_PQ_lookup_computation_replicated core=FIFO_SRL

    replicate_s_scanned_entries_every_cell_PQ_lookup_computation<query_num, nprobe>(
        s_scanned_entries_every_cell_PQ_lookup_computation, 
        s_scanned_entries_every_cell_PQ_lookup_computation_replicated);

    hls::stream<int> s_last_element_valid_PQ_lookup_computation[3 * HBM_CHANNEL_NUM];
#pragma HLS stream variable=s_last_element_valid_PQ_lookup_computation depth=8
#pragma HLS array_partition variable=s_last_element_valid_PQ_lookup_computation complete
// #pragma HLS RESOURCE variable=s_last_element_valid_PQ_lookup_computation core=FIFO_SRL

    // Note, here interpret the last valid element, rather than simply replicate
    send_s_last_element_valid_PQ_lookup_computation<query_num, nprobe>(
        s_last_valid_channel, 
        s_last_element_valid_PQ_lookup_computation);

    // PE 0, get input from s_distance_LUT
    PQ_lookup_computation<query_num, nprobe>(
        // input streams
        s_distance_LUT,
        s_single_PQ[0],
        s_scanned_entries_every_cell_PQ_lookup_computation_replicated[0],
        s_last_element_valid_PQ_lookup_computation[0], 
        // output streams
        s_distance_LUT_systolic[0],
        s_single_PQ_result[0][0]);

    // PE 1~15 
    for (int j = 1; j < 16; j++) {
#pragma HLS UNROLL
        PQ_lookup_computation<query_num, nprobe>(
            // input streams
            s_distance_LUT_systolic[j - 1],
            s_single_PQ[j],
            s_scanned_entries_every_cell_PQ_lookup_computation_replicated[j],
            s_last_element_valid_PQ_lookup_computation[j], 
            // output streams
            s_distance_LUT_systolic[j],
            s_single_PQ_result[0][j]);
    }

    // PE 16~47
    for (int i = 1; i < 3; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < 16; j++) {
#pragma HLS UNROLL
            PQ_lookup_computation<query_num, nprobe>(
                // input streams
                s_distance_LUT_systolic[i * 16 + j - 1],
                s_single_PQ[i * 16 + j],
                s_scanned_entries_every_cell_PQ_lookup_computation_replicated[i * 16 + j],
                s_last_element_valid_PQ_lookup_computation[i * 16 + j], 
                // output streams
                s_distance_LUT_systolic[i * 16 + j],
                s_single_PQ_result[i][j]);
        }
    }

    // PE 48~63
    for (int j = 0; j < 15; j++) {
#pragma HLS UNROLL
        PQ_lookup_computation<query_num, nprobe>(
            // input streams
            s_distance_LUT_systolic[48 + j - 1],
            s_single_PQ[48 + j],
            s_scanned_entries_every_cell_PQ_lookup_computation_replicated[48 + j],
            s_last_element_valid_PQ_lookup_computation[48 + j], 
            // output streams
            s_distance_LUT_systolic[48 + j],
            s_single_PQ_result[3][j]);
    }

    // consume the systolic output of the last PE
    dummy_distance_LUT_consumer<query_num, nprobe>(
        s_distance_LUT_systolic[62]);

    dummy_PQ_result_sender<query_num, nprobe>(
        s_scanned_entries_every_cell_Dummy, s_single_PQ_result[3][15]);
}

