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

    // Manual control on the double buffer
    float distance_LUT_A[16][256];
#pragma HLS array_partition variable=distance_LUT_A dim=1
#pragma HLS resource variable=distance_LUT_A core=RAM_1P_BRAM
    float distance_LUT_B[16][256];
#pragma HLS array_partition variable=distance_LUT_B dim=1
#pragma HLS resource variable=distance_LUT_B core=RAM_1P_BRAM

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe + 1; nprobe_id++) {

            int last_element_valid;
            if (nprobe_id > 0) {
                // int scanned_entries_every_cell = 
                //     s_scanned_entries_every_cell_PQ_lookup_computation.read();
                last_element_valid = 
                    s_last_element_valid_PQ_lookup_computation.read();
            }
            else {
                // for the first iteration, there's no computation, just randomly
                //  init these variables
                // int scanned_entries_every_cell = 0;
                last_element_valid = 0;
            }

            int max_iter = scanned_entries_every_cell > K?
                scanned_entries_every_cell : K;

            int nprobe_mod = nprobe_id % 2;

            for (int common_iter = 0; common_iter < max_iter; common_iter++) {
#pragma HLS pipeline II=1

                // Note! Use the if else clause such that A and B certainly
                //   won't have bank conflict in a single CC

                // even: load to buffer A, compute by buffer B
                if (nprobe_mod == 0) { 

                    // load part
                    // last iter not read 
                    if (nprobe_id < nprobe) { 

                        // load or not
                        if (common_iter < K) { 

                            distance_LUT_PQ16_t dist_row = s_distance_LUT_in.read();
                            s_distance_LUT_out.write(dist_row);

                            // even: load to buffer A
                            distance_LUT_A[0][common_iter] = dist_row.dist_0; 
                            distance_LUT_A[1][common_iter] = dist_row.dist_1; 
                            distance_LUT_A[2][common_iter] = dist_row.dist_2;
                            distance_LUT_A[3][common_iter] = dist_row.dist_3; 
                            distance_LUT_A[4][common_iter] = dist_row.dist_4; 
                            distance_LUT_A[5][common_iter] = dist_row.dist_5; 
                            distance_LUT_A[6][common_iter] = dist_row.dist_6; 
                            distance_LUT_A[7][common_iter] = dist_row.dist_7; 
                            distance_LUT_A[8][common_iter] = dist_row.dist_8; 
                            distance_LUT_A[9][common_iter] = dist_row.dist_9; 
                            distance_LUT_A[10][common_iter] = dist_row.dist_10; 
                            distance_LUT_A[11][common_iter] = dist_row.dist_11; 
                            distance_LUT_A[12][common_iter] = dist_row.dist_12; 
                            distance_LUT_A[13][common_iter] = dist_row.dist_13; 
                            distance_LUT_A[14][common_iter] = dist_row.dist_14; 
                            distance_LUT_A[15][common_iter] = dist_row.dist_15; 
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
                            
                            out.dist = 
                                distance_LUT_B[0][PQ_local.PQ_code[0]] + 
                                distance_LUT_B[1][PQ_local.PQ_code[1]] + 
                                distance_LUT_B[2][PQ_local.PQ_code[2]] + 
                                distance_LUT_B[3][PQ_local.PQ_code[3]] + 
                                distance_LUT_B[4][PQ_local.PQ_code[4]] + 
                                distance_LUT_B[5][PQ_local.PQ_code[5]] + 
                                distance_LUT_B[6][PQ_local.PQ_code[6]] + 
                                distance_LUT_B[7][PQ_local.PQ_code[7]] + 
                                distance_LUT_B[8][PQ_local.PQ_code[8]] + 
                                distance_LUT_B[9][PQ_local.PQ_code[9]] + 
                                distance_LUT_B[10][PQ_local.PQ_code[10]] + 
                                distance_LUT_B[11][PQ_local.PQ_code[11]] + 
                                distance_LUT_B[12][PQ_local.PQ_code[12]] + 
                                distance_LUT_B[13][PQ_local.PQ_code[13]] + 
                                distance_LUT_B[14][PQ_local.PQ_code[14]] + 
                                distance_LUT_B[15][PQ_local.PQ_code[15]];
                                
                            // for padded element, replace its distance by large number
                            if ((common_iter == (scanned_entries_every_cell - 1)) && (last_element_valid == 0)) {
                                out.vec_ID = -1;
                                out.dist = LARGE_NUM;
                            }
                            s_single_PQ_result.write(out);
                        }
                    }
                }
                // odd: load from buffer B, compute by buffer A
                else {

                    // load part
                    // last iter not read 
                    if (nprobe_id < nprobe) { 

                        // load or not
                        if (common_iter < K) { 

                            distance_LUT_PQ16_t dist_row = s_distance_LUT_in.read();
                            s_distance_LUT_out.write(dist_row);

                            // even: load to buffer A
                            distance_LUT_B[0][common_iter] = dist_row.dist_0; 
                            distance_LUT_B[1][common_iter] = dist_row.dist_1; 
                            distance_LUT_B[2][common_iter] = dist_row.dist_2;
                            distance_LUT_B[3][common_iter] = dist_row.dist_3; 
                            distance_LUT_B[4][common_iter] = dist_row.dist_4; 
                            distance_LUT_B[5][common_iter] = dist_row.dist_5; 
                            distance_LUT_B[6][common_iter] = dist_row.dist_6; 
                            distance_LUT_B[7][common_iter] = dist_row.dist_7; 
                            distance_LUT_B[8][common_iter] = dist_row.dist_8; 
                            distance_LUT_B[9][common_iter] = dist_row.dist_9; 
                            distance_LUT_B[10][common_iter] = dist_row.dist_10; 
                            distance_LUT_B[11][common_iter] = dist_row.dist_11; 
                            distance_LUT_B[12][common_iter] = dist_row.dist_12; 
                            distance_LUT_B[13][common_iter] = dist_row.dist_13; 
                            distance_LUT_B[14][common_iter] = dist_row.dist_14; 
                            distance_LUT_B[15][common_iter] = dist_row.dist_15; 
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
                            
                            out.dist = 
                                distance_LUT_A[0][PQ_local.PQ_code[0]] + 
                                distance_LUT_A[1][PQ_local.PQ_code[1]] + 
                                distance_LUT_A[2][PQ_local.PQ_code[2]] + 
                                distance_LUT_A[3][PQ_local.PQ_code[3]] + 
                                distance_LUT_A[4][PQ_local.PQ_code[4]] + 
                                distance_LUT_A[5][PQ_local.PQ_code[5]] + 
                                distance_LUT_A[6][PQ_local.PQ_code[6]] + 
                                distance_LUT_A[7][PQ_local.PQ_code[7]] + 
                                distance_LUT_A[8][PQ_local.PQ_code[8]] + 
                                distance_LUT_A[9][PQ_local.PQ_code[9]] + 
                                distance_LUT_A[10][PQ_local.PQ_code[10]] + 
                                distance_LUT_A[11][PQ_local.PQ_code[11]] + 
                                distance_LUT_A[12][PQ_local.PQ_code[12]] + 
                                distance_LUT_A[13][PQ_local.PQ_code[13]] + 
                                distance_LUT_A[14][PQ_local.PQ_code[14]] + 
                                distance_LUT_A[15][PQ_local.PQ_code[15]];
                                
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
