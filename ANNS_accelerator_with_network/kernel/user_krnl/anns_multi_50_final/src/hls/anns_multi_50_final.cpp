/*
 * Copyright (c) 2020, Systems Group, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ap_axi_sdata.h"
#include <ap_fixed.h>
#include "ap_int.h" 
#include "../../../../common/include/communication.hpp"
#include "hls_stream.h"
#include "vadd.hpp"

#define SESSION_NUM 16

template<const int query_num>
void network_output_converter(
    hls::stream<single_PQ_result> &s_tuple_results, 
    hls::stream<ap_uint512_t>& s_network_results) {

    // Output format per query:
    //   pkg0 -> vector IDs
    //   pkg1 -> distances

    // Note! Here PRIORITY_QUEUE_LEN is hard-coded as 10
    int send_buffer_vec_ID[PRIORITY_QUEUE_LEN];
    float send_buffer_dist[PRIORITY_QUEUE_LEN];
#pragma HLS array_partition variable=send_buffer_vec_ID complete
#pragma HLS array_partition variable=send_buffer_dist complete

    int processed_query_num = 0;
    int out_per_query_counter = 0;

    do {

        if ((!s_tuple_results.empty()) && (out_per_query_counter < PRIORITY_QUEUE_LEN)) {
            single_PQ_result reg = s_tuple_results.read();
            send_buffer_vec_ID[out_per_query_counter] = reg.vec_ID;
            send_buffer_dist[out_per_query_counter] = reg.dist;
            out_per_query_counter++;
        }

        if (out_per_query_counter == 10) {
            ap_uint<512> pkg_vec_ID = 0;
            for (int i = 0; i < PRIORITY_QUEUE_LEN; i++) {
#pragma HLS UNROLL
                int vec_ID_reg = send_buffer_vec_ID[i];
                ap_uint<32> tmp = *((ap_uint<32>*) (&vec_ID_reg));
                pkg_vec_ID.range((i + 1) * 32 - 1, i * 32) = tmp;
            }

            ap_uint<512> pkg_dist = 0;
            for (int i = 0; i < PRIORITY_QUEUE_LEN; i++) {
#pragma HLS UNROLL
                float dist_reg = send_buffer_dist[i];
                ap_uint<32> tmp = *((ap_uint<32>*) (&dist_reg));
                pkg_dist.range((i + 1) * 32 - 1, i * 32) = tmp;
            }

            s_network_results.write(pkg_vec_ID);
            s_network_results.write(pkg_dist);

            out_per_query_counter = 0;
            processed_query_num++;
        }

    } while (processed_query_num < query_num);

}


void network_query_controller_push(
    ap_uint<64> expectedRxByteCnt,
    hls::stream<ap_uint<512> >& s_data_in,
    hls::stream<ap_uint<16> >& s_sessionID_in,
    hls::stream<ap_uint<16> >& s_nextRxPacketLength_in,
    // output
    hls::stream<float> (&s_query_vectors_per_session)[SESSION_NUM],
    hls::stream<int> &s_session_entry, // which FIFO to pull
    hls::stream<ap_uint<16> >& s_sessionID_out
) {

    ap_uint<16> session_ID_array[SESSION_NUM];
    int remaining_floats_in_FIFO[SESSION_NUM]; // how many floats in each FIFO
    for (int i = 0; i < SESSION_NUM; i++) {
        remaining_floats_in_FIFO[i] = 0;
    }

    int existing_session_count = 0; 

    ap_uint<64> rxByteCnt = 0;

    do{
        // WENQI: The length here is in terms of bytes,
        //    hopefully the network stack has handled it 
        //    such that the bytes is always a multiple of 512-bit (64byte)
        ap_uint<16> byte_length = s_nextRxPacketLength_in.read();
        ap_uint<16> session_ID = s_sessionID_in.read();

        // identify which FIFO to write into
        int session_entry = -1;
        bool session_in_array = false;
        for (int i = 0; i < existing_session_count; i++) {
            if (session_ID == session_ID_array[i]) {
                session_entry = i;
                session_in_array = true;
                break;
            }
        }
        if (!session_in_array) {
            session_entry = existing_session_count;
            session_ID_array[session_entry] = session_ID;
            existing_session_count++;
        }

        // byte counts
        rxByteCnt = rxByteCnt + byte_length;
        int pkt_len = byte_length / 64; // 1 512 bit packet = 64 bytes
        int float_len = pkt_len * 16; // 1 float = 4 bytes

        // should add counter first, send enable signal out, then write 
        //     to prevent deadlock in the case that the puller doesnt know it should pull without the signal
        remaining_floats_in_FIFO[session_entry] += float_len;
        if (remaining_floats_in_FIFO[session_entry] >= 128) {
            s_session_entry.write(session_entry);
            s_sessionID_out.write(session_ID);
            remaining_floats_in_FIFO[session_entry] -= 128;
        }

        // write data to the respective FIFO
        for (int j = 0; j < pkt_len; j++) {
            ap_uint<512> pkt_data = s_data_in.read();
            for (int k = 0; k < 16; k++) {
                ap_uint<32> tmp = pkt_data.range(31 + 32 * k, 32 * k);
                float content = *((float*) (&tmp));
                s_query_vectors_per_session[session_entry].write(content);
            }
        }
    } while(rxByteCnt < expectedRxByteCnt);

}

template<const int query_num>
void network_query_controller_pull(
    // input
    hls::stream<float> (&s_query_vectors_per_session)[SESSION_NUM],
    hls::stream<int> &s_session_entry, 
    // output
    hls::stream<float>& s_query_vectors
) {

    int processed_query_num = 0;
    int ele_per_query_counter = 0;
    int session_entry = 0;
    bool consume_data = false; // whether to read data FIFO
    bool session_lock = false;  // read from one session consecutively


    do {

        if (!s_session_entry.empty() & !session_lock) {
            session_entry = s_session_entry.read();
            consume_data = true;
            session_lock = true;
        }

        // read input & write output
        if (consume_data && (ele_per_query_counter < 128)) {
            if (!s_query_vectors_per_session[session_entry].empty()) {
                s_query_vectors.write(s_query_vectors_per_session[session_entry].read());
                ele_per_query_counter++;
            }
        }

        if (ele_per_query_counter == 128) {
            ele_per_query_counter = 0;
            consume_data = false;
            session_lock = false;
            processed_query_num++;
        }

    } while (processed_query_num < query_num);   
}


extern "C" {

    void anns_multi_50_final(
        // ANNS kernel options
        const ap_uint512_t* HBM_in0, 
        const ap_uint512_t* HBM_in1, 
        const ap_uint512_t* HBM_in2,
        const ap_uint512_t* HBM_in3, 
        const ap_uint512_t* HBM_in4, 
        const ap_uint512_t* HBM_in5, 
        const ap_uint512_t* HBM_in6, 
        const ap_uint512_t* HBM_in7, 
        const ap_uint512_t* HBM_in8, 
        // const ap_uint512_t* HBM_in9, 
        // const ap_uint512_t* HBM_in10,  
        // const ap_uint512_t* HBM_in11,  
        // const ap_uint512_t* HBM_in12,  
        // const ap_uint512_t* HBM_in13,  
        // const ap_uint512_t* HBM_in14,  
        // const ap_uint512_t* HBM_in15,  
        // const ap_uint512_t* HBM_in16,  
        // const ap_uint512_t* HBM_in17,  
        // const ap_uint512_t* HBM_in18,  
        // const ap_uint512_t* HBM_in19,  
        // const ap_uint512_t* HBM_in20,  
        // const ap_uint512_t* HBM_in21,  
        // const ap_uint512_t* HBM_in22,  
        // const ap_uint512_t* HBM_in23,  
        // const ap_uint512_t* HBM_in24,  
        // const ap_uint512_t* HBM_in25,  
        // const ap_uint512_t* HBM_in26,  
        // HBM21: assigned for HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid
        const int* HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid, 
        // HBM22: query vectors
        // float* HBM_query_vectors,
        // HBM23: center vector table (Vector_quantizer)
        float* HBM_vector_quantizer,
        // HBM24: PQ quantizer
        float* HBM_product_quantizer,
        // HBM25: OPQ Matrix
        float* HBM_OPQ_matrix,

        // Internal Stream, arg 25 + 0~3
        hls::stream<pkt512>& s_axis_udp_rx, 
        hls::stream<pkt512>& m_axis_udp_tx, 
        hls::stream<pkt256>& s_axis_udp_rx_meta, 
        hls::stream<pkt256>& m_axis_udp_tx_meta, 
        
        // arg 25 + 4~15
        hls::stream<pkt16>& m_axis_tcp_listen_port, 
        hls::stream<pkt8>& s_axis_tcp_port_status, 
        hls::stream<pkt64>& m_axis_tcp_open_connection, 
        hls::stream<pkt128>& s_axis_tcp_open_status, 
        hls::stream<pkt16>& m_axis_tcp_close_connection, 
        hls::stream<pkt128>& s_axis_tcp_notification, 
        hls::stream<pkt32>& m_axis_tcp_read_pkg, 
        hls::stream<pkt16>& s_axis_tcp_rx_meta, 
        hls::stream<pkt512>& s_axis_tcp_rx_data, 
        hls::stream<pkt32>& m_axis_tcp_tx_meta, 
        hls::stream<pkt512>& m_axis_tcp_tx_data, 
        hls::stream<pkt64>& s_axis_tcp_tx_status,

        // arg 25 + 16~20
        int useConn, 
        int listenPort, 
        int expectedRxByteCnt,
        // the destination arguments are kept but not used
        int destIpAddress,
        int destPort
            ) {

#pragma HLS INTERFACE m_axi port=HBM_in0  offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=HBM_in1  offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=HBM_in2  offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=HBM_in3  offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi port=HBM_in4  offset=slave bundle=gmem4
#pragma HLS INTERFACE m_axi port=HBM_in5  offset=slave bundle=gmem5
#pragma HLS INTERFACE m_axi port=HBM_in6  offset=slave bundle=gmem6
#pragma HLS INTERFACE m_axi port=HBM_in7  offset=slave bundle=gmem7
#pragma HLS INTERFACE m_axi port=HBM_in8  offset=slave bundle=gmem8
// #pragma HLS INTERFACE m_axi port=HBM_in9  offset=slave bundle=gmem9
// #pragma HLS INTERFACE m_axi port=HBM_in10  offset=slave bundle=gmem10
// #pragma HLS INTERFACE m_axi port=HBM_in11  offset=slave bundle=gmem11
// #pragma HLS INTERFACE m_axi port=HBM_in12  offset=slave bundle=gmem12
// #pragma HLS INTERFACE m_axi port=HBM_in13  offset=slave bundle=gmem13
// #pragma HLS INTERFACE m_axi port=HBM_in14  offset=slave bundle=gmem14
// #pragma HLS INTERFACE m_axi port=HBM_in15  offset=slave bundle=gmem15
// #pragma HLS INTERFACE m_axi port=HBM_in16  offset=slave bundle=gmem16
// #pragma HLS INTERFACE m_axi port=HBM_in17  offset=slave bundle=gmem17
// #pragma HLS INTERFACE m_axi port=HBM_in18  offset=slave bundle=gmem18
// #pragma HLS INTERFACE m_axi port=HBM_in19  offset=slave bundle=gmem19
// #pragma HLS INTERFACE m_axi port=HBM_in20  offset=slave bundle=gmem20
// #pragma HLS INTERFACE m_axi port=HBM_in21  offset=slave bundle=gmem21
// #pragma HLS INTERFACE m_axi port=HBM_in22  offset=slave bundle=gmem22
// #pragma HLS INTERFACE m_axi port=HBM_in23  offset=slave bundle=gmem23
// #pragma HLS INTERFACE m_axi port=HBM_in24  offset=slave bundle=gmem24
// #pragma HLS INTERFACE m_axi port=HBM_in25  offset=slave bundle=gmem25
// #pragma HLS INTERFACE m_axi port=HBM_in26  offset=slave bundle=gmem26
#pragma HLS INTERFACE m_axi port=HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid  offset=slave bundle=gmem21
// #pragma HLS INTERFACE m_axi port=HBM_query_vectors  offset=slave bundle=gmem22
#pragma HLS INTERFACE m_axi port=HBM_vector_quantizer  offset=slave bundle=gmem23
#pragma HLS INTERFACE m_axi port=HBM_product_quantizer  offset=slave bundle=gmem24
#pragma HLS INTERFACE m_axi port=HBM_OPQ_matrix  offset=slave bundle=gmem25

#pragma HLS INTERFACE s_axilite port=HBM_in0  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_in1  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_in2  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_in3  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_in4  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_in5  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_in6  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_in7  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_in8  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in9  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in10  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in11  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in12  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in13  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in14  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in15  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in16  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in17  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in18  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in19  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in20  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in21  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in22  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in23  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in24  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in25  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_in26  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM_query_vectors  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_vector_quantizer  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_product_quantizer  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM_OPQ_matrix  bundle=control

#pragma HLS INTERFACE axis port = s_axis_udp_rx
#pragma HLS INTERFACE axis port = m_axis_udp_tx
#pragma HLS INTERFACE axis port = s_axis_udp_rx_meta
#pragma HLS INTERFACE axis port = m_axis_udp_tx_meta
#pragma HLS INTERFACE axis port = m_axis_tcp_listen_port
#pragma HLS INTERFACE axis port = s_axis_tcp_port_status
#pragma HLS INTERFACE axis port = m_axis_tcp_open_connection
#pragma HLS INTERFACE axis port = s_axis_tcp_open_status
#pragma HLS INTERFACE axis port = m_axis_tcp_close_connection
#pragma HLS INTERFACE axis port = s_axis_tcp_notification
#pragma HLS INTERFACE axis port = m_axis_tcp_read_pkg
#pragma HLS INTERFACE axis port = s_axis_tcp_rx_meta
#pragma HLS INTERFACE axis port = s_axis_tcp_rx_data
#pragma HLS INTERFACE axis port = m_axis_tcp_tx_meta
#pragma HLS INTERFACE axis port = m_axis_tcp_tx_data
#pragma HLS INTERFACE axis port = s_axis_tcp_tx_status
#pragma HLS INTERFACE s_axilite port=useConn bundle = control
#pragma HLS INTERFACE s_axilite port=listenPort bundle = control
#pragma HLS INTERFACE s_axilite port=expectedRxByteCnt bundle = control
#pragma HLS INTERFACE s_axilite port=destIpAddress bundle = control
#pragma HLS INTERFACE s_axilite port=destPort bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

// #pragma HLS INTERFACE ap_control_non

#pragma HLS dataflow


        listenPorts (listenPort, useConn, m_axis_tcp_listen_port, 
            s_axis_tcp_port_status);


        hls::stream<ap_uint<512> >    s_data_in;
#pragma HLS STREAM variable=s_data_in depth=512

        hls::stream<ap_uint<16> > s_sessionID_in;
#pragma HLS STREAM variable=s_sessionID_in depth=512

        hls::stream<ap_uint<16> > s_nextRxPacketLength_in;
#pragma HLS STREAM variable=s_nextRxPacketLength_in depth=512

        recvData(
            expectedRxByteCnt, 
            s_axis_tcp_notification, 
            m_axis_tcp_read_pkg, 
            s_axis_tcp_rx_meta, 
            s_axis_tcp_rx_data,
            // output, including the sessionID and length of each packet
            s_data_in,
            s_sessionID_in,
            s_nextRxPacketLength_in);

        hls::stream<float> s_query_vectors;
#pragma HLS STREAM variable=s_query_vectors depth=512

        hls::stream<ap_uint<16> > s_sessionID_out;
#pragma HLS STREAM variable=s_sessionID_out depth=512


        hls::stream<float> s_query_vectors_per_session[SESSION_NUM];
#pragma HLS STREAM variable=s_query_vectors_per_session depth=512
#pragma HLS array_partition variable=s_query_vectors_per_session complete

        hls::stream<int> s_session_entry;
#pragma HLS STREAM variable=s_session_entry depth=512

        network_query_controller_push(
            // input
            QUERY_NUM * 512,
            s_data_in,
            s_sessionID_in,
            s_nextRxPacketLength_in,
            // output
            s_query_vectors_per_session,
            s_session_entry, // which FIFO to pull
            s_sessionID_out);

        network_query_controller_pull<QUERY_NUM>(
            // input
            s_query_vectors_per_session,
            s_session_entry, 
            // output
            s_query_vectors);

        ///////// Network Recv Ends /////////

        ////////////////////     Init     ////////////////////

        hls::stream<float> s_center_vectors_init_lookup_PE;
#pragma HLS stream variable=s_center_vectors_init_lookup_PE depth=2
// #pragma HLS resource variable=s_center_vectors_init_lookup_PE core=FIFO_SRL

    //     hls::stream<float> s_center_vectors_init_distance_computation_PE[PE_NUM_CENTER_DIST_COMP];
// #pragma HLS stream variable=s_center_vectors_init_distance_computation_PE depth=8
// #pragma HLS array_partition variable=s_center_vectors_init_distance_computation_PE complete
    // // #pragma HLS resource variable=s_center_vectors_init_distance_computation_PE core=FIFO_SRL

        // load_center_vectors(HBM_vector_quantizer, s_center_vectors_init_distance_computation_PE, s_center_vectors_init_lookup_PE);
        hls::stream<float> s_center_vectors_init_distance_computation_PE;
#pragma HLS stream variable=s_center_vectors_init_distance_computation_PE depth=8

        load_center_vectors(
            HBM_vector_quantizer,
            s_center_vectors_init_distance_computation_PE,
            s_center_vectors_init_lookup_PE);

        hls::stream<float> s_PQ_quantizer_init;
#pragma HLS stream variable=s_PQ_quantizer_init depth=4
// #pragma HLS resource variable=s_PQ_quantizer_init core=FIFO_SRL

        load_PQ_quantizer(HBM_product_quantizer, s_PQ_quantizer_init);

        hls::stream<float> s_OPQ_init;
#pragma HLS stream variable=s_OPQ_init depth=512
// #pragma HLS resource variable=s_OPQ_init core=FIFO_BRAM

        load_OPQ_matrix(HBM_OPQ_matrix, s_OPQ_init);

        ////////////////////     Preprocessing    ////////////////////

        hls::stream<float> s_preprocessed_query_vectors;
#pragma HLS stream variable=s_preprocessed_query_vectors depth=512
// #pragma HLS resource variable=s_preprocessed_query_vectors core=FIFO_BRAM

        OPQ_preprocessing<QUERY_NUM>(
            s_OPQ_init,
            s_query_vectors,
            s_preprocessed_query_vectors);

        hls::stream<float> s_preprocessed_query_vectors_lookup_PE;
#pragma HLS stream variable=s_preprocessed_query_vectors_lookup_PE depth=512
// #pragma HLS resource variable=s_preprocessed_query_vectors_lookup_PE core=FIFO_BRAM

        hls::stream<float> s_preprocessed_query_vectors_distance_computation_PE;
#pragma HLS stream variable=s_preprocessed_query_vectors_distance_computation_PE depth=512
// #pragma HLS resource variable=s_preprocessed_query_vectors_distance_computation_PE core=FIFO_BRAM

        broadcast_preprocessed_query_vectors<QUERY_NUM>(
            s_preprocessed_query_vectors,
            s_preprocessed_query_vectors_distance_computation_PE,
            s_preprocessed_query_vectors_lookup_PE);

        ////////////////////      Center Distance Computation    ////////////////////

    //     hls::stream<dist_cell_ID_t> s_partial_cell_distance[PE_NUM_CENTER_DIST_COMP];
// #pragma HLS stream variable=s_partial_cell_distance depth=512
    // // #pragma HLS resource variable=s_partial_cell_distance core=FIFO_BRAM
// #pragma HLS array_partition variable=s_partial_cell_distance complete

    //     compute_cell_distance_wrapper<QUERY_NUM>(
    //         s_center_vectors_init_distance_computation_PE, s_preprocessed_query_vectors_distance_computation_PE, 
    //         s_partial_cell_distance);

        ////////////////////     Select Scanned Cells     ////////////////////    

        hls::stream<dist_cell_ID_t> s_merged_cell_distance;
#pragma HLS stream variable=s_merged_cell_distance depth=512
// #pragma HLS resource variable=s_merged_cell_distance core=FIFO_BRAM

        compute_cell_distance_wrapper<QUERY_NUM>(
            s_center_vectors_init_distance_computation_PE, 
            s_preprocessed_query_vectors_distance_computation_PE, 
            s_merged_cell_distance);

        // merge_partial_cell_distance<QUERY_NUM>(
        //     s_partial_cell_distance, 
        //     s_merged_cell_distance);

        hls::stream<dist_cell_ID_t> s_selected_distance_cell_ID;
#pragma HLS stream variable=s_selected_distance_cell_ID depth=512
// #pragma HLS resource variable=s_selected_distance_cell_ID core=FIFO_BRAM

        select_Voronoi_cell<STAGE_3_PRIORITY_QUEUE_LEVEL, STAGE_3_PRIORITY_QUEUE_L1_NUM>(
            s_merged_cell_distance,
            s_selected_distance_cell_ID);

        hls::stream<int> s_searched_cell_id_lookup_PE;
#pragma HLS stream variable=s_searched_cell_id_lookup_PE depth=512
// #pragma HLS resource variable=s_searched_cell_id_lookup_PE core=FIFO_BRAM

        hls::stream<int> s_searched_cell_id_scan_controller;
#pragma HLS stream variable=s_searched_cell_id_scan_controller depth=512
// #pragma HLS resource variable=s_searched_cell_id_scan_controller core=FIFO_BRAM

        //  dist struct to cell ID (int)
        split_cell_ID<QUERY_NUM>(
            s_selected_distance_cell_ID, 
            s_searched_cell_id_lookup_PE, 
            s_searched_cell_id_scan_controller);

        ////////////////////     Center Vector Lookup     ////////////////////    

        hls::stream<float> s_center_vectors_lookup_PE;
#pragma HLS stream variable=s_center_vectors_lookup_PE depth=128
// #pragma HLS resource variable=s_center_vectors_lookup_PE core=FIFO_BRAM

        lookup_center_vectors<QUERY_NUM>(
            s_center_vectors_init_lookup_PE, 
            s_searched_cell_id_lookup_PE, 
            s_center_vectors_lookup_PE);

        ////////////////////     Distance Lookup Table Construction     ////////////////////    

        const int depth_s_distance_LUT = K * PE_NUM_TABLE_CONSTRUCTION;
        hls::stream<distance_LUT_PQ16_t> s_distance_LUT;
#pragma HLS stream variable=s_distance_LUT depth=depth_s_distance_LUT
// #pragma HLS resource variable=s_distance_LUT core=FIFO_BRAM

        lookup_table_construction_wrapper<QUERY_NUM>(
            s_PQ_quantizer_init, 
            s_center_vectors_lookup_PE, 
            s_preprocessed_query_vectors_lookup_PE, 
            s_distance_LUT);

        ////////////////////     Load PQ Codes     ////////////////////    

        hls::stream<int> s_scanned_entries_every_cell_Load_unit;
#pragma HLS stream variable=s_scanned_entries_every_cell_Load_unit depth=512
// #pragma HLS RESOURCE variable=s_scanned_entries_every_cell_Load_unit core=FIFO_BRAM

        hls::stream<int> s_scanned_entries_every_cell_Split_unit;
#pragma HLS stream variable=s_scanned_entries_every_cell_Split_unit depth=512
// #pragma HLS RESOURCE variable=s_scanned_entries_every_cell_Split_unit core=FIFO_BRAM

        hls::stream<int> s_scanned_entries_every_cell_PQ_lookup_computation;
#pragma HLS stream variable=s_scanned_entries_every_cell_PQ_lookup_computation depth=512
// #pragma HLS RESOURCE variable=s_scanned_entries_every_cell_PQ_lookup_computation core=FIFO_BRAM

        hls::stream<int> s_scanned_entries_every_cell_Dummy;
#pragma HLS stream variable=s_scanned_entries_every_cell_Dummy depth=512
// #pragma HLS RESOURCE variable=s_scanned_entries_every_cell_Dummy core=FIFO_BRAM

        hls::stream<int> s_last_valid_channel;
#pragma HLS stream variable=s_last_valid_channel depth=512
// #pragma HLS RESOURCE variable=s_last_valid_channel core=FIFO_BRAM

        hls::stream<int> s_start_addr_every_cell;
#pragma HLS stream variable=s_start_addr_every_cell depth=512
// #pragma HLS RESOURCE variable=s_start_addr_every_cell core=FIFO_BRAM

        hls::stream<int> s_scanned_entries_per_query_Sort_and_reduction;
#pragma HLS stream variable=s_scanned_entries_per_query_Sort_and_reduction depth=512
// #pragma HLS RESOURCE variable=s_scanned_entries_per_query_Sort_and_reduction core=FIFO_BRAM

        hls::stream<int> s_scanned_entries_per_query_Priority_queue;
#pragma HLS stream variable=s_scanned_entries_per_query_Priority_queue depth=512
// #pragma HLS RESOURCE variable=s_scanned_entries_per_query_Priority_queue core=FIFO_BRAM

        scan_controller<QUERY_NUM, NLIST, NPROBE>(
            HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid,
            s_searched_cell_id_scan_controller, 
            s_start_addr_every_cell,
            s_scanned_entries_every_cell_Load_unit, 
            s_scanned_entries_every_cell_Split_unit,
            s_scanned_entries_every_cell_PQ_lookup_computation,
            s_scanned_entries_every_cell_Dummy,
            s_last_valid_channel, 
            s_scanned_entries_per_query_Sort_and_reduction,
            s_scanned_entries_per_query_Priority_queue);

        // each 512 bit can store 3 set of (vecID, PQ code)
        hls::stream<single_PQ> s_single_PQ[3 * HBM_CHANNEL_NUM];
#pragma HLS stream variable=s_single_PQ depth=8
#pragma HLS array_partition variable=s_single_PQ complete
// #pragma HLS RESOURCE variable=s_single_PQ core=FIFO_SRL

        load_and_split_PQ_codes_wrapper<QUERY_NUM, NPROBE>(
            HBM_in0, HBM_in1, HBM_in2, HBM_in3, HBM_in4, HBM_in5, HBM_in6, HBM_in7, 
            HBM_in8, 
            // HBM_in9, 
            // HBM_in10, HBM_in11, HBM_in12, HBM_in13, 
            // HBM_in14, HBM_in15, HBM_in16, HBM_in17, HBM_in18, HBM_in19, 
            // HBM_in20, HBM_in21, HBM_in22, HBM_in23, HBM_in24, HBM_in25, HBM_in26, 
            s_start_addr_every_cell,
            s_scanned_entries_every_cell_Load_unit,
            s_scanned_entries_every_cell_Split_unit,
            s_single_PQ);

        // 64 streams = 21 channels * 3 + 1 dummy
        hls::stream<single_PQ_result> s_single_PQ_result[SORT_GROUP_NUM][16];
#pragma HLS stream variable=s_single_PQ_result depth=8
#pragma HLS array_partition variable=s_single_PQ_result complete
// #pragma HLS RESOURCE variable=s_single_PQ_result core=FIFO_SRL


        ////////////////////     Estimate Distance by LUT     ////////////////////    

        PQ_lookup_computation_wrapper<QUERY_NUM, NPROBE>(
            s_single_PQ, 
            s_distance_LUT, 
            s_scanned_entries_every_cell_PQ_lookup_computation,
            s_scanned_entries_every_cell_Dummy,
            s_last_valid_channel,
            s_single_PQ_result);

        ////////////////////     Sort Results     ////////////////////    
        Sort_reduction<single_PQ_result, SORT_GROUP_NUM * 16, 16, Collect_smallest> sort_reduction_module;

        hls::stream<single_PQ_result> s_sorted_PQ_result[16];
#pragma HLS stream variable=s_sorted_PQ_result depth=8
#pragma HLS array_partition variable=s_sorted_PQ_result complete
// #pragma HLS RESOURCE variable=s_sorted_PQ_result core=FIFO_SRL

        sort_reduction_module.sort_and_reduction<QUERY_NUM>(
            s_scanned_entries_per_query_Sort_and_reduction, 
            s_single_PQ_result, 
            s_sorted_PQ_result);


        hls::stream<single_PQ_result> s_output; // the top 10 numbers
#pragma HLS stream variable=s_output depth=512
// #pragma HLS RESOURCE variable=s_output core=FIFO_BRAM

        stream_redirect_to_priority_queue_wrapper<QUERY_NUM>(
            s_scanned_entries_per_query_Priority_queue, 
            s_sorted_PQ_result, 
            s_output);

        ///////// Network Send Starts /////////

        hls::stream<ap_uint512_t> s_network_results;
#pragma HLS stream variable=s_network_results depth=512
// #pragma HLS RESOURCE variable=s_output core=FIFO_BRAM

        network_output_converter<QUERY_NUM>(
            s_output, 
            s_network_results);

        // output: 10 * (vec_ID, dist) = 10 * 8 = 80 bytes
        //   need 2 * 512 bit Word
        // const int pkgWordCountOut = 2;
        // int expectedTxByteCnt = expectedRxByteCnt / 4;
        sendData(
            m_axis_tcp_tx_meta, 
            m_axis_tcp_tx_data, 
            s_axis_tcp_tx_status,
            s_network_results,
            s_sessionID_out,
            QUERY_NUM * 128, 
            2);
          
        tie_off_tcp_open_connection(m_axis_tcp_open_connection, 
            s_axis_tcp_open_status);


        // tie_off_tcp_tx(m_axis_tcp_tx_meta, 
        //                m_axis_tcp_tx_data, 
        //                s_axis_tcp_tx_status);
        tie_off_udp(s_axis_udp_rx, 
            m_axis_udp_tx, 
            s_axis_udp_rx_meta, 
            m_axis_udp_tx_meta);

        tie_off_tcp_close_con(m_axis_tcp_close_connection);
    }
}
