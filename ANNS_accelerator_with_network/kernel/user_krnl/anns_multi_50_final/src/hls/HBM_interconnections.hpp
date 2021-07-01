#pragma once 

#include "constants.hpp"
#include "types.hpp"

////////////////////     Declaration     ////////////////////

template<const int query_num, const int nprobe>
void load_PQ_codes(
    const ap_uint512_t* src,
    hls::stream<int>& s_scanned_entries_every_cell,
    hls::stream<int>& s_start_addr_every_cell,
    hls::stream<ap_uint512_t>& s_raw_input);

three_PQ_codes ap_ap_uint512_to_three_PQ_codes(ap_uint<512> in);

template<const int query_num, const int nprobe>
void type_conversion_and_split(
    hls::stream<int>& s_scanned_entries_every_cell,
    hls::stream<ap_uint512_t>& s_raw_input,
    hls::stream<single_PQ>& s_single_PQ_0,
    hls::stream<single_PQ>& s_single_PQ_1,
    hls::stream<single_PQ>& s_single_PQ_2);

template<const int query_num, const int nprobe>
void load_and_split_PQ_codes(
    const ap_uint512_t* HBM_in, // HBM for PQ code + vecID storage
    hls::stream<int>& s_start_addr_every_cell,
    hls::stream<int>& s_scanned_entries_every_cell_Load_unit,
    hls::stream<int>& s_scanned_entries_every_cell_Split_unit,
    hls::stream<single_PQ>& s_single_PQ_0,
    hls::stream<single_PQ>& s_single_PQ_1,
    hls::stream<single_PQ>& s_single_PQ_2);

template<const int query_num, const int nprobe>
void replicate_s_start_addr_every_cell(
    hls::stream<int>& s_start_addr_every_cell,
    hls::stream<int> (&s_start_addr_every_cell_replicated)[HBM_CHANNEL_NUM]);

template<const int query_num, const int nprobe>
void replicate_s_scanned_entries_every_cell_Load_unit(
    hls::stream<int>& s_scanned_entries_every_cell_Load_unit,
    hls::stream<int> (&s_scanned_entries_every_cell_Load_unit_replicated)[HBM_CHANNEL_NUM]);

template<const int query_num, const int nprobe>
void replicate_s_scanned_entries_every_cell_Split_unit(
    hls::stream<int>& s_scanned_entries_every_cell_Split_unit,
    hls::stream<int> (&s_scanned_entries_every_cell_Split_unit_replicated)[HBM_CHANNEL_NUM]);

template<const int query_num, const int nprobe>
void load_and_split_PQ_codes_wrapper(
    const ap_uint512_t* HBM_in0, const ap_uint512_t* HBM_in1, 
    const ap_uint512_t* HBM_in2, const ap_uint512_t* HBM_in3, 
    const ap_uint512_t* HBM_in4, const ap_uint512_t* HBM_in5, 
    const ap_uint512_t* HBM_in6, const ap_uint512_t* HBM_in7, 
    const ap_uint512_t* HBM_in8, 
    // const ap_uint512_t* HBM_in9, 
    // const ap_uint512_t* HBM_in10, 
    // const ap_uint512_t* HBM_in11, 
    // const ap_uint512_t* HBM_in12, const ap_uint512_t* HBM_in13, 
    // const ap_uint512_t* HBM_in14, const ap_uint512_t* HBM_in15, 
    // const ap_uint512_t* HBM_in16, const ap_uint512_t* HBM_in17, 
    // const ap_uint512_t* HBM_in18, const ap_uint512_t* HBM_in19, 
    // const ap_uint512_t* HBM_in20, 
    hls::stream<int>& s_start_addr_every_cell,
    hls::stream<int>& s_scanned_entries_every_cell_Load_unit,
    hls::stream<int>& s_scanned_entries_every_cell_Split_unit,
    hls::stream<single_PQ> (&s_single_PQ)[3 * HBM_CHANNEL_NUM]);

////////////////////     Definition     ////////////////////

template<const int query_num, const int nprobe>
void load_PQ_codes(
    const ap_uint512_t* src,
    hls::stream<int>& s_scanned_entries_every_cell,
    hls::stream<int>& s_start_addr_every_cell,
    hls::stream<ap_uint512_t>& s_raw_input) {

    // s_scanned_entries_every_cell -> 
    //   number of axi width of scanned PQ code per Voronoi cell, 
    //   e.g. AXI width = 512 -> 64 bytes = 20 byte * 3 = 3 PQ code
    //      need scan 299 PQ code ->  axi_num_per_scanned_cell = 100
    //   read number = query_num * nprobe

    for (int query_id = 0; query_id < query_num; query_id++) {
#pragma HLS loop_flatten

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int scanned_entries_every_cell = s_scanned_entries_every_cell.read();
            int start_addr = s_start_addr_every_cell.read();

            for (int entry_id = 0; entry_id < scanned_entries_every_cell; entry_id++) {
#pragma HLS pipeline II=1
                s_raw_input.write(src[start_addr + entry_id]);
            }
        }
    }
}

three_PQ_codes ap_ap_ap_uint512_to_three_PQ_codes(ap_uint<512> in) {
// AXI datawidth of 480 is banned, must use 2^n, e.g., 512
#pragma HLS pipeline
#pragma HLS inline off
#pragma HLS interface port=return register
    three_PQ_codes out;

    ap_uint<32> tmp_int0 = in.range(31 + 0, 0 + 0);
    out.PQ_0.vec_ID = *((int*)(&tmp_int0));
    for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
        ap_uint<8> tmp_char = in.range(0 + 7 + 32 + i * 8, 0 + 32 + i * 8);
        out.PQ_0.PQ_code[i] = *((unsigned char*)(&tmp_char));
    }
    ap_uint<32> tmp_int1 = in.range(31 + 160, 0 + 160);
    out.PQ_1.vec_ID = *((int*)(&tmp_int1));
    for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
        ap_uint<8> tmp_char = in.range(160 + 7 + 32 + i * 8, 160 + 32 + i * 8);
        out.PQ_1.PQ_code[i] = *((unsigned char*)(&tmp_char));
    }
    ap_uint<32> tmp_int2 = in.range(31 + 320, 0 + 320);
    out.PQ_2.vec_ID = *((int*)(&tmp_int2));
    for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
        ap_uint<8> tmp_char = in.range(320 + 7 + 32 + i * 8, 320 + 32 + i * 8);
        out.PQ_2.PQ_code[i] = *((unsigned char*)(&tmp_char));
    }

    return out;
}


template<const int query_num, const int nprobe>
void type_conversion_and_split(
    hls::stream<int>& s_scanned_entries_every_cell,
    hls::stream<ap_uint512_t>& s_raw_input,
    hls::stream<single_PQ>& s_single_PQ_0,
    hls::stream<single_PQ>& s_single_PQ_1,
    hls::stream<single_PQ>& s_single_PQ_2) {


    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int scanned_entries_every_cell = s_scanned_entries_every_cell.read();
            
            for (int entry_id = 0; entry_id < scanned_entries_every_cell; entry_id++) {
#pragma HLS pipeline II=1
                ap_uint512_t in = s_raw_input.read();
                three_PQ_codes out = ap_ap_ap_uint512_to_three_PQ_codes(in);
                s_single_PQ_0.write(out.PQ_0);
                s_single_PQ_1.write(out.PQ_1);
                s_single_PQ_2.write(out.PQ_2);
            }
        }
    }
}


template<const int query_num, const int nprobe>
void load_and_split_PQ_codes(
    const ap_uint512_t* HBM_in, // HBM for PQ code + vecID storage
    hls::stream<int>& s_start_addr_every_cell,
    hls::stream<int>& s_scanned_entries_every_cell_Load_unit,
    hls::stream<int>& s_scanned_entries_every_cell_Split_unit,
    hls::stream<single_PQ>& s_single_PQ_0,
    hls::stream<single_PQ>& s_single_PQ_1,
    hls::stream<single_PQ>& s_single_PQ_2) {

#pragma HLS inline

    hls::stream<ap_uint512_t> s_raw_input; // raw AXI width input

    load_PQ_codes<query_num, nprobe>(
        HBM_in, 
        s_scanned_entries_every_cell_Load_unit, 
        s_start_addr_every_cell, 
        s_raw_input);
    type_conversion_and_split<query_num, nprobe>(
        s_scanned_entries_every_cell_Split_unit,
        s_raw_input, s_single_PQ_0, s_single_PQ_1, s_single_PQ_2);
}


template<const int query_num, const int nprobe>
void replicate_s_start_addr_every_cell(
    hls::stream<int>& s_start_addr_every_cell,
    hls::stream<int> (&s_start_addr_every_cell_replicated)[HBM_CHANNEL_NUM]) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int start_addr_every_cell = s_start_addr_every_cell.read();

            for (int s = 0; s < HBM_CHANNEL_NUM; s++) {
#pragma HLS UNROLL
                s_start_addr_every_cell_replicated[s].write(start_addr_every_cell);
            }
        }
    }
}

template<const int query_num, const int nprobe>
void replicate_s_scanned_entries_every_cell_Load_unit(
    hls::stream<int>& s_scanned_entries_every_cell_Load_unit,
    hls::stream<int> (&s_scanned_entries_every_cell_Load_unit_replicated)[HBM_CHANNEL_NUM]) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int scanned_entries_every_cell_Load_unit = s_scanned_entries_every_cell_Load_unit.read();

            for (int s = 0; s < HBM_CHANNEL_NUM; s++) {
#pragma HLS UNROLL
                s_scanned_entries_every_cell_Load_unit_replicated[s].write(
                    scanned_entries_every_cell_Load_unit);
            }
        }
    }
}


template<const int query_num, const int nprobe>
void replicate_s_scanned_entries_every_cell_Split_unit(
    hls::stream<int>& s_scanned_entries_every_cell_Split_unit,
    hls::stream<int> (&s_scanned_entries_every_cell_Split_unit_replicated)[HBM_CHANNEL_NUM]) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int scanned_entries_every_cell_Split_unit = s_scanned_entries_every_cell_Split_unit.read();

            for (int s = 0; s < HBM_CHANNEL_NUM; s++) {
#pragma HLS UNROLL
                s_scanned_entries_every_cell_Split_unit_replicated[s].write(
                    scanned_entries_every_cell_Split_unit);
            }
        }
    }
}

template<const int query_num, const int nprobe>
void load_and_split_PQ_codes_wrapper(
    const ap_uint512_t* HBM_in0, 
    const ap_uint512_t* HBM_in1, 
    const ap_uint512_t* HBM_in2, 
    const ap_uint512_t* HBM_in3, 
    const ap_uint512_t* HBM_in4, 
    const ap_uint512_t* HBM_in5, 
    const ap_uint512_t* HBM_in6, 
    const ap_uint512_t* HBM_in7, 
    const ap_uint512_t* HBM_in8, 
    // const ap_uint512_t* HBM_in9, 
    // const ap_uint512_t* HBM_in10, 
    // const ap_uint512_t* HBM_in11, 
    // const ap_uint512_t* HBM_in12, 
    // const ap_uint512_t* HBM_in13, 
    // const ap_uint512_t* HBM_in14, 
    // const ap_uint512_t* HBM_in15, 
    // const ap_uint512_t* HBM_in16, 
    // const ap_uint512_t* HBM_in17, 
    // const ap_uint512_t* HBM_in18, 
    // const ap_uint512_t* HBM_in19, 
    // const ap_uint512_t* HBM_in20, 
    // const ap_uint512_t* HBM_in21, 
    // const ap_uint512_t* HBM_in22, 
    // const ap_uint512_t* HBM_in23, 
    // const ap_uint512_t* HBM_in24, 
    // const ap_uint512_t* HBM_in25, 
    // const ap_uint512_t* HBM_in26, 
    hls::stream<int>& s_start_addr_every_cell,
    hls::stream<int>& s_scanned_entries_every_cell_Load_unit,
    hls::stream<int>& s_scanned_entries_every_cell_Split_unit,
    hls::stream<single_PQ> (&s_single_PQ)[3 * HBM_CHANNEL_NUM]) {

#pragma HLS inline

    hls::stream<int> s_start_addr_every_cell_replicated[HBM_CHANNEL_NUM];
#pragma HLS stream variable=s_start_addr_every_cell_replicated depth=8
#pragma HLS array_partition variable=s_start_addr_every_cell_replicated complete
// #pragma HLS RESOURCE variable=s_start_addr_every_cell_replicated core=FIFO_SRL

    hls::stream<int> s_scanned_entries_every_cell_Load_unit_replicated[HBM_CHANNEL_NUM];
#pragma HLS stream variable=s_scanned_entries_every_cell_Load_unit_replicated depth=8
#pragma HLS array_partition variable=s_scanned_entries_every_cell_Load_unit_replicated complete
// #pragma HLS RESOURCE variable=s_scanned_entries_every_cell_Load_unit_replicated core=FIFO_SRL

    hls::stream<int> s_scanned_entries_every_cell_Split_unit_replicated[HBM_CHANNEL_NUM];
#pragma HLS stream variable=s_scanned_entries_every_cell_Split_unit_replicated depth=8
#pragma HLS array_partition variable=s_scanned_entries_every_cell_Split_unit_replicated complete
// #pragma HLS RESOURCE variable=s_scanned_entries_every_cell_Split_unit_replicated core=FIFO_SRL

    replicate_s_start_addr_every_cell<query_num, nprobe>(
        s_start_addr_every_cell, 
        s_start_addr_every_cell_replicated); 

    replicate_s_scanned_entries_every_cell_Load_unit<query_num, nprobe>(
        s_scanned_entries_every_cell_Load_unit, 
        s_scanned_entries_every_cell_Load_unit_replicated); 

    replicate_s_scanned_entries_every_cell_Split_unit<query_num, nprobe>(
        s_scanned_entries_every_cell_Split_unit, 
        s_scanned_entries_every_cell_Split_unit_replicated); 

    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in0, s_start_addr_every_cell_replicated[0], 
        s_scanned_entries_every_cell_Load_unit_replicated[0], 
        s_scanned_entries_every_cell_Split_unit_replicated[0],
        s_single_PQ[0 * 3 + 0], s_single_PQ[0 * 3 + 1], s_single_PQ[0 * 3 + 2]);
    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in1, s_start_addr_every_cell_replicated[1], 
        s_scanned_entries_every_cell_Load_unit_replicated[1], 
        s_scanned_entries_every_cell_Split_unit_replicated[1],
        s_single_PQ[1 * 3 + 0], s_single_PQ[1 * 3 + 1], s_single_PQ[1 * 3 + 2]);
    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in2, s_start_addr_every_cell_replicated[2], 
        s_scanned_entries_every_cell_Load_unit_replicated[2], 
        s_scanned_entries_every_cell_Split_unit_replicated[2],
        s_single_PQ[2 * 3 + 0], s_single_PQ[2 * 3 + 1], s_single_PQ[2 * 3 + 2]);
    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in3, s_start_addr_every_cell_replicated[3], 
        s_scanned_entries_every_cell_Load_unit_replicated[3], 
        s_scanned_entries_every_cell_Split_unit_replicated[3],
        s_single_PQ[3 * 3 + 0], s_single_PQ[3 * 3 + 1], s_single_PQ[3 * 3 + 2]);
    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in4, s_start_addr_every_cell_replicated[4], 
        s_scanned_entries_every_cell_Load_unit_replicated[4], 
        s_scanned_entries_every_cell_Split_unit_replicated[4],
        s_single_PQ[4 * 3 + 0], s_single_PQ[4 * 3 + 1], s_single_PQ[4 * 3 + 2]);
    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in5, s_start_addr_every_cell_replicated[5], 
        s_scanned_entries_every_cell_Load_unit_replicated[5], 
        s_scanned_entries_every_cell_Split_unit_replicated[5],
        s_single_PQ[5 * 3 + 0], s_single_PQ[5 * 3 + 1], s_single_PQ[5 * 3 + 2]);
    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in6, s_start_addr_every_cell_replicated[6], 
        s_scanned_entries_every_cell_Load_unit_replicated[6], 
        s_scanned_entries_every_cell_Split_unit_replicated[6],
        s_single_PQ[6 * 3 + 0], s_single_PQ[6 * 3 + 1], s_single_PQ[6 * 3 + 2]);
    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in7, s_start_addr_every_cell_replicated[7], 
        s_scanned_entries_every_cell_Load_unit_replicated[7], 
        s_scanned_entries_every_cell_Split_unit_replicated[7],
        s_single_PQ[7 * 3 + 0], s_single_PQ[7 * 3 + 1], s_single_PQ[7 * 3 + 2]);
    load_and_split_PQ_codes<query_num, nprobe>(
        HBM_in8, s_start_addr_every_cell_replicated[8], 
        s_scanned_entries_every_cell_Load_unit_replicated[8], 
        s_scanned_entries_every_cell_Split_unit_replicated[8],
        s_single_PQ[8 * 3 + 0], s_single_PQ[8 * 3 + 1], s_single_PQ[8 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in9, s_start_addr_every_cell_replicated[9], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[9], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[9],
    //     s_single_PQ[9 * 3 + 0], s_single_PQ[9 * 3 + 1], s_single_PQ[9 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in10, s_start_addr_every_cell_replicated[10], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[10], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[10],
    //     s_single_PQ[10 * 3 + 0], s_single_PQ[10 * 3 + 1], s_single_PQ[10 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in11, s_start_addr_every_cell_replicated[11], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[11], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[11],
    //     s_single_PQ[11 * 3 + 0], s_single_PQ[11 * 3 + 1], s_single_PQ[11 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in12, s_start_addr_every_cell_replicated[12], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[12], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[12],
    //     s_single_PQ[12 * 3 + 0], s_single_PQ[12 * 3 + 1], s_single_PQ[12 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in13, s_start_addr_every_cell_replicated[13], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[13], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[13],
    //     s_single_PQ[13 * 3 + 0], s_single_PQ[13 * 3 + 1], s_single_PQ[13 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in14, s_start_addr_every_cell_replicated[14], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[14], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[14],
    //     s_single_PQ[14 * 3 + 0], s_single_PQ[14 * 3 + 1], s_single_PQ[14 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in15, s_start_addr_every_cell_replicated[15], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[15], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[15],
    //     s_single_PQ[15 * 3 + 0], s_single_PQ[15 * 3 + 1], s_single_PQ[15 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in16, s_start_addr_every_cell_replicated[16], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[16], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[16],
    //     s_single_PQ[16 * 3 + 0], s_single_PQ[16 * 3 + 1], s_single_PQ[16 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in17, s_start_addr_every_cell_replicated[17], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[17], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[17],
    //     s_single_PQ[17 * 3 + 0], s_single_PQ[17 * 3 + 1], s_single_PQ[17 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in18, s_start_addr_every_cell_replicated[18], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[18], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[18],
    //     s_single_PQ[18 * 3 + 0], s_single_PQ[18 * 3 + 1], s_single_PQ[18 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in19, s_start_addr_every_cell_replicated[19], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[19], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[19],
    //     s_single_PQ[19 * 3 + 0], s_single_PQ[19 * 3 + 1], s_single_PQ[19 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in20, s_start_addr_every_cell_replicated[20], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[20], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[20],
    //     s_single_PQ[20 * 3 + 0], s_single_PQ[20 * 3 + 1], s_single_PQ[20 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in21, s_start_addr_every_cell_replicated[21], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[21], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[21],
    //     s_single_PQ[21 * 3 + 0], s_single_PQ[21 * 3 + 1], s_single_PQ[21 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in22, s_start_addr_every_cell_replicated[22], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[22], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[22],
    //     s_single_PQ[22 * 3 + 0], s_single_PQ[22 * 3 + 1], s_single_PQ[22 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in23, s_start_addr_every_cell_replicated[23], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[23], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[23],
    //     s_single_PQ[23 * 3 + 0], s_single_PQ[23 * 3 + 1], s_single_PQ[23 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in24, s_start_addr_every_cell_replicated[24], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[24], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[24],
    //     s_single_PQ[24 * 3 + 0], s_single_PQ[24 * 3 + 1], s_single_PQ[24 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in25, s_start_addr_every_cell_replicated[25], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[25], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[25],
    //     s_single_PQ[25 * 3 + 0], s_single_PQ[25 * 3 + 1], s_single_PQ[25 * 3 + 2]);
    // load_and_split_PQ_codes<query_num, nprobe>(
    //     HBM_in26, s_start_addr_every_cell_replicated[26], 
    //     s_scanned_entries_every_cell_Load_unit_replicated[26], 
    //     s_scanned_entries_every_cell_Split_unit_replicated[26],
    //     s_single_PQ[26 * 3 + 0], s_single_PQ[26 * 3 + 1], s_single_PQ[26 * 3 + 2]);
}

void init_distance_table_partition(float table[512], float bias) {
#pragma HLS inline
    for (int j = 0; j < 512; j++) {
#pragma HLS pipeline II=1
        table[j] = bias + j;
    }
}
