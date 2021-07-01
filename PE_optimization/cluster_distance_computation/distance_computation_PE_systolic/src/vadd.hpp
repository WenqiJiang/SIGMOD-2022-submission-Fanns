#include <hls_stream.h>

#include "constants.hpp"
#include "types.hpp"

extern "C" {

void vadd(  
    const float* table_HBM0, const float* table_HBM1, 
    const float* table_HBM2, const float* table_HBM3, 
    const float* table_HBM4, const float* table_HBM5, 
    const float* table_HBM6, const float* table_HBM7, 
    const float* table_HBM8, const float* table_HBM9, 
    const float* table_HBM10, const float* table_HBM11, 
    const float* table_HBM12, const float* table_HBM13, 
    const float* table_HBM14, const float* table_HBM15, 
    const float* table_HBM16, const float* table_HBM17, 
    const float* table_HBM18, const float* table_HBM19, 
    const float* table_HBM20, const float* table_HBM21, 
    const float* table_HBM22, const float* table_HBM23, 
    const float* table_HBM24, const float* table_HBM25, 
    const float* table_HBM26, const float* table_HBM27, 
    const float* table_HBM28, const float* table_HBM29, 
    const float* table_HBM30, 
    // const float* table_HBM31, 
    // const float* table_DDR0, const float* table_DDR1,
    ap_uint<64> out_HBM[QUERY_NUM * NLIST]
    );
}

template<const int query_num>
void broadcast_query_vector(
    const float* table_DDR0,
    hls::stream<float>& s_query_vectors);

void broadcast_init_centroid_vectors(
    const float* table_DDR1,
    hls::stream<float>& s_centroid_vectors);

template<const int total_len>
void write_result(
    hls::stream<dist_cell_ID_t>& s_result, ap_uint<64>* results_out);
