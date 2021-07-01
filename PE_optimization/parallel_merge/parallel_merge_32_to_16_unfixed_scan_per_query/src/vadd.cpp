#include "vadd.hpp"
#include "parallel_merge.hpp"

#include <stdio.h>

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

    hls::stream<single_PQ_result> s_input_A[16];
#pragma HLS stream variable=s_input_A depth=8
#pragma HLS RESOURCE variable=s_input_A core=FIFO_SRL

    hls::stream<single_PQ_result> s_input_B[16];
#pragma HLS stream variable=s_input_B depth=8
#pragma HLS RESOURCE variable=s_input_B core=FIFO_SRL

    hls::stream<single_PQ_result> s_output[16];
#pragma HLS stream variable=s_output depth=8
#pragma HLS RESOURCE variable=s_output core=FIFO_SRL

    hls::stream<int> s_control_iter_num_per_query[3];
#pragma HLS array_partition variable=s_control_iter_num_per_query complete
#pragma HLS stream variable=s_control_iter_num_per_query depth=8
#pragma HLS RESOURCE variable=s_control_iter_num_per_query core=FIFO_SRL
    

    control_signal_sender<QUERY_NUM>(
        s_control_iter_num_per_query);

    dummy_input_sender<QUERY_NUM>(
        s_control_iter_num_per_query[0],
        s_input_A, 
        s_input_B, 
        table_DDR0);

////////////////////     Core Function Starts     ////////////////////

    parallel_merge_sort_16<QUERY_NUM>(
        s_control_iter_num_per_query[1],
        s_input_A,
        s_input_B,
        s_output);

////////////////////     Core Function Ends     ////////////////////

    write_result<QUERY_NUM>(
        s_control_iter_num_per_query[2],
        s_output,
        out_PLRAM);
}

////////////////////     Helper Function Starts     ////////////////////


template<const int query_num>
void control_signal_sender(
    hls::stream<int> (&s_control_iter_num_per_query)[3]) {

    // Here I hard-coded the iteration per query, in reality this is not true
    //   only for performance modeling
    int iteration_per_query = 10000;
    for (int query_id = 0; query_id < query_num; query_id++) {
        s_control_iter_num_per_query[0].write(iteration_per_query);
        s_control_iter_num_per_query[1].write(iteration_per_query);
        s_control_iter_num_per_query[2].write(iteration_per_query);
    }
}

template<const int query_num>
void dummy_input_sender(
    hls::stream<int>& s_control_iter_num_per_query,
    hls::stream<single_PQ_result> (&s_input_A)[16],
    hls::stream<single_PQ_result> (&s_input_B)[16],
    const float* array_DDR) {

    single_PQ_result input_array_A[16];
#pragma HLS array_partition variable=input_array_A complete
    single_PQ_result input_array_B[16];
#pragma HLS array_partition variable=input_array_B complete

    for (int i = 0; i < 16; i++) {
        input_array_A[i].vec_ID = i;
        input_array_A[i].dist = array_DDR[i];
    }
    for (int i = 16; i < 32; i++) {
        input_array_B[i - 16].vec_ID = i;
        input_array_B[i - 16].dist = array_DDR[i];
    }


    for (int query_id = 0; query_id < query_num; query_id++) {
        
        // Each query scans different number of entries
        int iter_num = s_control_iter_num_per_query.read();

        for (int iter = 0; iter < iter_num; iter++) {
#pragma HLS pipeline II=1            
            for (int s = 0; s < 16; s++) {
#pragma HLS UNROLL
                s_input_A[s].write(input_array_A[s]);
                s_input_B[s].write(input_array_B[s]);
            }
        }
    }
}


template<const int query_num>
void write_result(
    hls::stream<int>& s_control_iter_num_per_query,
    hls::stream<single_PQ_result> (&s_output)[16],
    ap_uint<64>* output) {

    single_PQ_result output_array[16];
#pragma HLS array_partition variable=input_array complete

    for (int query_id = 0; query_id < query_num; query_id++) {
        
        // Each query scans different number of entries
        int iter_num = s_control_iter_num_per_query.read();

        for (int iter = 0; iter < iter_num; iter++) {            
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