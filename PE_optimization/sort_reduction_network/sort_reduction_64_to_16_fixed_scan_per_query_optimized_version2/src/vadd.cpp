#include "vadd.hpp"
#include "sort_reduction_64_to_16_with_vecID.hpp"

#include <stdio.h>

/////     Given 128 input streams (unordered), output the 16 smallest numbers /////

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

    hls::stream<single_PQ_result> s_input[4][16];
#pragma HLS array_partition variable=s_input complete
#pragma HLS stream variable=s_input depth=8
#pragma HLS RESOURCE variable=s_input core=FIFO_SRL

    hls::stream<single_PQ_result> s_output[16];
#pragma HLS array_partition variable=s_output complete
#pragma HLS stream variable=s_output depth=8
#pragma HLS RESOURCE variable=s_output core=FIFO_SRL

    dummy_input_sender<QUERY_NUM, ITERATION_PER_QUERY>(
        s_input, table_DDR0);

////////////////////     Core Function Starts     ////////////////////

    Sort_reduction<single_PQ_result, 64, 16, Collect_smallest> sort_reduction_module;

    sort_reduction_module.sort_and_reduction<QUERY_NUM, ITERATION_PER_QUERY>(
        s_input, 
        s_output);

////////////////////     Core Function Ends     ////////////////////

    write_result<QUERY_NUM, ITERATION_PER_QUERY>(
        s_output,
        out_PLRAM);
}

////////////////////     Helper Function Starts     ////////////////////

template<const int query_num, const int iteration_per_query>
void dummy_input_sender(
    hls::stream<single_PQ_result> (&s_input)[4][16],
    const float* array_DDR) {

    single_PQ_result input_array[4][16];
#pragma HLS array_partition variable=input_array complete

    for (int s1 = 0; s1 < 4; s1++) {
        for (int s2 = 0; s2 < 16; s2++) {
            input_array[s1][s2].vec_ID = s1 * 16 + s2;
            input_array[s1][s2].dist = array_DDR[s1 * 16 + s2];
        }
    }

    for (int query_id = 0; query_id < query_num; query_id++) {
        
        for (int iter = 0; iter < iteration_per_query; iter++) {
#pragma HLS pipeline II=1            
            for (int s1 = 0; s1 < 4; s1++) {
#pragma HLS UNROLL
                for (int s2 = 0; s2 < 16; s2++) {
#pragma HLS UNROLL
                    s_input[s1][s2].write(input_array[s1][s2]);
                }
            }
        }
    }
}


template<const int query_num, const int iteration_per_query>
void write_result(
    hls::stream<single_PQ_result> (&s_output)[16],
    ap_uint<64>* output) {

    single_PQ_result output_array[16];
#pragma HLS array_partition variable=input_array complete

    for (int query_id = 0; query_id < query_num; query_id++) {
        
        for (int iter = 0; iter < iteration_per_query; iter++) {            
#pragma HLS pipeline II=1
            for (int s = 0; s < 16; s++) {
#pragma HLS UNROLL
                output_array[s] = s_output[s].read();
            }
        }
    }

    // Write the last set of results back
    for (int i = 0; i < 16; i++) {
        float dist_local = output_array[i].dist;
        int cell_ID_local = output_array[i].vec_ID;
        output[i].range(31, 0) = *((ap_uint<32>*) &dist_local);
        output[i].range(63, 32) = *((ap_uint<32>*) &cell_ID_local);
    }
}

////////////////////     Helper Function Ends     ////////////////////