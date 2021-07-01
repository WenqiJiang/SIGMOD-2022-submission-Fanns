# Bitonic Sort Performance

Test "bitonic_sort_16_unfixed_scan_per_query", which is the unfixed scan per query equivalent of "bitonic_sort_16_fixed_scan_per_query_version2".

## Performance Modeling

Total time =  query_num * (L + N * II) (i.e., 1 insertion per cycle, given each query scans many entries)

here L=12

```
        +-------------+---------+---------+----------+-----------+-----------+-------+----------+
        |             |  Latency (cycles) | Iteration|  Initiation Interval  |  Trip |          |
        |  Loop Name  |   min   |   max   |  Latency |  achieved |   target  | Count | Pipelined|
        +-------------+---------+---------+----------+-----------+-----------+-------+----------+
        |- Loop 1     |        ?|        ?|         ?|          -|          -|  10000|    no    |
        | + Loop 1.1  |        ?|        ?|        12|          1|          1|      ?|    yes   |
        +-------------+---------+---------+----------+-----------+-----------+-------+----------+
s
```

Real Performance = 715.971 ms

140 MHz, 10000 query, 10000 scan per query

Theoretical performance  = 10000 * 10000 / 140 / 1e6 = 714.285 ms

**Results verified on hardware, reaches theoretical performance**

## Resource Usage (Vitis Build Flow)

FF = 15693

LUT = 20373

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|       32|    -|
|FIFO                 |        0|      -|      412|     3167|    -|
|Instance             |        8|      0|    21288|    27920|    -|
|Memory               |        -|      -|        -|        -|    -|
|Multiplexer          |        -|      -|        -|       36|    -|
|Register             |        -|      -|       12|        -|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |        8|      0|    21712|    31155|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |      0|        2|        7|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |    ~0   |      0|    ~0   |        2|    0|
+---------------------+---------+-------+---------+---------+-----+

+ Detail: 
    * Instance: 
    +----------------------------------+-------------------------------+---------+-------+-------+-------+-----+
    |             Instance             |             Module            | BRAM_18K| DSP48E|   FF  |  LUT  | URAM|
    +----------------------------------+-------------------------------+---------+-------+-------+-------+-----+
    |bitonic_sort_16_10000_19_U0       |bitonic_sort_16_10000_19       |        0|      0|  15693|  20373|    0|
    |control_signal_sender_10000_5_U0  |control_signal_sender_10000_5  |        0|      0|     22|    153|    0|
    |dummy_input_sender_10000_U0       |dummy_input_sender_10000_s     |        0|      0|    693|    604|    0|
    |vadd_control_s_axi_U              |vadd_control_s_axi             |        0|      0|   2486|   4520|    0|
    |vadd_gmem32_m_axi_U               |vadd_gmem32_m_axi              |        4|      0|    566|    766|    0|
    |vadd_gmem34_m_axi_U               |vadd_gmem34_m_axi              |        4|      0|    566|    766|    0|
    |write_result_10000_20_U0          |write_result_10000_20          |        0|      0|   1262|    738|    0|
    +----------------------------------+-------------------------------+---------+-------+-------+-------+-----+
    |Total                             |                               |        8|      0|  21288|  27920|    0|
    +----------------------------------+-------------------------------+---------+-------+-------+-------+-----+
```

# Bitonic Sort Optimization Log

**We reduced LUT usage from 48874 to 16628 for bitnoic sort 16.** 

The key idea is: instead of instantiate each compare-swap units as independent PE and interconnect them by dataflow, we inline those units and pipeline them in a loop to form a single large PE. This reduces FIFO overhead in dataflow (although I am still not sure why many LUTs are used there). 

## Unoptmized version

*bitonic_sort_16_fixed_scan_per_query_unoptmized*

For each compare-swap iteration, using a single function, and connect the functions by dataflow.

Functions (inline off, pipeline II=1):
```
template<const int array_len, const int partition_num>
void compare_swap_range_head_tail(
    single_PQ_result* input_array, single_PQ_result* output_array) {
#pragma HLS inline off
#pragma HLS pipeline II=1

template<const int array_len, const int partition_num>
void compare_swap_range_interval(
    single_PQ_result* input_array, single_PQ_result* output_array) {
    // e.g., in the image phase merge 4 -> 8, the 2nd and 3rd stage
#pragma HLS inline off
#pragma HLS pipeline II=1
```

Bitonic sort loop:
```
        for (int iter = 0; iter < iteration_per_query; iter++) {
#pragma HLS dataflow
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
```

The problem is that when treating these units as independent function, many FIFOs are added to the dataflow loop, consuming many LUTs.

Reading the log: 

hls_output/xcu280-fsvh2892-2L-e/syn/report/bitonic_sort_16_10000_10000_s_csynth.rpt

-> 

hls_output/xcu280-fsvh2892-2L-e/syn/report/dataflow_parent_loop_proc_csynth.rpt

-> 

hls_output/xcu280-fsvh2892-2L-e/syn/report/dataflow_in_loop_proc_csynth.rpt

**Almost half of the resources are used on FIFO:**

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|     1454|    -|
|FIFO                 |        0|     -|    34848|    23584|    -|
|Instance             |        -|     -|    14374|    20668|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|     3168|    -|
|Register             |        -|     -|      352|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        0|     0|    49574|    48874|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        0|     0|        5|       11|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        0|     0|        1|        3|    0|
+---------------------+---------+------+---------+---------+-----+

```

Here HLS report may be wrong, the FF consumed on FIFO is correct, yet LUTs are not counted. But **this is caused by the dataflow loop: there are FIFOs instantiated between every two functions.**

```
    * FIFO: 
    +--------------------------+---------+-----+----+-----+------+-----+---------+
    |           Name           | BRAM_18K|  FF | LUT| URAM| Depth| Bits| Size:D*B|
    +--------------------------+---------+-----+----+-----+------+-----+---------+
    |input_array_0_dist_U      |        0|   99|   0|    -|     2|   32|       64|
    |input_array_0_vec_ID_U    |        0|   99|   0|    -|     2|   32|       64|
    |input_array_10_dist_U     |        0|   99|   0|    -|     2|   32|       64|
    |input_array_10_vec_ID_U   |        0|   99|   0|    -|     2|   32|       64|
    |input_array_11_dist_U     |        0|   99|   0|    -|     2|   32|       64|

    ...
    
    ...

    ...

    |out_stage4_3_5_vec_ID_U   |        0|   99|   0|    -|     2|   32|       64|
    |out_stage4_3_6_dist_U     |        0|   99|   0|    -|     2|   32|       64|
    |out_stage4_3_6_vec_ID_U   |        0|   99|   0|    -|     2|   32|       64|
    |out_stage4_3_7_dist_U     |        0|   99|   0|    -|     2|   32|       64|
    |out_stage4_3_7_vec_ID_U   |        0|   99|   0|    -|     2|   32|       64|
    |out_stage4_3_8_dist_U     |        0|   99|   0|    -|     2|   32|       64|
    |out_stage4_3_8_vec_ID_U   |        0|   99|   0|    -|     2|   32|       64|
    |out_stage4_3_9_dist_U     |        0|   99|   0|    -|     2|   32|       64|
    |out_stage4_3_9_vec_ID_U   |        0|   99|   0|    -|     2|   32|       64|
    +--------------------------+---------+-----+----+-----+------+-----+---------+
    |Total                     |        0|34848|   0|    0|   704|11264|    22528|
    +--------------------------+---------+-----+----+-----+------+-----+---------+
```

## Optmized Version 1

The compare swap functions doesn't need to be independent. They can be inline function and then be pipelined by the loop.

Below is the optmized version:

Inline the compare-swap units:

```
template<const int array_len, const int partition_num>
void compare_swap_range_head_tail(
    single_PQ_result* input_array, single_PQ_result* output_array) {
    // e.g., in the image phase merge 4 -> 8, the 1st stage
    // Input these constants to make computation fast
#pragma HLS inline 

template<const int array_len, const int partition_num>
void compare_swap_range_interval(
    single_PQ_result* input_array, single_PQ_result* output_array) {
    // e.g., in the image phase merge 4 -> 8, the 2nd and 3rd stage
#pragma HLS inline
```

Merge these stages to a single function with pipeline II=1 (this function is in the dataflow loop, thus inline off):

```
void bitonic_compare_swap_merged(single_PQ_result input_array[16],
                                 single_PQ_result out_stage4_3[16]) {
#pragma HLS PIPELINE II=1
#pragma HLS INLINE off 

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
#pragma HLS array_partition variable=out_stage4_0 complete
#pragma HLS array_partition variable=out_stage4_1 complete
#pragma HLS array_partition variable=out_stage4_2 complete

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

}
```

The dataflow loop only contain 3 functions now, thus much less FIFO usage is expected:
```
        for (int iter = 0; iter < iteration_per_query; iter++) {
#pragma HLS dataflow
            load_input_stream<16>(s_input, input_array);
            // Total: 15 sub-stages
            // Stage 1
            bitonic_compare_swap_merged(input_array, out_stage4_3);
            
            write_output_stream<16>(out_stage4_3, s_output);
        }
```

Report: 

hls_output/xcu280-fsvh2892-2L-e/syn/report/dataflow_in_loop_csynth.rpt

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|      266|    -|
|FIFO                 |        0|     -|     6336|     4288|    -|
|Instance             |        -|     -|    11279|    17095|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|      576|    -|
|Register             |        -|     -|       64|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        0|     0|    17679|    22225|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        0|     0|        2|        5|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        0|     0|    ~0   |        1|    0|
+---------------------+---------+------+---------+---------+-----+
```

There's some FIFOs that can be optmized away.

```
    * FIFO: 
    +--------------------------+---------+----+----+-----+------+-----+---------+
    |           Name           | BRAM_18K| FF | LUT| URAM| Depth| Bits| Size:D*B|
    +--------------------------+---------+----+----+-----+------+-----+---------+
    |input_array_0_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_0_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_10_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |input_array_10_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |input_array_11_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |input_array_11_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |input_array_12_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |input_array_12_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |input_array_13_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |input_array_13_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |input_array_14_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |input_array_14_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |input_array_15_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |input_array_15_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |input_array_1_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_1_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_2_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_2_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_3_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_3_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_4_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_4_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_5_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_5_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_6_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_6_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_7_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_7_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_8_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_8_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |input_array_9_dist_U      |        0|  99|   0|    -|     2|   32|       64|
    |input_array_9_vec_ID_U    |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_0_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_0_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_10_dist_U    |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_10_vec_ID_U  |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_11_dist_U    |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_11_vec_ID_U  |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_12_dist_U    |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_12_vec_ID_U  |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_13_dist_U    |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_13_vec_ID_U  |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_14_dist_U    |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_14_vec_ID_U  |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_15_dist_U    |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_15_vec_ID_U  |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_1_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_1_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_2_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_2_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_3_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_3_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_4_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_4_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_5_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_5_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_6_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_6_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_7_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_7_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_8_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_8_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_9_dist_U     |        0|  99|   0|    -|     2|   32|       64|
    |out_stage4_3_9_vec_ID_U   |        0|  99|   0|    -|     2|   32|       64|
    +--------------------------+---------+----+----+-----+------+-----+---------+
    |Total                     |        0|6336|   0|    0|   128| 2048|     4096|
    +--------------------------+---------+----+----+-----+------+-----+---------+
```

## Optmized Version 2

Apply inline to "load_input_stream" and "write_output_stream":

```
template<const int array_len>
void load_input_stream(
    hls::stream<single_PQ_result> (&s_input)[array_len], 
    single_PQ_result input_array[array_len]) {
#pragma HLS inline 

template<const int array_len>
void write_output_stream(
    single_PQ_result output_array[array_len], 
    hls::stream<single_PQ_result> (&s_output)[array_len]) {
#pragma HLS inline 

```

In the bitonic sort loop, use pipeline instead of dataflow:

```
    for (int query_id = 0; query_id < query_num; query_id++) {

        for (int iter = 0; iter < iteration_per_query; iter++) {
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
```

Resource:

**Reduced LUT usage to 16628.** All resource are used on expressions. 

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|    15936|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|     -|        0|       27|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|      633|    -|
|Register             |        -|     -|    11373|       32|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        0|     0|    11373|    16628|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        0|     0|        1|        3|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        0|     0|    ~0   |        1|    0|
+---------------------+---------+------+---------+---------+-----+
```