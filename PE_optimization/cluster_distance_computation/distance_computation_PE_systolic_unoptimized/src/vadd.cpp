// This program:
//   1. stora all the cell centroids coordinates on-chip
//   2. given an input query, compute distance with those centroids
//   3. select the top nprobe centroids using sorting network / heap
//   4. access the cell codebooks stored in HBM, construct distance table

#include "vadd.hpp"
#include "cluster_distance_computation.hpp"

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
// #pragma HLS INTERFACE m_axi port=table_HBM31  offset=slave bundle=gmem31

// #pragma HLS INTERFACE m_axi port=table_DDR0  offset=slave bundle=gmem32
// #pragma HLS INTERFACE m_axi port=table_DDR1  offset=slave bundle=gmem33

// PLRAM
#pragma HLS INTERFACE m_axi port=out_HBM offset=slave bundle=gmem34

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
// #pragma HLS INTERFACE s_axilite port=table_HBM31  bundle=control

// #pragma HLS INTERFACE s_axilite port=table_DDR0  bundle=control
// #pragma HLS INTERFACE s_axilite port=table_DDR1  bundle=control

#pragma HLS INTERFACE s_axilite port=out_HBM bundle=control

#pragma HLS INTERFACE s_axilite port=return bundle=control
    
#pragma HLS dataflow

    hls::stream<float> s_query_vectors;
#pragma HLS stream variable=s_query_vectors depth=128

    hls::stream<float> s_centroid_vectors;
#pragma HLS stream variable=s_centroid_vectors depth=8

    hls::stream<dist_cell_ID_t> s_partial_cell_distance;
#pragma HLS stream variable=s_partial_cell_distance depth=4

    broadcast_init_centroid_vectors(table_HBM1, s_centroid_vectors);
    broadcast_query_vector<QUERY_NUM>(table_HBM0, s_query_vectors);

    compute_cell_distance_wrapper<QUERY_NUM>(
        s_centroid_vectors, 
        s_query_vectors, 
        s_partial_cell_distance);

    write_result<QUERY_NUM * NLIST>(s_partial_cell_distance, out_HBM);
}

template<const int query_num>
void broadcast_query_vector(
    const float* table_DDR0,
    hls::stream<float>& s_query_vectors) {

    // Data type: suppose each vector = 128 D, FPGA freq = 200 MHz
    //   then it takes 640 + 200 ns < 1 us to load a query vector, 
    //   much faster than computing distance and constructing LUT (> 10 us)

    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int d = 0; d < D; d++) {
        #pragma HLS pipeline II=1
            s_query_vectors.write(table_DDR0[query_id * D + d]);
        }
    }
}

void broadcast_init_centroid_vectors(
    const float* table_DDR1,
    hls::stream<float>& s_centroid_vectors) {

    for (int i = 0; i < NLIST * D; i++) {
        s_centroid_vectors.write(table_DDR1[i]);
    }
}

template<const int total_len>
void write_result(
    hls::stream<dist_cell_ID_t>& s_result, ap_uint<64>* results_out) {
    
    for (int i = 0; i < total_len; i++) {
        dist_cell_ID_t tmp = s_result.read();
        int out_ID = tmp.cell_ID;
        float out_dist = tmp.dist;
        ap_uint<64> out = 0;
        out.range(31, 0) = *((ap_uint<32>*) (&out_ID));
        out.range(63, 32) = *((ap_uint<32>*) (&out_dist));
        results_out[i] = out;
    }
}

