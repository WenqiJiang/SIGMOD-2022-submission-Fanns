#pragma once 

#include "constants.hpp"
#include "types.hpp"

template<const int query_num, const int nlist, const int nprobe>
void scan_controller(
    const int* HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid,
    hls::stream<int> &s_scanned_cell_id_Input, // from the cluster selection unit
    hls::stream<int> &s_start_addr_every_cell,
    hls::stream<int> &s_scanned_entries_every_cell_Load_unit,
    hls::stream<int> &s_scanned_entries_every_cell_Split_unit,
    hls::stream<int> &s_scanned_entries_every_cell_PQ_lookup_computation,
    hls::stream<int> &s_scanned_entries_every_cell_Dummy,
    hls::stream<int> &s_last_valid_channel,
    hls::stream<int> &s_scanned_entries_per_query_Sort_and_reduction,
    hls::stream<int> &s_scanned_entries_per_query_Priority_queue);

template<const int query_num>
void merge_partial_cell_distance(
    hls::stream<dist_cell_ID_t> (&s_partial_cell_distance)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<dist_cell_ID_t>& s_merged_cell_distance);

template<const int query_num>
void write_result(
    hls::stream<single_PQ_result> &output_stream, 
    ap_uint64_t* output);

template<const int query_num, const int nlist, const int nprobe>
void scan_controller(
    const int* HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid,
    hls::stream<int> &s_scanned_cell_id_Input, // from the cluster selection unit
    hls::stream<int> &s_start_addr_every_cell,
    hls::stream<int> &s_scanned_entries_every_cell_Load_unit,
    hls::stream<int> &s_scanned_entries_every_cell_Split_unit,
    hls::stream<int> &s_scanned_entries_every_cell_PQ_lookup_computation,
    hls::stream<int> &s_scanned_entries_every_cell_Dummy,
    hls::stream<int> &s_last_valid_channel,
    hls::stream<int> &s_scanned_entries_per_query_Sort_and_reduction,
    hls::stream<int> &s_scanned_entries_per_query_Priority_queue) {
   
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
        start_addr_LUT[i] = HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid[i];
    }
    for (int i = 0; i < nlist; i++) {
#pragma HLS pipeline II=1
        scanned_entries_every_cell_LUT[i] = 
            HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid[nlist + i];
    }
    // ---- Fixed ----
    for (int i = 0; i < nlist; i++) {
#pragma HLS pipeline II=1
        last_valid_channel_LUT[i] = 
            HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid[2 * nlist + i];
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

        s_scanned_entries_per_query_Sort_and_reduction.write(accumulated_scanned_entries_per_query);
        s_scanned_entries_per_query_Priority_queue.write(accumulated_scanned_entries_per_query);
    }
}

template<const int query_num>
void merge_partial_cell_distance(
    hls::stream<dist_cell_ID_t> (&s_partial_cell_distance)[PE_NUM_CENTER_DIST_COMP],
    hls::stream<dist_cell_ID_t>& s_merged_cell_distance) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int c = 0; c < CENTROIDS_PER_PARTITION; c++) {

            for (int s = 0; s < PE_NUM_CENTER_DIST_COMP; s++) {
#pragma HLS pipeline II=1
                s_merged_cell_distance.write(s_partial_cell_distance[s].read());
            }
        }
    }
}

template<const int query_num>
void write_result(
    hls::stream<single_PQ_result> &output_stream, 
    ap_uint64_t* output) {

    for (int i = 0; i < query_num * PRIORITY_QUEUE_LEN; i++) {
#pragma HLS pipeline II=1
        single_PQ_result raw_output = output_stream.read();
        ap_uint<64> reg;
        int vec_ID = raw_output.vec_ID;
        float dist = raw_output.dist;
        reg.range(31, 0) = *((ap_uint<32>*) (&vec_ID));
        reg.range(63, 32) = *((ap_uint<32>*) (&dist));
        output[i] = reg;
    }
}