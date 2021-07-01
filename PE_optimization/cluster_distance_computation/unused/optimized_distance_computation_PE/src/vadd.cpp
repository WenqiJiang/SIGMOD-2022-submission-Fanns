// This program:
//   1. stora all the cell centroids coordinates on-chip
//   2. given an input query, compute distance with those centroids
//   3. select the top nprobe centroids using sorting network / heap
//   4. access the cell codebooks stored in HBM, construct distance table

#include "vadd.hpp"

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
    const t_axi* table_DDR0, const t_axi* table_DDR1,
    float out_PLRAM[QUERY_NUM * CENTROIDS_PER_PARTITION]
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

    hls::stream<float> s_query_vectors;
#pragma HLS stream variable=s_query_vectors depth=128

    hls::stream<float> s_centroid_vectors;
#pragma HLS stream variable=s_centroid_vectors depth=8

    hls::stream<dist_cell_ID_t> s_partial_cell_distance;
#pragma HLS stream variable=s_partial_cell_distance depth=4

    broadcast_init_centroid_vectors(table_DDR1, s_centroid_vectors);
    broadcast_query_vector<QUERY_NUM>(table_DDR0, s_query_vectors);

    compute_cell_distance<QUERY_NUM>(0, s_centroid_vectors, s_query_vectors, s_partial_cell_distance);

    write_result<QUERY_NUM * CENTROIDS_PER_PARTITION>(s_partial_cell_distance, out_PLRAM);
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

    for (int i = 0; i < CENTROIDS_PER_PARTITION * D; i++) {
        s_centroid_vectors.write(table_DDR1[i]);
    }
}


template<const int query_num>
void compute_cell_distance(
    int start_cell_ID,
    hls::stream<float>& s_centroid_vectors,
    hls::stream<float>& s_query_vectors,
    hls::stream<dist_cell_ID_t>& s_partial_cell_distance) {

    // what is the speed needed to compute these distances? need to match HBM
    // python performance_estimation_three_nodes_IVF.py --nlist=8192 --nprobe=32
    // Estimated HBM Latency: 23.58 us -> 3K cycles
    // each URAM -> 64bit port x 2 -> 4 elements per cycle -> 64 cycles per vec
    //   -> at most 50 vectors per URAM (actually 1 URAM can only fit 32)
    // My strategy: 32 Partitions, each responsible for 256 vectors, 
    //   each vector finish in 10 cycles
    float cell_centroids_partition[CENTROIDS_PER_PARTITION * D];
#pragma HLS array_partition variable=cell_centroids_partition block factor=4
#pragma HLS resource variable=cell_centroids_partition core=RAM_2P_URAM

    // Suppose the goal is 20K QPS, 140 MHz
    //  without unroll the mulAdd operations, 
    //    the time to finish one query = CENTROIDS_PER_PARTITION * D = NLIST / PE_NUM_CENTER_DIST_COMP * D
    //    e.g., 32 PE, QPS = 1 / (8192 / 32 * 128 * 1/140/1e6) = 4272
    //    requires to compute 8 rows in parallel  4272 * 8 = 34179 QPS
#define TILE_SIZE_COMPUTE_CELL_DISTANCE 8 
    float distance_buffer[TILE_SIZE_COMPUTE_CELL_DISTANCE];
#pragma HLS array_partition variable=distance_buffer complete

    float local_query_buffer[TILE_SIZE_COMPUTE_CELL_DISTANCE][D];
#pragma HLS array_partition variable=local_query_buffer dim=1 factor=4
#pragma HLS resource variable=local_query_buffer core=RAM_2P_BRAM

    // init before any computation
    for (int c = 0; c < CENTROIDS_PER_PARTITION; c++) {
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            cell_centroids_partition[c * D + d] = s_centroid_vectors.read();
        }
    }

    for (int query_id = 0; query_id < query_num; query_id++) {

        // load query vec
        for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
            float reg = s_query_vectors.read();
            for (int s = 0; s < TILE_SIZE_COMPUTE_CELL_DISTANCE; s++) {
#pragma HLS UNROLL
                local_query_buffer[s][d] = reg;
            }
        }

        // compute distance and write results out
        for (int c = 0; c < CENTROIDS_PER_PARTITION / TILE_SIZE_COMPUTE_CELL_DISTANCE; c++) {

            // compute 8 rows in parallel
            for (int s = 0; s < TILE_SIZE_COMPUTE_CELL_DISTANCE; s++) {
#pragma HLS UNROLL
                for (int d = 0; d < D; d++) {
#pragma HLS pipeline II=1
                    // loaded row number = CENTROIDS_PER_PARTITION / TILE_SIZE_COMPUTE_CELL_DISTANCE * s + c
                    float scalar_dist = 
                        local_query_buffer[s][d] * 
                        cell_centroids_partition[
                            (CENTROIDS_PER_PARTITION / TILE_SIZE_COMPUTE_CELL_DISTANCE * s + c) * D + d];
                    float square_dist = scalar_dist * scalar_dist;
                    distance_buffer[s] += square_dist;
                }
            }

            // write 8 values out
            for (int s = 0; s < TILE_SIZE_COMPUTE_CELL_DISTANCE; s++) {
#pragma HLS pipeline II=1
                dist_cell_ID_t out;
                out.dist = distance_buffer[s];
                out.cell_ID = start_cell_ID + CENTROIDS_PER_PARTITION / TILE_SIZE_COMPUTE_CELL_DISTANCE * s + c;
                s_partial_cell_distance.write(out);
            }


        }
    }
}

template<const int total_len>
void write_result(
    hls::stream<dist_cell_ID_t>& s_result, float* results_out) {
    
    for (int i = 0; i < total_len; i++) {
        dist_cell_ID_t reg = s_result.read();
        results_out[i] = reg.dist;
    }
}

