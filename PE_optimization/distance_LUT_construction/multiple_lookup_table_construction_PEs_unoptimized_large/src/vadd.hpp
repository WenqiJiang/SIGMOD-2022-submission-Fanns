#include <hls_stream.h>

#include "constants.hpp"
#include "types.hpp"

#include "LUT_construction.hpp"

extern "C" {

void vadd(  
    float* table_HBM0, float* table_HBM1, 
    float* table_HBM2, float* table_HBM3 , 
    /* float* table_HBM4, float* table_HBM5, 
    float* table_HBM6, float* table_HBM7, 
    float* table_HBM8, float* table_HBM9, 
    float* table_HBM10, float* table_HBM11, 
    float* table_HBM12, float* table_HBM13, 
    float* table_HBM14, float* table_HBM15, 
    float* table_HBM16, float* table_HBM17, 
    float* table_HBM18, float* table_HBM19, 
    float* table_HBM20, float* table_HBM21, 
    float* table_HBM22, float* table_HBM23, 
    float* table_HBM24, float* table_HBM25, 
    float* table_HBM26, float* table_HBM27, 
    float* table_HBM28, float* table_HBM29, 
    float* table_HBM30, float* table_HBM31, 
    float* table_DDR0, float* table_DDR1, */
    ap_uint512_t out_PLRAM[K] 
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
    hls::stream<float> (&s_PQ_quantizer_init)[PE_NUM_TABLE_CONSTRUCTION]);

void consume_and_write(
    hls::stream<distance_LUT_PQ16_t>& s_result, ap_uint512_t* results_out);
