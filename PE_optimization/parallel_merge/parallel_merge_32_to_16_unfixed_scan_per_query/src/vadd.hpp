#include <hls_stream.h>

#include "constants.hpp"

extern "C" {

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
    const float* table_DDR0, const t_axi* table_DDR1,
    ap_uint<64>* out_PLRAM
    );
}

template<const int query_num>
void control_signal_sender(
    hls::stream<int> (&s_control_iter_num_per_query)[3]);

template<const int query_num>
void dummy_input_sender(
    hls::stream<int>& s_control_iter_num_per_query,
    hls::stream<single_PQ_result> (&s_input_A)[16],
    hls::stream<single_PQ_result> (&s_input_B)[16],
    const float* array_DDR);

template<const int query_num>
void write_result(
    hls::stream<int>& s_control_iter_num_per_query,
    hls::stream<single_PQ_result> (&s_output)[16],
    ap_uint<64>* output);