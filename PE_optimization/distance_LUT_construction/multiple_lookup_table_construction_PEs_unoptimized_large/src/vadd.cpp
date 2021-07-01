// This program:
//   1. stora all the cell centroids coordinates on-chip
//   2. given an input query, compute distance with those centroids
//   3. select the top nprobe centroids using sorting network / heap
//   4. access the cell codebooks stored in HBM, construct distance table

#include "vadd.hpp"

#include "stdio.h"

// HBM 3 is used for output
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
    )
{
#pragma HLS INTERFACE m_axi port=table_HBM0  offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=table_HBM1  offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=table_HBM2  offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=table_HBM3  offset=slave bundle=gmem3
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
#pragma HLS INTERFACE m_axi port=out_PLRAM offset=slave bundle=gmem34

#pragma HLS INTERFACE s_axilite port=table_HBM0  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM1  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM2  bundle=control
#pragma HLS INTERFACE s_axilite port=table_HBM3  bundle=control
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

#pragma HLS INTERFACE s_axilite port=out_PLRAM bundle=control

#pragma HLS INTERFACE s_axilite port=return bundle=control
    
#pragma HLS dataflow

    // Storage: 
    //   HBM0 -> query vector (1 vector per NPROBE=32 center vector)
    //   HBM1 -> center vector
    //   HBM2 -> PQ codebook
    
    hls::stream<float> s_query_vectors;
#pragma HLS stream variable=s_query_vectors depth=512

    hls::stream<float> s_center_vectors;
#pragma HLS stream variable=s_center_vectors depth=512

    hls::stream<float> s_PQ_quantizer_init[PE_NUM_TABLE_CONSTRUCTION];
#pragma HLS stream variable=s_PQ_quantizer_init depth=512
#pragma HLS array_partition variable=s_PQ_quantizer_init complete

    hls::stream<distance_LUT_PQ16_t> s_lookup_table;
#pragma HLS stream variable=s_lookup_table depth=512
// #pragma HLS resource variable=s_lookup_table core=FIFO_SRL

    load_query_vectors<QUERY_NUM>(
        table_HBM0, 
        s_query_vectors);

    load_center_vectors<QUERY_NUM>(
        table_HBM1, 
        s_center_vectors);

    load_PQ_quantizer(
        table_HBM2,
        s_PQ_quantizer_init);

    lookup_table_construction_wrapper<QUERY_NUM>(
        s_PQ_quantizer_init,
        s_center_vectors,
        s_query_vectors,
        s_lookup_table);

    consume_and_write(s_lookup_table, out_PLRAM);
}


template<const int query_num>
void load_query_vectors(
    const float* DRAM_query_vector,
    hls::stream<float>& s_query_vectors) {

    // Data type: suppose each vector = 128 D, FPGA freq = 200 MHz
    //   then it takes 640 + 200 ns < 1 us to load a query vector, 
    //   much faster than computing distance and constructing LUT (> 10 us)

    // HBM0 -> query vector (1 vector per NPROBE=32 center vector)
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int d = 0; d < D; d++) {
        #pragma HLS pipeline II=1
            s_query_vectors.write(DRAM_query_vector[query_id * D + d]);
        }
    }
}

template<const int query_num>
void load_center_vectors(
    const float* DRAM_center_vector,
    hls::stream<float>& s_center_vectors) {

    //   HBM1 -> center vector
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int nprobe_id = 0; nprobe_id < NPROBE; nprobe_id++) {

            int start_addr = (query_id * NPROBE + nprobe_id) * D;

            for (int i = 0; i < D; i++) {
                s_center_vectors.write(DRAM_center_vector[start_addr + i]);
            }
        }
    }
}

void load_PQ_quantizer(
    const float* DRAM_PQ_quantizer,
    hls::stream<float> (&s_PQ_quantizer_init)[PE_NUM_TABLE_CONSTRUCTION]) {

    // load PQ quantizer centers from HBM
    for (int i = 0; i < K * D; i++) {
#pragma HLS pipeline II=1
        for (int s = 0; s < PE_NUM_TABLE_CONSTRUCTION; s++) {
#pragma HLS UNROLL
            s_PQ_quantizer_init[s].write(DRAM_PQ_quantizer[i]);
        }
    }
}

void consume_and_write(
    hls::stream<distance_LUT_PQ16_t>& s_result, ap_uint512_t* results_out) {
    
    distance_LUT_PQ16_t result_local[K];
    for (int query_id = 0; query_id < QUERY_NUM; query_id++) {
        for (int nprobe_id = 0; nprobe_id < NPROBE; nprobe_id++) {
            for (int i = 0; i < K; i++) {
                printf("consume result: query_id = %d, nprobe_id = %d, row = %d\n", query_id, nprobe_id, i);
                result_local[i] = s_result.read();
            }
        }
    }

    for (int i = 0; i < K; i++) {
        float tmp[16];
#pragma HLS array_partition variable=tmp complete
        tmp[0] = result_local[i].dist_0;
        tmp[1] = result_local[i].dist_1;
        tmp[2] = result_local[i].dist_2;
        tmp[3] = result_local[i].dist_3;
        tmp[4] = result_local[i].dist_4;
        tmp[5] = result_local[i].dist_5;
        tmp[6] = result_local[i].dist_6;
        tmp[7] = result_local[i].dist_7;
        tmp[8] = result_local[i].dist_8;
        tmp[9] = result_local[i].dist_9;
        tmp[10] = result_local[i].dist_10;
        tmp[11] = result_local[i].dist_11;
        tmp[12] = result_local[i].dist_12;
        tmp[13] = result_local[i].dist_13;
        tmp[14] = result_local[i].dist_14;
        tmp[15] = result_local[i].dist_15;

        ap_uint512_t out;
        for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
            float reg = tmp[i];
            out.range(31 + i * 32, i * 32) = *((ap_uint<32>*) (&reg));
        }
        
        results_out[i] = out;
    }
}

