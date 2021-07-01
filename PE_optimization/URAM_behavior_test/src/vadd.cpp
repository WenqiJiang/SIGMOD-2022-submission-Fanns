// This program:
//   1. stora all the cell centroids coordinates on-chip
//   2. given an input query, compute distance with those centroids
//   3. select the top nprobe centroids using sorting network / heap
//   4. access the cell codebooks stored in HBM, construct distance table

#include "vadd.hpp"

template<const int uram_depth>
void uram_ap_uint_64(
    const float* table_HBM0,
    float* out_HBM) {
#pragma HLS inline off    

    ap_uint<64> uram_array[uram_depth];
#pragma HLS resource variable=uram_array core=RAM_2P_URAM

    for (int i = 0; i < uram_depth; i++) {
        float tmp_32_A_float = table_HBM0[2 * i];
        float tmp_32_B_float = table_HBM0[2 * i + 1];
        ap_uint<32> tmp_32_A_ap = *((ap_uint<32>*) (&tmp_32_A_float));
        ap_uint<32> tmp_32_B_ap = *((ap_uint<32>*) (&tmp_32_B_float));
        uram_array[i].range(31, 0) = tmp_32_A_ap;
        uram_array[i].range(63, 32) = tmp_32_B_ap; 
    }

    ap_uint<64> write_val_A = uram_array[uram_depth - 1];
    ap_uint<64> write_val_B = uram_array[uram_depth - 1];
    write_loop:
    for (int i = 0; i < uram_depth - 1; i++) {
#pragma HLS pipeline II=1
        uram_array[i] = write_val_A;
        uram_array[i + 1] = write_val_B;
    }

    ap_uint<64> read_val_A;
    ap_uint<64> read_val_B;
    read_loop:
    for (int i = 0; i < uram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = uram_array[i];
        read_val_B = uram_array[uram_depth - 1 - i];
    }

    for (int i = 0; i < uram_depth; i++) {
        ap_uint<64> tmp_64 = read_val_A + read_val_B;
        ap_uint<32> tmp_32 = tmp_64.range(31, 0);
        out_HBM[i] = *((float*) (&tmp_32));
    }
}


template<const int uram_depth>
void uram_float(
    const float* table_HBM0,
    float* out_HBM) {
#pragma HLS inline off    

    float uram_array[uram_depth];
#pragma HLS resource variable=uram_array core=RAM_2P_URAM

    for (int i = 0; i < uram_depth; i++) {
        uram_array[i] = table_HBM0[i];
    }

    float write_val_A = uram_array[uram_depth - 1];
    float write_val_B = uram_array[uram_depth - 1];
    write_loop:
    for (int i = 0; i < uram_depth - 1; i++) {
#pragma HLS pipeline II=1
        uram_array[i] = write_val_A;
        uram_array[i + 1] = write_val_B;
    }

    float read_val_A;
    float read_val_B;
    read_loop:
    for (int i = 0; i < uram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = uram_array[i];
        read_val_B = uram_array[uram_depth - 1 - i];
    }

    for (int i = 0; i < uram_depth; i++) {
        out_HBM[i] = read_val_A + read_val_B;
    }
}

template<const int uram_depth>
void uram_ap_two_floats_t(
    const float* table_HBM0,
    float* out_HBM) {
#pragma HLS inline off    

    two_floats_t uram_array[uram_depth];
#pragma HLS resource variable=uram_array core=RAM_2P_URAM

    for (int i = 0; i < uram_depth; i++) {
        uram_array[i].f_0 = table_HBM0[2 * i];
        uram_array[i].f_1 = table_HBM0[2 * i + 1];
    }

    two_floats_t write_val_A = uram_array[uram_depth - 1];
    two_floats_t write_val_B = uram_array[uram_depth - 1];
    write_loop:
    for (int i = 0; i < uram_depth - 1; i++) {
#pragma HLS pipeline II=1
        uram_array[i] = write_val_A;
        uram_array[i + 1] = write_val_B;
    }

    two_floats_t read_val_A;
    two_floats_t read_val_B;
    read_loop:
    for (int i = 0; i < uram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = uram_array[i];
        read_val_B = uram_array[uram_depth - 1 - i];
    }

    for (int i = 0; i < uram_depth; i++) {
        float tmp = read_val_A.f_0 + read_val_B.f_1;
        out_HBM[i] = tmp;
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

    ////// ap_uint<64> //////
    // uram_ap_uint_64<16>(table_HBM0, out_HBM);

    // uram_ap_uint_64<4096>(table_HBM0, out_HBM);

    // uram_ap_uint_64<4097>(table_HBM0, out_HBM);

    // uram_ap_uint_64<8192>(table_HBM0, out_HBM);

    ////// float //////
    // uram_float<16>(table_HBM0, out_HBM);

    // uram_float<4096>(table_HBM0, out_HBM);

    // uram_float<4097>(table_HBM0, out_HBM);

    // uram_float<8192>(table_HBM0, out_HBM);


    ////// two_float_t //////
    // uram_two_float_t<16>(table_HBM0, out_HBM);

    // uram_two_float_t<4096>(table_HBM0, out_HBM);

    // uram_two_float_t<4097>(table_HBM0, out_HBM);

    uram_two_float_t<8192>(table_HBM0, out_HBM);
}
