#pragma once 

#include "constants.hpp"
#include "types.hpp"
#include "priority_queue_vector_quantizer.hpp"

////////////////////     Function to call in top-level     ////////////////////
void select_Voronoi_cell(
    hls::stream<dist_cell_ID_t> &s_distance_cell_ID,
    hls::stream<dist_cell_ID_t> &s_selected_distance_cell_ID);
////////////////////     Function to call in top-level     ////////////////////

template<const int query_num>
void split_distance_cell_ID(
    hls::stream<dist_cell_ID_t> &s_distance_cell_ID,
    hls::stream<dist_cell_ID_t> (&s_distance_cell_ID_level_A)[2]) {

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int c = 0; c < NLIST / 2; c++) {
#pragma HLS pipeline II=2
            s_distance_cell_ID_level_A[0].write(s_distance_cell_ID.read());
            s_distance_cell_ID_level_A[1].write(s_distance_cell_ID.read());
        }
    }
}

template<const int query_num>
void merge_distance_cell_ID_level_A(
    hls::stream<dist_cell_ID_t> (&s_result_level_A)[2],
    hls::stream<dist_cell_ID_t> &s_distance_cell_ID_level_B) {
    
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int c = 0; c < NPROBE; c++) {
#pragma HLS pipeline II=2
            s_distance_cell_ID_level_B.write(s_result_level_A[0].read());
            s_distance_cell_ID_level_B.write(s_result_level_A[1].read());
        }
    }
}

template<const int level_num, const int L1_queue_num>
void select_Voronoi_cell(
    hls::stream<dist_cell_ID_t> &s_distance_cell_ID,
    hls::stream<dist_cell_ID_t> &s_selected_distance_cell_ID);


template<>
void select_Voronoi_cell<1, 1>(
    hls::stream<dist_cell_ID_t> &s_distance_cell_ID,
    hls::stream<dist_cell_ID_t> &s_selected_distance_cell_ID) {
#pragma HLS inline

    // the depth of this priority queue is nprobe
    Priority_queue<dist_cell_ID_t, NPROBE, Collect_smallest> priority_queue_level_B;

    priority_queue_level_B.insert_wrapper<QUERY_NUM, NLIST>(
        s_distance_cell_ID, 
        s_selected_distance_cell_ID); 

}

template<>
void select_Voronoi_cell<2, 2>(
    hls::stream<dist_cell_ID_t> &s_distance_cell_ID,
    hls::stream<dist_cell_ID_t> &s_selected_distance_cell_ID) {
#pragma HLS inline

    hls::stream<dist_cell_ID_t> s_distance_cell_ID_level_A[2];
#pragma HLS stream variable=s_distance_cell_ID_level_A depth=2
#pragma HLS array_partition variable=s_distance_cell_ID_level_A complete

    split_distance_cell_ID<QUERY_NUM>(
        s_distance_cell_ID,
        s_distance_cell_ID_level_A);

    hls::stream<dist_cell_ID_t> s_result_level_A[2];
#pragma HLS stream variable=s_result_level_A depth=512
#pragma HLS array_partition variable=s_result_level_A complete

    // the depth of this priority queue is nprobe
    Priority_queue<dist_cell_ID_t, NPROBE, Collect_smallest> priority_queue_level_A[2];
#pragma HLS array_partition variable=priority_queue_level_A complete

    for (int s = 0; s < 2; s++) {
#pragma HLS UNROLL
        priority_queue_level_A[s].insert_wrapper<QUERY_NUM, NLIST / 2>(
            s_distance_cell_ID_level_A[s], 
            s_result_level_A[s]); 
    }

    hls::stream<dist_cell_ID_t> s_distance_cell_ID_level_B;
#pragma HLS stream variable=s_distance_cell_ID_level_B depth=512

    merge_distance_cell_ID_level_A<QUERY_NUM>(
        s_result_level_A,
        s_distance_cell_ID_level_B);

    // the depth of this priority queue is nprobe
    Priority_queue<dist_cell_ID_t, NPROBE, Collect_smallest> priority_queue_level_B;

    priority_queue_level_B.insert_wrapper<QUERY_NUM, NPROBE * 2>(
        s_distance_cell_ID_level_B, 
        s_selected_distance_cell_ID); 

}