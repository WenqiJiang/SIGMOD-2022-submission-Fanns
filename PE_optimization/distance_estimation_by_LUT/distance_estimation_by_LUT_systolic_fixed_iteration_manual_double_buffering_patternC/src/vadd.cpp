// This program:
//   1. stora all the cell centroids coordinates on-chip
//   2. given an input query, compute distance with those centroids
//   3. select the top nprobe centroids using sorting network / heap
//   4. access the cell codebooks stored in HBM, construct distance table
#include <stdio.h>
#include "vadd.hpp"


void vadd(  
    const t_axi* table_HBM0, const t_axi* table_HBM1, 
    const t_axi* table_HBM2, const t_axi* table_HBM3, 
    const t_axi* table_HBM4, const t_axi* table_HBM5, 
    const t_axi* table_HBM6, const t_axi* table_HBM7, 
    const t_axi* table_HBM8, const t_axi* table_HBM9, 
    const t_axi* table_HBM10, const t_axi* table_HBM11, 
    const t_axi* table_HBM12, const t_axi* table_HBM13, 
    const t_axi* table_HBM14, const t_axi* table_HBM15, 
    const t_axi* table_HBM16, const t_axi* table_HBM17, 
    const t_axi* table_HBM18, const t_axi* table_HBM19, 
    const t_axi* table_HBM20, const t_axi* table_HBM21, 
    const t_axi* table_HBM22, const t_axi* table_HBM23, 
    const t_axi* table_HBM24, const t_axi* table_HBM25, 
    const t_axi* table_HBM26, const t_axi* table_HBM27, 
    const t_axi* table_HBM28, const t_axi* table_HBM29, 
    const t_axi* table_HBM30, const t_axi* table_HBM31, 
    const t_axi* table_DDR0, const t_axi* table_DDR1,
    ap_uint<64>* out_PLRAM
    )
{
#pragma HLS INTERFACE m_axi port=table_HBM0  offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=table_HBM1  offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=table_HBM2  offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=table_HBM3  offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi port=table_HBM4  offset=slave bundle=gmem4
#pragma HLS INTERFACE m_axi port=table_HBM5  offset=slave bundle=gmem5
#pragma HLS INTERFACE m_axi port=table_HBM6  offset=slave bundle=gmem6
#pragma HLS INTERFACE m_axi port=table_HBM7  offset=slave bundle=gmem7
#pragma HLS INTERFACE m_axi port=table_HBM8  offset=slave bundle=gmem8
#pragma HLS INTERFACE m_axi port=table_HBM9  offset=slave bundle=gmem9
#pragma HLS INTERFACE m_axi port=table_HBM10  offset=slave bundle=gmem10
#pragma HLS INTERFACE m_axi port=table_HBM11  offset=slave bundle=gmem11
#pragma HLS INTERFACE m_axi port=table_HBM12  offset=slave bundle=gmem12
#pragma HLS INTERFACE m_axi port=table_HBM13  offset=slave bundle=gmem13
#pragma HLS INTERFACE m_axi port=table_HBM14  offset=slave bundle=gmem14
#pragma HLS INTERFACE m_axi port=table_HBM15  offset=slave bundle=gmem15
#pragma HLS INTERFACE m_axi port=table_HBM16  offset=slave bundle=gmem16
#pragma HLS INTERFACE m_axi port=table_HBM17  offset=slave bundle=gmem17
#pragma HLS INTERFACE m_axi port=table_HBM18  offset=slave bundle=gmem18
#pragma HLS INTERFACE m_axi port=table_HBM19  offset=slave bundle=gmem19
#pragma HLS INTERFACE m_axi port=table_HBM20  offset=slave bundle=gmem20
#pragma HLS INTERFACE m_axi port=table_HBM21  offset=slave bundle=gmem21
#pragma HLS INTERFACE m_axi port=table_HBM22  offset=slave bundle=gmem22
#pragma HLS INTERFACE m_axi port=table_HBM23  offset=slave bundle=gmem23
#pragma HLS INTERFACE m_axi port=table_HBM24  offset=slave bundle=gmem24
#pragma HLS INTERFACE m_axi port=table_HBM25  offset=slave bundle=gmem25
#pragma HLS INTERFACE m_axi port=table_HBM26  offset=slave bundle=gmem26
#pragma HLS INTERFACE m_axi port=table_HBM27  offset=slave bundle=gmem27
#pragma HLS INTERFACE m_axi port=table_HBM28  offset=slave bundle=gmem28
#pragma HLS INTERFACE m_axi port=table_HBM29  offset=slave bundle=gmem29
#pragma HLS INTERFACE m_axi port=table_HBM30  offset=slave bundle=gmem30
#pragma HLS INTERFACE m_axi port=table_HBM31  offset=slave bundle=gmem31

#pragma HLS INTERFACE m_axi port=table_DDR0  offset=slave bundle=gmem32
#pragma HLS INTERFACE m_axi port=table_DDR1  offset=slave bundle=gmem33

// PLRAM
#pragma HLS INTERFACE m_axi port=out_PLRAM offset=slave bundle=gmem34

#pragma HLS INTERFACE s_axilite port=table_HBM0  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM1  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM2  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM3  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM4  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM5  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM6  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM7  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM8  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM9  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM10  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM11  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM12  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM13  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM14  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM15  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM16  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM17  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM18  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM19  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM20  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM21  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM22  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM23  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM24  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM25  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM26  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM27  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM28  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM29  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM30  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM31  bundle=control

#pragma HLS INTERFACE s_axilite port=table_DDR0  bundle=control
#pragma HLS INTERFACE s_axilite port=table_DDR1  bundle=control

#pragma HLS INTERFACE s_axilite port=out_PLRAM bundle=control

#pragma HLS INTERFACE s_axilite port=return bundle=control
    
#pragma HLS dataflow

    hls::stream<single_PQ> s_PQ_codes;
#pragma HLS stream variable=s_PQ_codes depth=8
#pragma HLS resource variable=s_PQ_codes core=FIFO_SRL

    hls::stream<single_PQ_result> s_single_PQ_result;
#pragma HLS stream variable=s_single_PQ_result depth=8
#pragma HLS resource variable=s_single_PQ_result core=FIFO_SRL

    hls::stream<int> s_last_element_valid_PQ_lookup_computation;
#pragma HLS stream variable=s_last_element_valid_PQ_lookup_computation depth=8
#pragma HLS resource variable=s_last_element_valid_PQ_lookup_computation core=FIFO_SRL

    hls::stream<distance_LUT_PQ16_t> s_distance_LUT_in;
#pragma HLS stream variable=s_distance_LUT_in depth=8
#pragma HLS resource variable=s_distance_LUT_in core=FIFO_SRL

    hls::stream<distance_LUT_PQ16_t> s_distance_LUT_out;
#pragma HLS stream variable=s_distance_LUT_out depth=8
#pragma HLS resource variable=s_distance_LUT_out core=FIFO_SRL

    send_PE_codes<QUERY_NUM, NPROBE, SCANNED_ENTRIES_PER_CELL>(
        s_PQ_codes);

    dummy_last_element_valid_sender<QUERY_NUM,  NPROBE>(
        s_last_element_valid_PQ_lookup_computation);

    dummy_distance_LUT_sender<QUERY_NUM,  NPROBE>(
        s_distance_LUT_in);

////////////////////    Core Function Starts     //////////////////// 

    PQ_lookup_computation<QUERY_NUM, NPROBE, SCANNED_ENTRIES_PER_CELL>(
        // input streams
        s_distance_LUT_in,
        s_PQ_codes, 
        s_last_element_valid_PQ_lookup_computation,
        // output streams
        s_distance_LUT_out,
        s_single_PQ_result);

////////////////////    Core Function Ends     //////////////////// 

    dummy_distance_LUT_consumer<QUERY_NUM,  NPROBE>(
        s_distance_LUT_out);

    write_result<QUERY_NUM, NPROBE, SCANNED_ENTRIES_PER_CELL>(
        s_single_PQ_result, 
        out_PLRAM);
}


template<const int query_num, const int nprobe, const int scanned_entries_every_cell>
void send_PE_codes(
    hls::stream<single_PQ>& s_single_PQ) {

    single_PQ reg;
    reg.vec_ID = 100;
    for (int i = 0; i < M; i++) {
        reg.PQ_code[i] = 0;
    }

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            for (int entry_id = 0; entry_id < scanned_entries_every_cell; entry_id++) {
#pragma HLS pipeline II=1
                s_single_PQ.write(reg);
            }
        }
    }
}


template<const int query_num, const int nprobe>
void dummy_last_element_valid_sender(
    hls::stream<int> &s_last_element_valid_PQ_lookup_computation) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            // invalid, padded element
            s_last_element_valid_PQ_lookup_computation.write(0); 
        }
    }
}

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

template<const int query_num, const int nprobe, const int scanned_entries_every_cell>
void PQ_lookup_computation(
    // input streams
    hls::stream<distance_LUT_PQ16_t>& s_distance_LUT_in,
    hls::stream<single_PQ>& s_single_PQ,
    // hls::stream<int>& s_scanned_entries_every_cell_PQ_lookup_computation,
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

            // int scanned_entries_every_cell = 
            //     s_scanned_entries_every_cell_PQ_lookup_computation.read();
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

                        // even: load to buffer A
                        if (nprobe_mod == 0) { 
                            distance_LUT_0[common_iter] = dist_row.dist_0; 
                            distance_LUT_1[common_iter] = dist_row.dist_1; 
                            distance_LUT_2[common_iter] = dist_row.dist_2;
                            distance_LUT_3[common_iter] = dist_row.dist_3; 
                            distance_LUT_4[common_iter] = dist_row.dist_4; 
                            distance_LUT_5[common_iter] = dist_row.dist_5; 
                            distance_LUT_6[common_iter] = dist_row.dist_6; 
                            distance_LUT_7[common_iter] = dist_row.dist_7; 
                            distance_LUT_8[common_iter] = dist_row.dist_8; 
                            distance_LUT_9[common_iter] = dist_row.dist_9; 
                            distance_LUT_10[common_iter] = dist_row.dist_10; 
                            distance_LUT_11[common_iter] = dist_row.dist_11; 
                            distance_LUT_12[common_iter] = dist_row.dist_12; 
                            distance_LUT_13[common_iter] = dist_row.dist_13; 
                            distance_LUT_14[common_iter] = dist_row.dist_14; 
                            distance_LUT_15[common_iter] = dist_row.dist_15; 
                        }
                        // odd: load to buffer B
                        else { 
                            distance_LUT_0[256 + common_iter] = dist_row.dist_0;  
                            distance_LUT_1[256 + common_iter] = dist_row.dist_1; 
                            distance_LUT_2[256 + common_iter] = dist_row.dist_2;
                            distance_LUT_3[256 + common_iter] = dist_row.dist_3; 
                            distance_LUT_4[256 + common_iter] = dist_row.dist_4; 
                            distance_LUT_5[256 + common_iter] = dist_row.dist_5; 
                            distance_LUT_6[256 + common_iter] = dist_row.dist_6; 
                            distance_LUT_7[256 + common_iter] = dist_row.dist_7; 
                            distance_LUT_8[256 + common_iter] = dist_row.dist_8; 
                            distance_LUT_9[256 + common_iter] = dist_row.dist_9; 
                            distance_LUT_10[256 + common_iter] = dist_row.dist_10; 
                            distance_LUT_11[256 + common_iter] = dist_row.dist_11; 
                            distance_LUT_12[256 + common_iter] = dist_row.dist_12; 
                            distance_LUT_13[256 + common_iter] = dist_row.dist_13; 
                            distance_LUT_14[256 + common_iter] = dist_row.dist_14; 
                            distance_LUT_15[256 + common_iter] = dist_row.dist_15;
                        }
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

                        // if odd, compute using buffer A 
                        if (nprobe_mod ==  1) {
                            out.dist = 
                                distance_LUT_0[PQ_local.PQ_code[0]] + 
                                distance_LUT_1[PQ_local.PQ_code[1]] + 
                                distance_LUT_2[PQ_local.PQ_code[2]] + 
                                distance_LUT_3[PQ_local.PQ_code[3]] + 
                                distance_LUT_4[PQ_local.PQ_code[4]] + 
                                distance_LUT_5[PQ_local.PQ_code[5]] + 
                                distance_LUT_6[PQ_local.PQ_code[6]] + 
                                distance_LUT_7[PQ_local.PQ_code[7]] + 
                                distance_LUT_8[PQ_local.PQ_code[8]] + 
                                distance_LUT_9[PQ_local.PQ_code[9]] + 
                                distance_LUT_10[PQ_local.PQ_code[10]] + 
                                distance_LUT_11[PQ_local.PQ_code[11]] + 
                                distance_LUT_12[PQ_local.PQ_code[12]] + 
                                distance_LUT_13[PQ_local.PQ_code[13]] + 
                                distance_LUT_14[PQ_local.PQ_code[14]] + 
                                distance_LUT_15[PQ_local.PQ_code[15]];
                        }
                        // if even, compute using buffer B
                        else {
                            out.dist = 
                                distance_LUT_0[((unsigned int) 256) + PQ_local.PQ_code[0]] + 
                                distance_LUT_1[((unsigned int) 256) + PQ_local.PQ_code[1]] + 
                                distance_LUT_2[((unsigned int) 256) + PQ_local.PQ_code[2]] + 
                                distance_LUT_3[((unsigned int) 256) + PQ_local.PQ_code[3]] + 
                                distance_LUT_4[((unsigned int) 256) + PQ_local.PQ_code[4]] + 
                                distance_LUT_5[((unsigned int) 256) + PQ_local.PQ_code[5]] + 
                                distance_LUT_6[((unsigned int) 256) + PQ_local.PQ_code[6]] + 
                                distance_LUT_7[((unsigned int) 256) + PQ_local.PQ_code[7]] + 
                                distance_LUT_8[((unsigned int) 256) + PQ_local.PQ_code[8]] + 
                                distance_LUT_9[((unsigned int) 256) + PQ_local.PQ_code[9]] + 
                                distance_LUT_10[((unsigned int) 256) + PQ_local.PQ_code[10]] + 
                                distance_LUT_11[((unsigned int) 256) + PQ_local.PQ_code[11]] + 
                                distance_LUT_12[((unsigned int) 256) + PQ_local.PQ_code[12]] + 
                                distance_LUT_13[((unsigned int) 256) + PQ_local.PQ_code[13]] + 
                                distance_LUT_14[((unsigned int) 256) + PQ_local.PQ_code[14]] + 
                                distance_LUT_15[((unsigned int) 256) + PQ_local.PQ_code[15]];
                            }
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

template<const int query_num, const int nprobe, const int scanned_entries_every_cell>
void write_result(
    hls::stream<single_PQ_result>& s_result, ap_uint<64>* results_out) {
    
    single_PQ_result reg;

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            for (int entry_id = 0; entry_id < scanned_entries_every_cell; entry_id++) {
#pragma HLS pipeline II=1
                reg = s_result.read();
            }
        }
    }

    // only write the last value to out
    int vec_ID = reg.vec_ID;
    float dist = reg.dist;
    ap_uint<32> vec_ID_ap = *((ap_uint<32>*) (&vec_ID));
    ap_uint<32> dist_ap = *((ap_uint<32>*) (&dist));
    results_out[0].range(31, 0) = vec_ID_ap;
    results_out[0].range(63, 32) = dist_ap;
}
