# Performance

The performance is tested on "parallel_merge_32_to_16_unfixed_scan_per_query", which is equivalent to the unfixed scan per query version of "parallel_merge_32_to_16_fixed_scan_per_query_optmized".

## Performance Modeling

Total time =  query_num * (L + N * II) (i.e., 1 insertion per cycle, given each query scans many entries)

```
       * Loop: 
        +-------------+---------+---------+----------+-----------+-----------+-------+----------+
        |             |  Latency (cycles) | Iteration|  Initiation Interval  |  Trip |          |
        |  Loop Name  |   min   |   max   |  Latency |  achieved |   target  | Count | Pipelined|
        +-------------+---------+---------+----------+-----------+-----------+-------+----------+
        |- Loop 1     |        ?|        ?|         ?|          -|          -|  10000|    no    |
        | + Loop 1.1  |        ?|        ?|         7|          1|          1|      ?|    yes   |
        +-------------+---------+---------+----------+-----------+-----------+-------+----------+
```

Real Performance = 715.534 ms

140 MHz, 10000 query, 10000 scan per query

Theoretical performance  = 10000 * 10000 / 140 / 1e6 = 714.285 ms

**Results verified on hardware, reaches theoretical performance**

## Resource Consumption (Vitis Build Flow)

FF 9480

LUT 11861

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
|FIFO                 |        0|      -|      604|     4607|    -|
|Instance             |        8|      0|    15733|    19903|    -|
|Memory               |        -|      -|        -|        -|    -|
|Multiplexer          |        -|      -|        -|       36|    -|
|Register             |        -|      -|       12|        -|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |        8|      0|    16349|    24578|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |      0|        1|        5|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |    ~0   |      0|    ~0   |        1|    0|
+---------------------+---------+-------+---------+---------+-----+

+ Detail: 
    * Instance: 
    +------------------------------------+---------------------------------+---------+-------+------+-------+-----+
    |              Instance              |              Module             | BRAM_18K| DSP48E|  FF  |  LUT  | URAM|
    +------------------------------------+---------------------------------+---------+-------+------+-------+-----+
    |control_signal_sender_10000_5_U0    |control_signal_sender_10000_5    |        0|      0|    22|    153|    0|
    |dummy_input_sender_10000_U0         |dummy_input_sender_10000_s       |        0|      0|  1351|   1099|    0|
    |parallel_merge_sort_16_10000_19_U0  |parallel_merge_sort_16_10000_19  |        0|      0|  9480|  11861|    0|
    |vadd_control_s_axi_U                |vadd_control_s_axi               |        0|      0|  2486|   4520|    0|
    |vadd_gmem32_m_axi_U                 |vadd_gmem32_m_axi                |        4|      0|   566|    766|    0|
    |vadd_gmem34_m_axi_U                 |vadd_gmem34_m_axi                |        4|      0|   566|    766|    0|
    |write_result_10000_20_U0            |write_result_10000_20            |        0|      0|  1262|    738|    0|
    +------------------------------------+---------------------------------+---------+-------+------+-------+-----+
    |Total                               |                                 |        8|      0| 15733|  19903|    0|
    +------------------------------------+---------------------------------+---------+-------+------+-------+-----+
```

# Parallel Merge Optmization Log

Reduce LUT usage from 29544 to 9588.

Optmization strategy: refer to bitonic_sort. Replace small PEs interconnected by dataflow by a large PE using a pipelined loop containing inline functions.

## Unoptmized Version

A number of not inlined function interconnected by dataflow, leading to unneeded FIFOs.

Code:
```
        for (int iter = 0; iter < iteration_per_query; iter++) {
#pragma HLS dataflow

            load_input_stream<16>(s_input_A, input_array_A);
            load_input_stream<16>(s_input_B, input_array_B);

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
```

Report: 
```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|      940|    -|
|FIFO                 |        0|     -|    22176|    15008|    -|
|Instance             |        -|     -|     7184|    11544|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|     2052|    -|
|Register             |        -|     -|      230|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        0|     0|    29590|    29544|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        0|     0|        3|        6|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        0|     0|        1|        2|    0|
+---------------------+---------+------+---------+---------+-----+
```

## Optimized Version

Inline function, set II=1:

```
template<const int array_len>
void load_input_stream(
    hls::stream<single_PQ_result> (&s_input)[array_len], 
    single_PQ_result input_array[array_len]) {
#pragma HLS inline 

template<const int array_len>
void compare_select_range_head_tail(
    single_PQ_result* input_array_A, single_PQ_result* input_array_B, 
    single_PQ_result* output_array) {
    // e.g., in the image phase merge 4 -> 8, the 1st stage
    // Input these constants to make computation fast
#pragma HLS inline
```

Use a loop with pipeline:

```
        for (int iter = 0; iter < iteration_per_query; iter++) {
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
```

Resource:

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|     8608|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|     -|        0|       27|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|      921|    -|
|Register             |        -|     -|     7272|       32|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        0|     0|     7272|     9588|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        0|     0|    ~0   |        2|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        0|     0|    ~0   |    ~0   |    0|
+---------------------+---------+------+---------+---------+-----+
```
