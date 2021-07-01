// This program:
//   1. stora all the cell centroids coordinates on-chip
//   2. given an input query, compute distance with those centroids
//   3. select the top nprobe centroids using sorting network / heap
//   4. access the cell codebooks stored in HBM, construct distance table

#include "vadd.hpp"

#define LOOP_COUNT 1000000
#define BUFFER_LEN 128


void read_input(
    const float* table_HBM0, 
    hls::stream<float> &s_in_A,
    hls::stream<float> &s_in_B,
    hls::stream<float> &s_in_C) {

    float buffer[BUFFER_LEN];
   
    for (int i = 0; i < BUFFER_LEN; i++) {
#pragma HLS pipeline II=1
        buffer[i] = table_HBM0[i];
    }  
    
    for (int c = 0; c < LOOP_COUNT; c++) {
        for (int i = 0; i < BUFFER_LEN; i++) {
#pragma HLS pipeline II=1
            float reg = buffer[i];
            s_in_A.write(reg);
            s_in_B.write(reg);
            s_in_C.write(reg);
        }
    }
}

void write_output(
    float* out_HBM, 
    hls::stream<float> &s_out_A,
    hls::stream<float> &s_out_B) {

    float buffer_A[BUFFER_LEN];
    float buffer_B[BUFFER_LEN];
   
    for (int c = 0; c < LOOP_COUNT; c++) {
        for (int i = 0; i < BUFFER_LEN; i++) {
#pragma HLS pipeline II=1
            buffer_A[i] = s_out_A.read();
            buffer_B[i] = s_out_B.read();
        }
    } 

    for (int i = 0; i < BUFFER_LEN; i++) {
#pragma HLS pipeline II=1
        out_HBM[i] = buffer_A[i] + buffer_B[i];
    }  
}

void dataflow_read_input(
    hls::stream<float> &s_in_A,
    hls::stream<float> &s_in_B,
    hls::stream<float> &s_in_C,
    float buffer[BUFFER_LEN],
    hls::stream<float> &s_out_A,
    hls::stream<float> &s_in_B_forward,
    hls::stream<float> &s_in_C_forward) {


// TODO: what if forward content (B,C) and A cannot be presented in the same loop? 
//   need extra control logic then
    for (int i = 0; i < BUFFER_LEN; i++) {
#pragma HLS pipeline II=1

        // double buffer
        float reg = s_in_A.read();
        buffer[i] = reg;

        // output of this function
        s_out_A.write(reg);

        // forward input to the next dataflow function
        s_in_B_forward.write(s_in_B.read());
        s_in_C_forward.write(s_in_C.read());
    } 
}

void dataflow_write_output(
    hls::stream<float> &s_in_B_forward,
    hls::stream<float> &s_in_C_forward,
    float buffer[BUFFER_LEN],
    hls::stream<float> &s_out_B) {

    for (int i = 0; i < BUFFER_LEN; i++) {
#pragma HLS pipeline II=1
        float result = s_in_B_forward.read() + s_in_C_forward.read() + buffer[i];
        s_out_B.write(result);
    } 
}

void double_buffer_loop(
    hls::stream<float> &s_in_A, 
    hls::stream<float> &s_in_B, 
    hls::stream<float> &s_in_C, 
    hls::stream<float> &s_out_A,
    hls::stream<float> &s_out_B) { 

    // each function ~128 CC
    // 128 * 1000000 * (1/140/1e6) = 0.914 sec
    for (int count = 0; count < LOOP_COUNT; count++) {
#pragma HLS dataflow

        float buffer[BUFFER_LEN];
#pragma HLS resource variable=buffer core=RAM_1P_BRAM

        hls::stream<float> s_in_B_forward;
#pragma HLS stream variable=s_in_B_forward depth=2
        hls::stream<float> s_in_C_forward;
#pragma HLS stream variable=s_in_C_forward depth=2

        dataflow_read_input(
            s_in_A, 
            s_in_B,
            s_in_C,
            buffer, 
            s_out_A,
            s_in_B_forward,
            s_in_C_forward);

        dataflow_write_output(
            s_in_B_forward,
            s_in_C_forward,
            buffer, 
            s_out_B);
    }    
}

void vadd(  
    const float* table_HBM0, const float* table_HBM1, 
    // const t_axi* table_HBM2, const t_axi* table_HBM3, 
    // const t_axi* table_HBM4, const t_axi* table_HBM5, 
    // const t_axi* table_HBM6, const t_axi* table_HBM7, 
    // const t_axi* table_HBM8, const t_axi* table_HBM9, 
    // const t_axi* table_HBM10, const t_axi* table_HBM11, 
    // const t_axi* table_HBM12, const t_axi* table_HBM13, 
    // const t_axi* table_HBM14, const t_axi* table_HBM15, 
    // const t_axi* table_HBM16, const t_axi* table_HBM17, 
    // const t_axi* table_HBM18, const t_axi* table_HBM19, 
    // const t_axi* table_HBM20, const t_axi* table_HBM21, 
    // const t_axi* table_HBM22, const t_axi* table_HBM23, 
    // const t_axi* table_HBM24, const t_axi* table_HBM25, 
    // const t_axi* table_HBM26, const t_axi* table_HBM27, 
    float out_HBM[QUERY_NUM * NLIST]
    // const t_axi* table_HBM28, const t_axi* table_HBM29, 
    // const t_axi* table_HBM30, const t_axi* table_HBM31, 
    // const t_axi* table_DDR0, const t_axi* table_DDR1,
    )
{
#pragma HLS INTERFACE m_axi port=table_HBM0  offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=table_HBM1  offset=slave bundle=gmem1
// #pragma HLS INTERFACE m_axi port=table_HBM2  offset=slave bundle=gmem2
// #pragma HLS INTERFACE m_axi port=table_HBM3  offset=slave bundle=gmem3
// #pragma HLS INTERFACE m_axi port=table_HBM4  offset=slave bundle=gmem4
// #pragma HLS INTERFACE m_axi port=table_HBM5  offset=slave bundle=gmem5
// #pragma HLS INTERFACE m_axi port=table_HBM6  offset=slave bundle=gmem6
// #pragma HLS INTERFACE m_axi port=table_HBM7  offset=slave bundle=gmem7
// #pragma HLS INTERFACE m_axi port=table_HBM8  offset=slave bundle=gmem8
// #pragma HLS INTERFACE m_axi port=table_HBM9  offset=slave bundle=gmem9
// #pragma HLS INTERFACE m_axi port=table_HBM10  offset=slave bundle=gmem10
// #pragma HLS INTERFACE m_axi port=table_HBM11  offset=slave bundle=gmem11
// #pragma HLS INTERFACE m_axi port=table_HBM12  offset=slave bundle=gmem12
// #pragma HLS INTERFACE m_axi port=table_HBM13  offset=slave bundle=gmem13
// #pragma HLS INTERFACE m_axi port=table_HBM14  offset=slave bundle=gmem14
// #pragma HLS INTERFACE m_axi port=table_HBM15  offset=slave bundle=gmem15
// #pragma HLS INTERFACE m_axi port=table_HBM16  offset=slave bundle=gmem16
// #pragma HLS INTERFACE m_axi port=table_HBM17  offset=slave bundle=gmem17
// #pragma HLS INTERFACE m_axi port=table_HBM18  offset=slave bundle=gmem18
// #pragma HLS INTERFACE m_axi port=table_HBM19  offset=slave bundle=gmem19
// #pragma HLS INTERFACE m_axi port=table_HBM20  offset=slave bundle=gmem20
// #pragma HLS INTERFACE m_axi port=table_HBM21  offset=slave bundle=gmem21
// #pragma HLS INTERFACE m_axi port=table_HBM22  offset=slave bundle=gmem22
// #pragma HLS INTERFACE m_axi port=table_HBM23  offset=slave bundle=gmem23
// #pragma HLS INTERFACE m_axi port=table_HBM24  offset=slave bundle=gmem24
// #pragma HLS INTERFACE m_axi port=table_HBM25  offset=slave bundle=gmem25
// #pragma HLS INTERFACE m_axi port=table_HBM26  offset=slave bundle=gmem26
// #pragma HLS INTERFACE m_axi port=table_HBM27  offset=slave bundle=gmem27
// #pragma HLS INTERFACE m_axi port=table_HBM28  offset=slave bundle=gmem28
// #pragma HLS INTERFACE m_axi port=table_HBM29  offset=slave bundle=gmem29
// #pragma HLS INTERFACE m_axi port=table_HBM30  offset=slave bundle=gmem30
// #pragma HLS INTERFACE m_axi port=table_HBM31  offset=slave bundle=gmem31

// #pragma HLS INTERFACE m_axi port=table_DDR0  offset=slave bundle=gmem32
// #pragma HLS INTERFACE m_axi port=table_DDR1  offset=slave bundle=gmem33

// PLRAM
#pragma HLS INTERFACE m_axi port=out_HBM offset=slave bundle=gmem34

#pragma HLS INTERFACE s_axilite port=table_HBM0  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM1  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM2  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM3  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM4  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM5  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM6  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM7  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM8  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM9  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM10  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM11  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM12  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM13  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM14  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM15  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM16  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM17  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM18  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM19  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM20  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM21  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM22  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM23  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM24  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM25  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM26  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM27  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM28  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM29  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM30  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_HBM31  bundle=control

// #pragma HLS INTERFACE s_axilite port=table_DDR0  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_DDR1  bundle=control

#pragma HLS INTERFACE s_axilite port=out_HBM bundle=control

#pragma HLS INTERFACE s_axilite port=return bundle=control


#pragma HLS dataflow

    hls::stream<float> s_in_A;
#pragma HLS stream variable=s_in_A depth=1024
    hls::stream<float> s_in_B;
#pragma HLS stream variable=s_in_B depth=1024
    hls::stream<float> s_in_C;
#pragma HLS stream variable=s_in_C depth=1024

    hls::stream<float> s_out_A;
#pragma HLS stream variable=s_out_A depth=1024
    hls::stream<float> s_out_B;
#pragma HLS stream variable=s_out_B depth=1024

    read_input(table_HBM0, s_in_A, s_in_B, s_in_C);

    double_buffer_loop(s_in_A, s_in_B, s_in_C, s_out_A, s_out_B);

    write_output(out_HBM, s_out_A, s_out_B);
}
