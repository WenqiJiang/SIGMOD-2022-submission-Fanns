#include <hls_stream.h>

#include "constants.hpp"
#include "types.hpp"

#include "LUT_construction.hpp"

extern "C" {

void vadd(  
    t_axi* table_HBM0, t_axi* table_HBM1, 
    t_axi* table_HBM2, t_axi* table_HBM3 , 
    /* t_axi* table_HBM4, t_axi* table_HBM5, 
    t_axi* table_HBM6, t_axi* table_HBM7, 
    t_axi* table_HBM8, t_axi* table_HBM9, 
    t_axi* table_HBM10, t_axi* table_HBM11, 
    t_axi* table_HBM12, t_axi* table_HBM13, 
    t_axi* table_HBM14, t_axi* table_HBM15, 
    t_axi* table_HBM16, t_axi* table_HBM17, 
    t_axi* table_HBM18, t_axi* table_HBM19, 
    t_axi* table_HBM20, t_axi* table_HBM21, 
    t_axi* table_HBM22, t_axi* table_HBM23, 
    t_axi* table_HBM24, t_axi* table_HBM25, 
    t_axi* table_HBM26, t_axi* table_HBM27, 
    t_axi* table_HBM28, t_axi* table_HBM29, 
    t_axi* table_HBM30, t_axi* table_HBM31, 
    t_axi* table_DDR0, t_axi* table_DDR1, */
    result_t out_PLRAM[K] 
    );
}


template<const int query_num>
void load_query_vectors(
    const float* DRAM_query_vector,
    hls::stream<float>& s_query_vectors);

template<const int query_num>
void load_center_vectors(
    const float* DRAM_center_vector,
    hls::stream<float>& s_center_vectors);

void load_PQ_quantizer(
    const float* DRAM_PQ_quantizer,
    hls::stream<float> &s_PQ_quantizer_init);

template<const int total_len>
void consume_and_write(
    hls::stream<distance_LUT_PQ16_t>& s_result, result_t* results_out);
