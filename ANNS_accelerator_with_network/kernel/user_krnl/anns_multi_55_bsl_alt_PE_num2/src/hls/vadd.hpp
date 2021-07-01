#pragma once 

#include <hls_stream.h>

#include "cluster_distance_computation.hpp"
#include "constants.hpp"
#include "distance_estimation_by_LUT.hpp"
#include "HBM_interconnections.hpp"
#include "helpers.hpp"
#include "LUT_construction.hpp"
#include "OPQ_preprocessing.hpp"
#include "priority_queue_distance_results_wrapper.hpp"
#include "priority_queue_vector_quantizer.hpp"
#include "select_Voronoi_cell.hpp"
#include "sort_reduction_with_vecID.hpp"
#include "types.hpp"

// extern "C" {

// void vadd(  
//     const ap_uint512_t* HBM_in0, 
//     const ap_uint512_t* HBM_in1, 
//     const ap_uint512_t* HBM_in2, 
//     const ap_uint512_t* HBM_in3, 
//     const ap_uint512_t* HBM_in4, 
//     const ap_uint512_t* HBM_in5, 
//     const ap_uint512_t* HBM_in6, 
//     const ap_uint512_t* HBM_in7, 
//     const ap_uint512_t* HBM_in8, 
//     // const ap_uint512_t* HBM_in9, 
//     // const ap_uint512_t* HBM_in10, 
//     // const ap_uint512_t* HBM_in11, 
//     // const ap_uint512_t* HBM_in12, 
//     // const ap_uint512_t* HBM_in13, 
//     // const ap_uint512_t* HBM_in14, 
//     // const ap_uint512_t* HBM_in15, 
//     // const ap_uint512_t* HBM_in16, 
//     // const ap_uint512_t* HBM_in17, 
//     // const ap_uint512_t* HBM_in18, 
//     // const ap_uint512_t* HBM_in19, 
//     // const ap_uint512_t* HBM_in20, 
//     // HBM21: assigned for HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid
//     const int* HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid, 
//     // HBM22: query vectors
//     float* HBM_query_vectors,
//     // HBM23: center vector table (Vector_quantizer)
//     float* HBM_vector_quantizer,
//     // HBM24: PQ quantizer
//     float* HBM_product_quantizer,
//     // HBM25: OPQ Matrix
//     float* HBM_OPQ_matrix,
//     // HBM26: output (vector_ID, distance)
//     ap_uint64_t* HBM_out
//     // const ap_uint512_t* HBM_in22, const ap_uint512_t* HBM_in23, 
//     // const ap_uint512_t* HBM_in24, const ap_uint512_t* HBM_in25, 
//     // const ap_uint512_t* HBM_in26, const ap_uint512_t* HBM_in27, 
//     // const ap_uint512_t* HBM_in28, const ap_uint512_t* HBM_in29, 
//     // const ap_uint512_t* HBM_in30, const ap_uint512_t* HBM_in31, 
//     // const ap_uint512_t* table_DDR0, const ap_uint512_t* table_DDR1, 
//     );
// }
