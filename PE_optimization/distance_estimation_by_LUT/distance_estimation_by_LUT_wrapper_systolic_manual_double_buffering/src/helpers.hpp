#include "constants.hpp"

template<const int query_num, const int nprobe>
void generate_scanned_cell_id(hls::stream<int> &s_scanned_cell_id);

template<const int query_num, const int nlist, const int nprobe>
void scan_controller(
    const int* HBM_info_start_addr_and_scanned_entries_every_cell,
    hls::stream<int> &s_scanned_cell_id_Input, // from the cluster selection unit
    hls::stream<int> &s_start_addr_every_cell,
    hls::stream<int> &s_scanned_entries_every_cell_Load_unit,
    hls::stream<int> &s_scanned_entries_every_cell_Split_unit,
    hls::stream<int> &s_scanned_entries_every_cell_PQ_lookup_computation,
    hls::stream<int> &s_scanned_entries_every_cell_Dummy,
    hls::stream<int> &s_last_valid_channel,
    hls::stream<int> &s_scanned_entries_per_query_Write_results);

template<const int query_num>
void write_result(
    hls::stream<int> &s_scanned_entries_per_query_Write_results,
    hls::stream<single_PQ_result> (&s_single_PQ_result)[4][16],
    ap_uint<64>* output);


template<const int query_num, const int nprobe>
void generate_scanned_cell_id(hls::stream<int> &s_scanned_cell_id) {

    for (int query_id = 0; query_id < query_num; query_id++) {
        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {
#pragma HLS pipeline II=1
            s_scanned_cell_id.write(nprobe_id);
        }
    }
}


template<const int query_num, const int nlist, const int nprobe>
void scan_controller(
    const int* HBM_info_start_addr_and_scanned_entries_every_cell,
    hls::stream<int> &s_scanned_cell_id_Input, // from the cluster selection unit
    hls::stream<int> &s_start_addr_every_cell,
    hls::stream<int> &s_scanned_entries_every_cell_Load_unit,
    hls::stream<int> &s_scanned_entries_every_cell_Split_unit,
    hls::stream<int> &s_scanned_entries_every_cell_PQ_lookup_computation,
    hls::stream<int> &s_scanned_entries_every_cell_Dummy,
    hls::stream<int> &s_last_valid_channel,
    hls::stream<int> &s_scanned_entries_per_query_Write_results) {
   
    // s_last_element_valid_PQ_lookup_computation -> last element of a channel can 
    //   be padded or not, 1 means valid (not padded), 0 means padded, should be discarded
    // last_valid_channel_LUT -> for each Voronoi cell, the last line in HBM may contain 
    //   padding, this is for storing where the last non-padded element id, ranges from 0~62
    //   e.g., all 63 elements store valid element, then last_valid_channel_LUT[x] = 62
    //   e.g., only the first channels's first element is valid, then last_valid_channel_LUT[x] = 0 
    int start_addr_LUT[nlist];
    int scanned_entries_every_cell_LUT[nlist];
    int last_valid_channel_LUT[nlist];  
#pragma HLS resource variable=start_addr_LUT core=RAM_2P_URAM
#pragma HLS resource variable=scanned_entries_every_cell_LUT core=RAM_2P_URAM
#pragma HLS resource variable=last_valid_channel_LUT core=RAM_2P_URAM

    // init LUTs
    for (int i = 0; i < nlist; i++) {
#pragma HLS pipeline II=1
        start_addr_LUT[i] = HBM_info_start_addr_and_scanned_entries_every_cell[i];
    }
    for (int i = 0; i < nlist; i++) {
#pragma HLS pipeline II=1
        scanned_entries_every_cell_LUT[i] = 
            HBM_info_start_addr_and_scanned_entries_every_cell[nlist + i];
    }
    // ---- Fixed ----
    for (int i = 0; i < nlist; i++) {
#pragma HLS pipeline II=1
        last_valid_channel_LUT[i] = 
            HBM_info_start_addr_and_scanned_entries_every_cell[2 * nlist + i];
    }

    // send control signals
    for (int query_id = 0; query_id < query_num; query_id++) {
        
        int accumulated_scanned_entries_per_query = 0;

        for (int nprobe_id = 0; nprobe_id < nprobe; nprobe_id++) {

            int cell_id = s_scanned_cell_id_Input.read();

            int start_addr = start_addr_LUT[cell_id];
            int scanned_entries_every_cell = scanned_entries_every_cell_LUT[cell_id];
            int last_valid_channel = last_valid_channel_LUT[cell_id];

            accumulated_scanned_entries_per_query += scanned_entries_every_cell;

            s_start_addr_every_cell.write(start_addr);
            s_scanned_entries_every_cell_Load_unit.write(scanned_entries_every_cell);
            s_scanned_entries_every_cell_Split_unit.write(scanned_entries_every_cell);
            s_scanned_entries_every_cell_Dummy.write(scanned_entries_every_cell);
            s_scanned_entries_every_cell_PQ_lookup_computation.write(scanned_entries_every_cell);
            s_last_valid_channel.write(last_valid_channel);
        }

        s_scanned_entries_per_query_Write_results.write(accumulated_scanned_entries_per_query);
    }
}






template<const int query_num>
void write_result(
    hls::stream<int> &s_scanned_entries_per_query_Write_results,
    hls::stream<single_PQ_result> (&s_single_PQ_result)[4][16],
    ap_uint<64>* output) {

    single_PQ_result results[4][16];
#pragma HLS array_partition variable=results complete

    // first query_num - 1 iterations
    for (int query_id = 0; query_id < query_num - 1; query_id++) {

        int scanned_entries_per_query = s_scanned_entries_per_query_Write_results.read();

        for (int entry_id = 0; entry_id < scanned_entries_per_query; entry_id++) {
#pragma HLS pipeline II=1

            for (int s1 = 0; s1 < 4; s1++) {
#pragma HLS UNROLL
                for (int s2 = 0; s2 < 16; s2++) {
#pragma HLS UNROLL
                    s_single_PQ_result[s1][s2].read();
                }
            }
        }
    }

    // last iteration
    for (int query_id = 0; query_id < 1; query_id++) {

        int scanned_entries_per_query = s_scanned_entries_per_query_Write_results.read();

        // discard all but the last
        for (int entry_id = 0; entry_id < scanned_entries_per_query - 1; entry_id++) {
#pragma HLS pipeline II=1

            for (int s1 = 0; s1 < 4; s1++) {
#pragma HLS UNROLL
                for (int s2 = 0; s2 < 16; s2++) {
#pragma HLS UNROLL
                    s_single_PQ_result[s1][s2].read();
                }
            }
        }

        for (int s1 = 0; s1 < 4; s1++) {
#pragma HLS UNROLL
            for (int s2 = 0; s2 < 16; s2++) {
#pragma HLS UNROLL
                results[s1][s2] = s_single_PQ_result[s1][s2].read();
            }
        }
    }

    for (int s1 = 0; s1 < 4; s1++) {

        for (int s2 = 0; s2 < 16; s2++) {

            ap_uint<64> reg;
            int vec_ID = results[s1][s2].vec_ID;
            float dist = results[s1][s2].dist;
            reg.range(31, 0) = *((ap_uint<32>*) (&vec_ID));
            reg.range(63, 32) = *((ap_uint<32>*) (&dist));
            output[s1 * 16 + s2] = reg;
        }
    }
}
