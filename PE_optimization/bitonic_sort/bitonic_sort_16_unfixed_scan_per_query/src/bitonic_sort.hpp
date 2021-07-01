#include "constants.hpp"

void compare_swap(
    single_PQ_result* input_array, single_PQ_result* output_array, int idxA, int idxB);

template<const int array_len, const int partition_num>
void compare_swap_range_head_tail(
    single_PQ_result* input_array, single_PQ_result* output_array);

template<const int array_len, const int partition_num>
void compare_swap_range_interval(
    single_PQ_result* input_array, single_PQ_result* output_array);

template<const int array_len>
void load_input_stream(
    hls::stream<single_PQ_result> (&s_input)[array_len], 
    single_PQ_result input_array[array_len]);

template<const int array_len>
void write_output_stream(
    single_PQ_result output_array[array_len], 
    hls::stream<single_PQ_result> (&s_output)[array_len]);

template<const int query_num>
void bitonic_sort_16(
    hls::stream<int>& s_control_iter_num_per_query,
    hls::stream<single_PQ_result> (&s_input)[16],
    hls::stream<single_PQ_result> (&s_output)[16]);

////////////////////     Sorting Network Start    ////////////////////

void compare_swap(
    single_PQ_result* input_array, single_PQ_result* output_array, int idxA, int idxB) {
    // note: idxA must < idxB
#pragma HLS inline
    if (input_array[idxA].dist > input_array[idxB].dist) {
        output_array[idxA] = input_array[idxB];
        output_array[idxB] = input_array[idxA];
    }
    else {
        output_array[idxA] = input_array[idxA];
        output_array[idxB] = input_array[idxB];
    }
}

template<const int array_len, const int partition_num>
void compare_swap_range_head_tail(
    single_PQ_result* input_array, single_PQ_result* output_array) {
    // e.g., in the image phase merge 4 -> 8, the 1st stage
    // Input these constants to make computation fast
#pragma HLS inline
#pragma HLS pipeline II=1

    const int elements_per_partition = array_len / partition_num;
    const int operations_per_partition = elements_per_partition / 2;

    for (int i = 0; i < partition_num; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < operations_per_partition; j++) {
#pragma HLS UNROLL
            compare_swap(input_array, output_array, 
                i * elements_per_partition + j, (i + 1) * elements_per_partition - 1 - j);
        }
    }
}

template<const int array_len, const int partition_num>
void compare_swap_range_interval(
    single_PQ_result* input_array, single_PQ_result* output_array) {
    // e.g., in the image phase merge 4 -> 8, the 2nd and 3rd stage
#pragma HLS inline
#pragma HLS pipeline II=1

    const int elements_per_partition = array_len / partition_num;
    const int operations_per_partition = elements_per_partition / 2;
    const int interval = operations_per_partition;

    for (int i = 0; i < partition_num; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < operations_per_partition; j++) {
#pragma HLS UNROLL
        compare_swap(input_array, output_array, 
            i * elements_per_partition + j, i * elements_per_partition + interval + j);
        }
    }
}

template<const int array_len>
void load_input_stream(
    hls::stream<single_PQ_result> (&s_input)[array_len], 
    single_PQ_result input_array[array_len]) {
#pragma HLS inline
#pragma HLS pipeline II=1

    for (int s = 0; s < array_len; s++) {
#pragma HLS UNROLL 
        input_array[s] = s_input[s].read();
    }
}

template<const int array_len>
void write_output_stream(
    single_PQ_result output_array[array_len], 
    hls::stream<single_PQ_result> (&s_output)[array_len]) {
#pragma HLS inline
#pragma HLS pipeline II=1

    for (int s = 0; s < array_len; s++) {
#pragma HLS UNROLL 
        s_output[s].write(output_array[s]);
    }
}

template<const int query_num>
void bitonic_sort_16(
    hls::stream<int>& s_control_iter_num_per_query,
    hls::stream<single_PQ_result> (&s_input)[16],
    hls::stream<single_PQ_result> (&s_output)[16]) {

    single_PQ_result input_array[16];
#pragma HLS array_partition variable=input_array complete

    single_PQ_result out_stage1_0[16];
#pragma HLS array_partition variable=out_stage1_0 complete

    single_PQ_result out_stage2_0[16];
    single_PQ_result out_stage2_1[16];
#pragma HLS array_partition variable=out_stage2_0 complete
#pragma HLS array_partition variable=out_stage2_1 complete

    single_PQ_result out_stage3_0[16];
    single_PQ_result out_stage3_1[16];
    single_PQ_result out_stage3_2[16];
#pragma HLS array_partition variable=out_stage3_0 complete
#pragma HLS array_partition variable=out_stage3_1 complete
#pragma HLS array_partition variable=out_stage3_2 complete

    single_PQ_result out_stage4_0[16];
    single_PQ_result out_stage4_1[16];
    single_PQ_result out_stage4_2[16];
    single_PQ_result out_stage4_3[16];
#pragma HLS array_partition variable=out_stage4_0 complete
#pragma HLS array_partition variable=out_stage4_1 complete
#pragma HLS array_partition variable=out_stage4_2 complete
#pragma HLS array_partition variable=out_stage4_3 complete


    for (int query_id = 0; query_id < query_num; query_id++) {

        int iter_num = s_control_iter_num_per_query.read();

        for (int iter = 0; iter < iter_num; iter++) {
#pragma HLS pipeline II=1
            load_input_stream<16>(s_input, input_array);
            // Total: 15 sub-stages
            // Stage 1
            compare_swap_range_interval<16, 8>(input_array, out_stage1_0);

            // Stage 2: 2 -> 4
            compare_swap_range_head_tail<16, 4>(out_stage1_0, out_stage2_0);
            compare_swap_range_interval<16, 8>(out_stage2_0, out_stage2_1);

            // Stage 3: 4 -> 8
            compare_swap_range_head_tail<16, 2>(out_stage2_1, out_stage3_0);
            compare_swap_range_interval<16, 4>(out_stage3_0, out_stage3_1);
            compare_swap_range_interval<16, 8>(out_stage3_1, out_stage3_2);

            // Stage 4: 8 -> 16
            compare_swap_range_head_tail<16, 1>(out_stage3_2, out_stage4_0);
            compare_swap_range_interval<16, 2>(out_stage4_0, out_stage4_1);
            compare_swap_range_interval<16, 4>(out_stage4_1, out_stage4_2);
            compare_swap_range_interval<16, 8>(out_stage4_2, out_stage4_3);
            
            write_output_stream<16>(out_stage4_3, s_output);
        }
    }
}
////////////////////     Sorting Network Ends    ////////////////////

////////////////////     Merge and Sort and Filter Network Starts    ////////////////////

void compare_select(
    single_PQ_result* input_array_A, single_PQ_result* input_array_B, 
    single_PQ_result* output_array, int idxA, int idxB) {
    // note: idxOut = idxA
    // select the smallest of the two as output
#pragma HLS inline
#pragma HLS pipeline II=1
    if (input_array_A[idxA].dist > input_array_B[idxB].dist) {
        output_array[idxA] = input_array_B[idxB];
    }
    else {
        output_array[idxA] = input_array_A[idxA];
    }
}

template<const int array_len>
void compare_select_range_head_tail(
    single_PQ_result* input_array_A, single_PQ_result* input_array_B, 
    single_PQ_result* output_array) {
    // e.g., in the image phase merge 4 -> 8, the 1st stage
    // Input these constants to make computation fast
#pragma HLS inline
#pragma HLS pipeline II=1

    // A[0] <-> B[127], A[1] <-> B[126], etc.
    for (int j = 0; j < array_len; j++) {
#pragma HLS UNROLL
        compare_select(
            input_array_A, input_array_B, output_array, 
            j, array_len - 1 - j);
    }
}

template<const int query_num>
void parallel_merge_sort_16(
    hls::stream<int>& s_control_iter_num_per_query,
    hls::stream<single_PQ_result> (&input_stream_A)[16],
    hls::stream<single_PQ_result> (&input_stream_B)[16],
    hls::stream<single_PQ_result> (&s_output)[16]) {
    
    // given 2 input sorted array A and B of len array_len, 
    // merge and sort and reduction to output array C of len array_len,
    // containing the smallest numbers among A and B. 

    single_PQ_result input_array_A[16];
    single_PQ_result input_array_B[16];
#pragma HLS array_partition variable=input_array_A complete
#pragma HLS array_partition variable=input_array_B complete

    single_PQ_result out_stage_0[16];
    single_PQ_result out_stage_1[16];
    single_PQ_result out_stage_2[16];
    single_PQ_result out_stage_3[16];
    single_PQ_result out_stage_4[16];
#pragma HLS array_partition variable=out_stage_0 complete
#pragma HLS array_partition variable=out_stage_1 complete
#pragma HLS array_partition variable=out_stage_2 complete
#pragma HLS array_partition variable=out_stage_3 complete
#pragma HLS array_partition variable=out_stage_4 complete

    for (int query_id = 0; query_id < query_num; query_id++) {
        
        // Each query scans different number of entries
        int iter_num = s_control_iter_num_per_query.read();

        for (int iter = 0; iter < iter_num; iter++) {
#pragma HLS pipeline II=1
            load_input_stream<16>(input_stream_A, input_array_A);
            load_input_stream<16>(input_stream_B, input_array_B);

            // select the smallest 128 numbers
            compare_select_range_head_tail<16>(
                input_array_A, input_array_B, out_stage_0);

            // sort the smallest 16 numbers
            /* Analogue to sorting 32 (a half of sorting 32) */
            compare_swap_range_interval<16, 1>(out_stage_0, out_stage_1);
            compare_swap_range_interval<16, 2>(out_stage_1, out_stage_2);
            compare_swap_range_interval<16, 4>(out_stage_2, out_stage_3);
            compare_swap_range_interval<16, 8>(out_stage_3, out_stage_4);

            write_output_stream<16>(out_stage_4, s_output);
        }
    }
}
