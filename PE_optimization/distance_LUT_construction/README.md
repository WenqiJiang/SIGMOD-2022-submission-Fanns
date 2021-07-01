# Performance & Resource analysis for distance LUT PE

Update: use the multiple_lookup_table_construction_PEs_optimized_version4_systolic version as the final version. Some other code styles either causes placement error, or consumes more resources.

Previously, we optimized the PE as single_lookup_table_construction_PE_optimized_version2. This version can roughly output 1 row of distance LUT per cycle (1 row per 1.2 cycle, dependent to nprobe), thus should be sufficient for most cases. However, this large PE will lead to placement error.

Another try is when we use the large unoptimized version multiple_lookup_table_construction_PEs_unoptimized_large which consumes ~200 DSPs. This version works without network stack. But with the network stack only the small version survives the placement & routing.

## multiple_lookup_table_construction_PEs_optimized_version4_systolic

I use two dataflow functions for LUT table construction and resource gathering, thus making a perfect nested loop. To be more specific, each row contains 16 values, and all of them must be gathered to pass as result. In this implementation, I use one function computing the 16 values per row, with a pipeline of 1 CC per value, and another function to gather 16 numbers as a wide data type. By doing this, FF / LUT usage is reduced by 2~3x compared to the unoptimized version.

This version is also interconnected by systolic array, avoiding the high-fanout problem.

The PE size can be increased by doing more aggressive unroll, e.g., compute 2 values per pipeline cycle and also increase the FIFO number between the two dataflow functions accordingly.


Each PE is responsible for constructing one distance LUT. For example, nprobe=17 and PE_num=3, then the number of LUTs constructed by three PEs are: 6, 6, 5.

Performance: 

Per PE:

total_time = query_num * (L_load_query + nprobe_per_PE * (L_load_and_compute_residual + L_compute + N_comupte * II_compute)

here, L_load_query = 128, L_load_and_compute_residual = 132, L_compute = 36, N_comupte = 256 * m = 256 * 16 = 4096, II_compute = 1

nprobe_per_PE can be different for different PE for the case that each PE has different number of LUTs to construct, e.g., 6, 6, 5 as above.

For all PEs, 

total_time = query_num * (L_load_query + nprobe_per_PE_max * (L_load_and_compute_residual + L_compute + N_comupte * II_compute))

For the example above, nprobe_per_PE_max = 6

Verification: Suppose query_num=10000, nprobe=17, PE num=4, nprobe_per_PE=5 

10000 * (128 + 5 * (132 + 36 + 4096 * 1)) = 214480000, very close to 214702771 estimated by HLS.

10000 query performance:

```
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  | 7.14 ns | 5.018 ns |   1.93 ns  |
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +-----------+-----------+-----------+-----------+-----------+-----------+----------+
    |    Latency (cycles)   |   Latency (absolute)  |        Interval       | Pipeline |
    |    min    |    max    |    min    |    max    |    min    |    max    |   Type   |
    +-----------+-----------+-----------+-----------+-----------+-----------+----------+
    |  214702771|  214702771| 1.534 sec | 1.534 sec |  214702772|  214702772| dataflow |
    +-----------+-----------+-----------+-----------+-----------+-----------+----------+

    + Detail: 
        * Instance: 
        +--------------------------------------------------------+-----------------------------------------------------+-----------+-----------+-----------+-----------+-----------+-----------+---------+
        |                                                        |                                                     |    Latency (cycles)   |   Latency (absolute)  |        Interval       | Pipeline|
        |                        Instance                        |                        Module                       |    min    |    max    |    min    |    max    |    min    |    max    |   Type  |
        +--------------------------------------------------------+-----------------------------------------------------+-----------+-----------+-----------+-----------+-----------+-----------+---------+
        |lookup_table_construction_compute_midlle_10000_5_35_U0  |lookup_table_construction_compute_midlle_10000_5_35  |  214702771|  214702771| 1.534 sec | 1.534 sec |  214702771|  214702771|   none  |
        |gather_float_to_distance_LUT_PQ16_10000_5_136_U0        |gather_float_to_distance_LUT_PQ16_10000_5_136        |  204800002|  204800002| 1.463 sec | 1.463 sec |  204800002|  204800002|   none  |
        |lookup_table_construction_forward_middle_10000_37_U0    |lookup_table_construction_forward_middle_10000_37    |   25850001|   25850001| 0.185 sec | 0.185 sec |   25850001|   25850001|   none  |
        +--------------------------------------------------------+-----------------------------------------------------+-----------+-----------+-----------+-----------+-----------+-----------+---------+

        * Loop: 
        N/A

Compute PE pipeline depth:

        * Loop: 
        +---------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+
        |                                             |    Latency (cycles)   | Iteration|  Initiation Interval  |  Trip |          |
        |                  Loop Name                  |    min    |    max    |  Latency |  achieved |   target  | Count | Pipelined|
        +---------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+
        |- Loop 1                                     |      32768|      32768|         2|          1|          1|  32768|    yes   |
        |- Loop 2                                     |  214670000|  214670000|     21467|          -|          -|  10000|    no    |
        | + Loop 2.1                                  |        128|        128|         2|          1|          1|    128|    yes   |
        | + Loop 2.2                                  |      21335|      21335|      4267|          -|          -|      5|    no    |
        |  ++ residual_center_vectors                 |        132|        132|         6|          1|          1|    128|    yes   |
        |  ++ single_row_lookup_table_construction_L  |       4130|       4130|        36|          1|          1|   4096|    yes   |
        +---------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+


================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|       22|    -|
|FIFO                 |       16|     -|      322|      112|    -|
|Instance             |        1|    54|     7497|     6233|    8|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|       36|    -|
|Register             |        -|     -|        6|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |       17|    54|     7825|     6403|    8|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        1|     1|    ~0   |        1|    2|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |    ~0   |  ~0  |    ~0   |    ~0   |  ~0 |
+---------------------+---------+------+---------+---------+-----+
```

There are two MUST-NOTICE stuffs:

First, the query forward FIFO must be long (we use 512 here), otherwise theoretical performance cannot be achieved. This can also be achieved by increasing the depth of the forward LUT FIFO, but that one is very wide (512-bit) thus consuming more resources. We thus to choose to use long query forward FIFO.

```
    hls::stream<float> s_query_vectors_table_construction_PE_forward[PE_NUM_TABLE_CONSTRUCTION_LARGER];
#pragma HLS stream variable=s_query_vectors_table_construction_PE_forward depth=512
```

Second, the output LUT FIFO must be long as well, because the next stage (stage 5) won't consume those LUTs immediately. In unused_opt4_systolic_experiments there are two experiments about it. multiple_lookup_table_construction_PEs_optimized_version4_systolic_sleep assumes the slow consumption, and takes 1.5s (theoretical 1.22 s) while by increasing the FIFO depth to 256 * PE_NUM_systolic in multiple_lookup_table_construction_PEs_optimized_version4_systolic_sleep_long_FIFO, the theoretical performance is achieved.

Currently we set the output FIFO depth to K * PE_NUM, which is not necessarily enough (a safer depth is K * NPROBE but too expensive) if stage 5 are continually accessing clusters of unbalanced sizes. 

```
    const int s_lookup_table_depth = K * PE_NUM_TABLE_CONSTRUCTION;
    hls::stream<distance_LUT_PQ16_t> s_lookup_table;
#pragma HLS stream variable=s_lookup_table depth=s_lookup_table_depth
```

## multiple_lookup_table_construction_PEs_unoptimized_small

A smaller version of "multiple_lookup_table_construction_PEs_unoptimized_large" DSP consumption ~50 vs ~200. This one survives placement & routing with network stack.


Each PE is responsible for constructing one distance LUT. For example, nprobe=17 and PE_num=3, then the number of LUTs constructed by three PEs are: 6, 6, 5.

Performance: 

Per PE:

total_time = query_num * (L_load_query + nprobe_per_PE * (L_load_and_compute_residual + L_compute + N_comupte * II_compute)

here, L_load_query = 128, L_load_and_compute_residual = 132, L_compute = 68, N_comupte = 256, II_compute = 16

nprobe_per_PE can be different for different PE for the case that each PE has different number of LUTs to construct, e.g., 6, 6, 5 as above.

For all PEs, 

total_time = query_num * (L_load_query + nprobe_per_PE_max * (L_load_and_compute_residual + L_compute + N_comupte * II_compute))

For the example above, nprobe_per_PE_max = 6

Verification: Suppose query_num=10000, nprobe=32, nprobe_per_PE=8

10000 * (128 + 8 * (132 + 68 + 256 * 16)) = 344960000, very close to 344392801 estimated by HLS.

```
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  | 7.14 ns | 5.380 ns |   1.93 ns  |
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +-----------+-----------+-----------+-----------+-----------+-----------+---------+
    |    Latency (cycles)   |   Latency (absolute)  |        Interval       | Pipeline|
    |    min    |    max    |    min    |    max    |    min    |    max    |   Type  |
    +-----------+-----------+-----------+-----------+-----------+-----------+---------+
    |  344392801|  344392801| 2.460 sec | 2.460 sec |  344392801|  344392801|   none  |
    +-----------+-----------+-----------+-----------+-----------+-----------+---------+

    + Detail: 
        * Instance: 
        N/A

        * Loop: 
        +-------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+
        |                                           |    Latency (cycles)   | Iteration|  Initiation Interval  |  Trip |          |
        |                 Loop Name                 |    min    |    max    |  Latency |  achieved |   target  | Count | Pipelined|
        +-------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+
        |- Loop 1                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 2                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 3                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 4                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 5                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 6                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 7                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 8                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 9                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 10                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 11                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 12                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 13                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 14                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 15                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 16                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 17                                  |  344360000|  344360000|     34436|          -|          -|  10000|    no    |
        | + Loop 17.1                               |        128|        128|         2|          1|          1|    128|    yes   |
        | + Loop 17.2                               |      34304|      34304|      4288|          -|          -|      8|    no    |
        |  ++ residual_center_vectors               |        132|        132|         6|          1|          1|    128|    yes   |
        |  ++ single_row_lookup_table_construction  |       4147|       4147|        68|         16|         16|    256|    yes   |
        +-------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+
```


Utilization:

Part 1: Per compute PE

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|      500|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|    54|     4489|     4228|    -|
|Memory               |        1|     -|     1024|       64|   16|
|Multiplexer          |        -|     -|        -|     6806|    -|
|Register             |        -|     -|    17187|      352|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        1|    54|    22700|    11950|   16|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |     1|        2|        2|    5|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |    ~0   |  ~0  |    ~0   |    ~0   |    1|
+---------------------+---------+------+---------+---------+-----+
```

Part 2: Look up center vector

For this part, LUT and FF consumption is negligible (< 500). Only URAM consumptionn.

Since we use float as URAM data type, per URAM available size is reduced to 128Kb.

* nlist = 1024 -> 32 URAM
* nlist = 2048 -> 64 URAM
* nlist = 4096 -> 128 URAM
* nlist = 8192 -> 256 URAM
* nlist = 16384 -> 512 URAM
* nlist > 16384 -> 1 HBM bank

If we use ap_uint<64>, then the URAM consumption will be:

* nlist = 1024 -> 16 URAM
* nlist = 2048 -> 32 URAM
* nlist = 4096 -> 64 URAM
* nlist = 8192 -> 128 URAM
* nlist = 16384 -> 256 URAM
* nlist = 32768 -> 512 URAM
* nlist > 32768 HBM bank


## multiple_lookup_table_construction_PEs_unoptimized_medium

A medium-size version of "multiple_lookup_table_construction_PEs_unoptimized_large" DSP consumption ~100 vs ~200. This one survives placement & routing with network stack.


Each PE is responsible for constructing one distance LUT. For example, nprobe=17 and PE_num=3, then the number of LUTs constructed by three PEs are: 6, 6, 5.

Performance: 

Per PE:

total_time = query_num * (L_load_query + nprobe_per_PE * (L_load_and_compute_residual + L_compute + N_comupte * II_compute)

here, L_load_query = 128, L_load_and_compute_residual = 132, L_compute = 53, N_comupte = 256, II_compute = 8

nprobe_per_PE can be different for different PE for the case that each PE has different number of LUTs to construct, e.g., 6, 6, 5 as above.

For all PEs, 

total_time = query_num * (L_load_query + nprobe_per_PE_max * (L_load_and_compute_residual + L_compute + N_comupte * II_compute))

For the example above, nprobe_per_PE_max = 6

Verification: Suppose query_num=10000, nprobe=32, nprobe_per_PE=8

10000 * (128 + 8 * (132 + 53 + 256 * 8)) = 179920000, very close to 179992801 estimated by HLS.

```
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  | 7.14 ns | 5.018 ns |   1.93 ns  |
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +-----------+-----------+-----------+-----------+-----------+-----------+---------+
    |    Latency (cycles)   |   Latency (absolute)  |        Interval       | Pipeline|
    |    min    |    max    |    min    |    max    |    min    |    max    |   Type  |
    +-----------+-----------+-----------+-----------+-----------+-----------+---------+
    |  179992801|  179992801| 1.286 sec | 1.286 sec |  179992801|  179992801|   none  |
    +-----------+-----------+-----------+-----------+-----------+-----------+---------+

    + Detail: 
        * Instance: 
        N/A

        * Loop: 
        +-------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+
        |                                           |    Latency (cycles)   | Iteration|  Initiation Interval  |  Trip |          |
        |                 Loop Name                 |    min    |    max    |  Latency |  achieved |   target  | Count | Pipelined|
        +-------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+
        |- Loop 1                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 2                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 3                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 4                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 5                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 6                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 7                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 8                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 9                                   |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 10                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 11                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 12                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 13                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 14                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 15                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 16                                  |       2048|       2048|         2|          1|          1|   2048|    yes   |
        |- Loop 17                                  |  179960000|  179960000|     17996|          -|          -|  10000|    no    |
        | + Loop 17.1                               |        128|        128|         2|          1|          1|    128|    yes   |
        | + Loop 17.2                               |      17864|      17864|      2233|          -|          -|      8|    no    |
        |  ++ residual_center_vectors               |        132|        132|         6|          1|          1|    128|    yes   |
        |  ++ single_row_lookup_table_construction  |       2092|       2092|        53|          8|          8|    256|    yes   |
        +-------------------------------------------+-----------+-----------+----------+-----------+-----------+-------+----------+

```


Utilization:

Part 1: Per compute PE

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|      500|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|   108|     8978|     8219|    -|
|Memory               |        1|     -|     1024|       64|   16|
|Multiplexer          |        -|     -|        -|     7499|    -|
|Register             |        -|     -|    20029|     1376|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        1|   108|    30031|    17658|   16|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |     3|        3|        4|    5|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |    ~0   |     1|        1|        1|    1|
+---------------------+---------+------+---------+---------+-----+
```

Part 2: Look up center vector

For this part, LUT and FF consumption is negligible (< 500). Only URAM consumptionn.

Since we use float as URAM data type, per URAM available size is reduced to 128Kb.

* nlist = 1024 -> 32 URAM
* nlist = 2048 -> 64 URAM
* nlist = 4096 -> 128 URAM
* nlist = 8192 -> 256 URAM
* nlist = 16384 -> 512 URAM
* nlist > 16384 -> 1 HBM bank

If we use ap_uint<64>, then the URAM consumption will be:

* nlist = 1024 -> 16 URAM
* nlist = 2048 -> 32 URAM
* nlist = 4096 -> 64 URAM
* nlist = 8192 -> 128 URAM
* nlist = 16384 -> 256 URAM
* nlist = 32768 -> 512 URAM
* nlist > 32768 HBM bank

## multiple_lookup_table_construction_PEs_unoptimized_large

Each PE is responsible for constructing one distance LUT. For example, nprobe=17 and PE_num=3, then the number of LUTs constructed by three PEs are: 6, 6, 5.

Performance: 

Per PE:

total_time = query_num * (L_load_query + nprobe_per_PE * (L_load_and_compute_residual + L_compute + N_comupte * II_compute)

here, L_load_query = 128, L_load_and_compute_residual = 132, L_compute = 39, N_comupte = 256, II_compute = 4

nprobe_per_PE can be different for different PE for the case that each PE has different number of LUTs to construct, e.g., 6, 6, 5 as above.

For all PEs, 

total_time = query_num * (L_load_query + nprobe_per_PE_max * (L_load_and_compute_residual + L_compute + N_comupte * II_compute))

For the example above, nprobe_per_PE_max = 6

Verification: Suppose query_num=10000, nprobe=32, nprobe_per_PE=8

10000 * (128 + 8 * (132 + 39 + 256 * 4)) = 96880000, very close to 97240000 estimated by HLS.

```
        * Loop: 
        +-------------------------------------------+----------+----------+----------+-----------+-----------+-------+----------+
        |                                           |   Latency (cycles)  | Iteration|  Initiation Interval  |  Trip |          |
        |                 Loop Name                 |    min   |    max   |  Latency |  achieved |   target  | Count | Pipelined|
        +-------------------------------------------+----------+----------+----------+-----------+-----------+-------+----------+
        |- Loop 1                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 2                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 3                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 4                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 5                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 6                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 7                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 8                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 9                                   |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 10                                  |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 11                                  |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 12                                  |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 13                                  |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 14                                  |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 15                                  |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 16                                  |      2048|      2048|         2|          1|          1|   2048|    yes   |
        |- Loop 17                                  |  97240000|  97240000|      9724|          -|          -|  10000|    no    |
        | + Loop 17.1                               |       128|       128|         2|          1|          1|    128|    yes   |
        | + Loop 17.2                               |      9592|      9592|      1199|          -|          -|      8|    no    |
        |  ++ residual_center_vectors               |       132|       132|         6|          1|          1|    128|    yes   |
        |  ++ single_row_lookup_table_construction  |      1058|      1058|        39|          4|          1|    256|    yes   |
        +-------------------------------------------+----------+----------+----------+-----------+-----------+-------+----------+
```


Utilization:

Part 1: Per compute PE

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|      500|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|   216|    17956|    16201|    -|
|Memory               |        1|     -|     1024|       64|   16|
|Multiplexer          |        -|     -|        -|     7755|    -|
|Register             |        -|     -|    21308|     2656|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        1|   216|    40288|    27176|   16|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |     7|        4|        6|    5|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |    ~0   |     2|        1|        2|    1|
+---------------------+---------+------+---------+---------+-----+

```

Part 2: Look up center vector

For this part, LUT and FF consumption is negligible (< 500). Only URAM consumptionn.

Since we use float as URAM data type, per URAM available size is reduced to 128Kb.

* nlist = 1024 -> 32 URAM
* nlist = 2048 -> 64 URAM
* nlist = 4096 -> 128 URAM
* nlist = 8192 -> 256 URAM
* nlist = 16384 -> 512 URAM
* nlist > 16384 -> 1 HBM bank

If we use ap_uint<64>, then the URAM consumption will be:

* nlist = 1024 -> 16 URAM
* nlist = 2048 -> 32 URAM
* nlist = 4096 -> 64 URAM
* nlist = 8192 -> 128 URAM
* nlist = 16384 -> 256 URAM
* nlist = 32768 -> 512 URAM
* nlist > 32768 HBM bank

## multiple_lookup_table_construction_PEs_optimized_version1

There are 2 major step to construct a LUT for a single scanned cell:
* for (d in 128): load residual_vector = (query_vector - centroid_vector)
* for (k in 256): compute a row of lookup table

Optimized version 1 applies full parallelism in step 2 to (a) compute 128 distances between the residual_vector and that row of product quantizer and (b) for each partition (m=16 in total), parallel sum the squared distance of 8 scalar distances. Such parallelism enables II=1 for computing a row (but the latency is high anyway).

### Performance

81920003 for 10000 queries. This performance is bound by the gather unit concatenating results from 2 PEs.

For the case of multiple PEs that saturate the gather unit:

Total cycles = query_num * nprobe * K_star = 10000 * 32 * 256 = 81920000

```

================================================================
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  | 7.14 ns | 5.018 ns |   1.93 ns  |
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +----------+----------+-----------+-----------+----------+----------+----------+
    |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline |
    |    min   |    max   |    min    |    max    |    min   |    max   |   Type   |
    +----------+----------+-----------+-----------+----------+----------+----------+
    |  81920051|  81920051| 0.585 sec | 0.585 sec |  81920003|  81920003| dataflow |
    +----------+----------+-----------+-----------+----------+----------+----------+

    + Detail: 
        * Instance: 
        +---------------------------------------------+------------------------------------------+----------+----------+-----------+-----------+----------+----------+---------+
        |                                             |                                          |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline|
        |                   Instance                  |                  Module                  |    min   |    max   |    min    |    max    |    min   |    max   |   Type  |
        +---------------------------------------------+------------------------------------------+----------+----------+-----------+-----------+----------+----------+---------+
        |lookup_table_construction_PE_10000_16_14_U0  |lookup_table_construction_PE_10000_16_14  |  70312771|  70312771| 0.502 sec | 0.502 sec |  70312771|  70312771|   none  |
        |lookup_table_construction_PE_10000_16_15_U0  |lookup_table_construction_PE_10000_16_15  |  70312771|  70312771| 0.502 sec | 0.502 sec |  70312771|  70312771|   none  |
        |gather_lookup_table_10000_2_16_U0            |gather_lookup_table_10000_2_16_s          |  81920002|  81920002| 0.585 sec | 0.585 sec |  81920002|  81920002|   none  |
        |replicate_center_vectors_10000_2_U0          |replicate_center_vectors_10000_2_s        |  40960002|  40960002| 0.293 sec | 0.293 sec |  40960002|  40960002|   none  |
        |replicate_product_quantizer_2_U0             |replicate_product_quantizer_2_s           |     65538|     65538|  0.468 ms |  0.468 ms |     65538|     65538|   none  |
        |replicate_query_vectors_10000_2_U0           |replicate_query_vectors_10000_2_s         |   1280002|   1280002|  9.143 ms |  9.143 ms |   1280002|   1280002|   none  |
        +---------------------------------------------+------------------------------------------+----------+----------+-----------+-----------+----------+----------+---------+

        * Loop: 
        N/A

```

For a single PE:

total latency = query_num * ((II_load_query * D + L_load) + nprobe / PE_num * ((II1 * D + L1) + (II2 * K_star + L2)))

Here, we know that (II1 * D + L1) + (II2 * K_star + L2) = 431 CC, and the load query loop takes 128 CC.

so total latency = 10000 * (128 + 32 / 2 * 431) = 70240000, very close to real latency

Real latency (without init) is 70312771 - 32768 = 70280003

**Performance Verified on Hardware**

```
+ Latency: 
    * Summary: 
    +----------+----------+-----------+-----------+----------+----------+---------+
    |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline|
    |    min   |    max   |    min    |    max    |    min   |    max   |   Type  |
    +----------+----------+-----------+-----------+----------+----------+---------+
    |  70312771|  70312771| 0.502 sec | 0.502 sec |  70312771|  70312771|   none  |
    +----------+----------+-----------+-----------+----------+----------+---------+

    + Detail: 
        * Instance: 
        N/A

        * Loop: 
        +-------------------------------------------+----------+----------+----------+-----------+-----------+-------+----------+
        |                                           |   Latency (cycles)  | Iteration|  Initiation Interval  |  Trip |          |
        |                 Loop Name                 |    min   |    max   |  Latency |  achieved |   target  | Count | Pipelined|
        +-------------------------------------------+----------+----------+----------+-----------+-----------+-------+----------+
        |- Loop 1                                   |     32768|     32768|         2|          1|          1|  32768|    yes   |
        |- Loop 2                                   |  70280000|  70280000|      7028|          -|          -|  10000|    no    |
        | + Loop 2.1                                |       128|       128|         2|          1|          1|    128|    yes   |
        | + Loop 2.2                                |      6896|      6896|       431|          -|          -|     16|    no    |
        |  ++ residual_center_vectors_L             |       132|       132|         6|          1|          1|    128|    yes   |
        |  ++ single_row_lookup_table_construction  |       290|       290|        36|          1|          1|    256|    yes   |
        +-------------------------------------------+----------+----------+----------+-----------+-----------+-------+----------+
```

### Resource

Wrapper of 2 PEs:

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|       27|    -|
|FIFO                 |       36|     -|     5994|     2150|    -|
|Instance             |      258|  1728|   178754|   137057|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|       54|    -|
|Register             |        -|     -|        9|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |      294|  1728|   184757|   139288|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |       21|    57|       21|       32|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        7|    19|        7|       10|    0|
+---------------------+---------+------+---------+---------+-----+

+ Detail: 
    * Instance: 
    +---------------------------------------------+------------------------------------------+---------+-----+-------+-------+-----+
    |                   Instance                  |                  Module                  | BRAM_18K| DSP |   FF  |  LUT  | URAM|
    +---------------------------------------------+------------------------------------------+---------+-----+-------+-------+-----+
    |gather_lookup_table_10000_2_16_U0            |gather_lookup_table_10000_2_16_s          |        0|    0|     72|    832|    0|
    |lookup_table_construction_PE_10000_16_14_U0  |lookup_table_construction_PE_10000_16_14  |      129|  864|  89257|  67824|    0|
    |lookup_table_construction_PE_10000_16_15_U0  |lookup_table_construction_PE_10000_16_15  |      129|  864|  89256|  67815|    0|
    |replicate_center_vectors_10000_2_U0          |replicate_center_vectors_10000_2_s        |        0|    0|     68|    307|    0|
    |replicate_product_quantizer_2_U0             |replicate_product_quantizer_2_s           |        0|    0|     73|    147|    0|
    |replicate_query_vectors_10000_2_U0           |replicate_query_vectors_10000_2_s         |        0|    0|     28|    132|    0|
    +---------------------------------------------+------------------------------------------+---------+-----+-------+-------+-----+
    |Total                                        |                                          |      258| 1728| 178754| 137057|    0|
    +---------------------------------------------+------------------------------------------+---------+-----+-------+-------+-----+
```

Single PE:

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|      245|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|   864|    71824|    63989|    -|
|Memory               |      129|     -|     1024|       64|    -|
|Multiplexer          |        -|     -|        -|     3398|    -|
|Register             |        -|     -|    16409|      128|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |      129|   864|    89257|    67824|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        9|    28|       10|       15|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        3|     9|        3|        5|    0|
+---------------------+---------+------+---------+---------+-----+
```

## single_lookup_table_construction_PE_optimized_version2

Similar as optimized_version1, except the fact that a wider data type is used for the center vector stream (16 float per cycle, thus the load stage of each nprobe is as low as 128 / 16 = 8 cycles).

### Performance

Same formula as the multiple PE version.

total latency = query_num * (D * II_load_query + L_load_query + nprobe * ((L_load_centroid + N_load_centroid * II_load_centroid) + (L_compute_row + N_compute_row * II_compute_row))) =
query_num * (L_load_query + nprobe * (L_load_and_compute_row))

In this equation, L_load_query = 129, L_load_and_compute_row = 307 (including iteration count)

To verify this, use query_num = 10000 and nprobe = 32, then 10000 * (129 + 32 * 307) = 99530000 is very close to 99600000 estimated by HLS

**Performance Verified on Hardware**

```
================================================================
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  | 7.14 ns | 5.040 ns |   1.93 ns  |
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +----------+----------+-----------+-----------+----------+----------+---------+
    |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline|
    |    min   |    max   |    min    |    max    |    min   |    max   |   Type  |
    +----------+----------+-----------+-----------+----------+----------+---------+
    |  99632771|  99632771| 0.712 sec | 0.712 sec |  99632771|  99632771|   none  |
    +----------+----------+-----------+-----------+----------+----------+---------+

    + Detail: 
        * Instance: 
        N/A

        * Loop: 
        +-----------------+----------+----------+----------+-----------+-----------+-------+----------+
        |                 |   Latency (cycles)  | Iteration|  Initiation Interval  |  Trip |          |
        |    Loop Name    |    min   |    max   |  Latency |  achieved |   target  | Count | Pipelined|
        +-----------------+----------+----------+----------+-----------+-----------+-------+----------+
        |- Loop 1         |     32768|     32768|         2|          1|          1|  32768|    yes   |
        |- Loop 2         |  99600000|  99600000|      9960|          -|          -|  10000|    no    |
        | + Loop 2.1      |       128|       128|         2|          1|          1|    128|    yes   |
        | + Loop 2.2      |      9824|      9824|       307|          -|          -|     32|    no    |
        |  ++ Loop 2.2.1  |        12|        12|         6|          1|          1|      8|    yes   |
        |  ++ Loop 2.2.2  |       290|       290|        36|          1|          1|    256|    yes   |
        +-----------------+----------+----------+----------+-----------+-----------+-------+----------+
```

### Resource

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|    21732|    -|
|FIFO                 |        -|      -|        -|        -|    -|
|Instance             |        -|    864|    71824|    63856|    -|
|Memory               |      128|      -|     1024|       64|    0|
|Multiplexer          |        -|      -|        -|     3985|    -|
|Register             |        0|      -|    21551|      400|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |      128|    864|    94399|    90037|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |        9|     28|       10|       20|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |        3|      9|        3|        6|    0|
+---------------------+---------+-------+---------+---------+-----+
```

## unused: dataflow version

Although the HLS reports seem to successfully infer dataflow, the performance cannot be achieved on hardware. See "dataflow_behavior_test" folder for more details.

version 2:

more parallelism + double buffering

to construct a LUT for a single scanned cell:
* for (d in 128): load residual_vector = (query_vector - centroid_vector)
* for (k in 256): compute a row of lookup table

Optimized version 2 not only applies full parallelism in step 2, but also using double buffer to overlap the 2 steps.

### Performance 

Again, the performance is capped by the gather unit:

query_num * nprobe * K_star = 10000 * 32 * 256 = 81920000

```
+ Latency: 
    * Summary: 
    +----------+----------+-----------+-----------+----------+----------+----------+
    |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline |
    |    min   |    max   |    min    |    max    |    min   |    max   |   Type   |
    +----------+----------+-----------+-----------+----------+----------+----------+
    |  81920050|  81920050| 0.585 sec | 0.585 sec |  81920003|  81920003| dataflow |
    +----------+----------+-----------+-----------+----------+----------+----------+

    + Detail: 
        * Instance: 
        +---------------------------------------------+------------------------------------------+----------+----------+-----------+-----------+----------+----------+---------+
        |                                             |                                          |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline|
        |                   Instance                  |                  Module                  |    min   |    max   |    min    |    max    |    min   |    max   |   Type  |
        +---------------------------------------------+------------------------------------------+----------+----------+-----------+-----------+----------+----------+---------+
        |lookup_table_construction_PE_10000_16_21_U0  |lookup_table_construction_PE_10000_16_21  |  50222771|  50222771| 0.359 sec | 0.359 sec |  50222771|  50222771|   none  |
        |lookup_table_construction_PE_10000_16_22_U0  |lookup_table_construction_PE_10000_16_22  |  50222771|  50222771| 0.359 sec | 0.359 sec |  50222771|  50222771|   none  |
        |gather_lookup_table_10000_2_16_U0            |gather_lookup_table_10000_2_16_s          |  81920002|  81920002| 0.585 sec | 0.585 sec |  81920002|  81920002|   none  |
        |replicate_center_vectors_10000_2_U0          |replicate_center_vectors_10000_2_s        |  40960002|  40960002| 0.293 sec | 0.293 sec |  40960002|  40960002|   none  |
        |replicate_product_quantizer_2_U0             |replicate_product_quantizer_2_s           |     65538|     65538|  0.468 ms |  0.468 ms |     65538|     65538|   none  |
        |replicate_query_vectors_10000_2_U0           |replicate_query_vectors_10000_2_s         |   1280002|   1280002|  9.143 ms |  9.143 ms |   1280002|   1280002|   none  |
        +---------------------------------------------+------------------------------------------+----------+----------+-----------+-----------+----------+----------+---------+
```

For a single PE:


total latency = nprobe * (D * II_load_query + L_load_query + (L_dataflow + (nprobe / PE_num - 1) * L_per_loop))

In this equation, L_per_loop = 297, which equals to max(load_res, compute_LUT), where compute_LUT takes over, thus

total latency = nprobe * (D * II_load_query + L_load_query + (L_dataflow + (nprobe / PE_num - 1) * (L_compute + K_start * II_compute)))) = 10000 * (128 + (431 + 15 * 297)) = 50140000

4888 ~= 431 - 297 +  297 * 16

```
+ Latency: 
    * Summary: 
    +----------+----------+-----------+-----------+----------+----------+---------+
    |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline|
    |    min   |    max   |    min    |    max    |    min   |    max   |   Type  |
    +----------+----------+-----------+-----------+----------+----------+---------+
    |  50222771|  50222771| 0.359 sec | 0.359 sec |  50222771|  50222771|   none  |
    +----------+----------+-----------+-----------+----------+----------+---------+

    + Detail: 
        * Instance: 
        +---------------------------------------+---------------------------+---------+---------+-----------+-----------+------+------+---------+
        |                                       |                           |  Latency (cycles) |   Latency (absolute)  |   Interval  | Pipeline|
        |                Instance               |           Module          |   min   |   max   |    min    |    max    |  min |  max |   Type  |
        +---------------------------------------+---------------------------+---------+---------+-----------+-----------+------+------+---------+
        |grp_dataflow_parent_loop_proc_fu_2456  |dataflow_parent_loop_proc  |     4888|     4888| 34.915 us | 34.915 us |  4888|  4888|   none  |
        +---------------------------------------+---------------------------+---------+---------+-----------+-----------+------+------+---------+

        * Loop: 
        +-------------+----------+----------+----------+-----------+-----------+-------+----------+
        |             |   Latency (cycles)  | Iteration|  Initiation Interval  |  Trip |          |
        |  Loop Name  |    min   |    max   |  Latency |  achieved |   target  | Count | Pipelined|
        +-------------+----------+----------+----------+-----------+-----------+-------+----------+
        |- Loop 1     |     32768|     32768|         2|          1|          1|  32768|    yes   |
        |- Loop 2     |  50190000|  50190000|      5019|          -|          -|  10000|    no    |
        | + Loop 2.1  |       128|       128|         1|          -|          -|    128|    no    |
        +-------------+----------+----------+----------+-----------+-----------+-------+----------+


dataflow_parent_loop:

+ Latency: 
    * Summary: 
    +---------+---------+-----------+-----------+------+------+---------+
    |  Latency (cycles) |   Latency (absolute)  |   Interval  | Pipeline|
    |   min   |   max   |    min    |    max    |  min |  max |   Type  |
    +---------+---------+-----------+-----------+------+------+---------+
    |     4888|     4888| 34.915 us | 34.915 us |  4888|  4888|   none  |
    +---------+---------+-----------+-----------+------+------+---------+

    + Detail: 
        * Instance: 
        +-----------------------+--------------------+---------+---------+----------+----------+-----+-----+----------+
        |                       |                    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline |
        |        Instance       |       Module       |   min   |   max   |    min   |    max   | min | max |   Type   |
        +-----------------------+--------------------+---------+---------+----------+----------+-----+-----+----------+
        |dataflow_in_loop33_U0  |dataflow_in_loop33  |      431|      431| 3.079 us | 3.079 us |  297|  297| dataflow |
        +-----------------------+--------------------+---------+---------+----------+----------+-----+-----+----------+


dataflow_in_loop:

    * Summary: 
    +---------+---------+----------+----------+-----+-----+----------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline |
    |   min   |   max   |    min   |    max   | min | max |   Type   |
    +---------+---------+----------+----------+-----+-----+----------+
    |      431|      431| 3.079 us | 3.079 us |  297|  297| dataflow |
    +---------+---------+----------+----------+-----+-----+----------+

    + Detail: 
        * Instance: 
        +------------------------------------+-------------------------------+---------+---------+----------+----------+-----+-----+---------+
        |                                    |                               |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
        |              Instance              |             Module            |   min   |   max   |    min   |    max   | min | max |   Type  |
        +------------------------------------+-------------------------------+---------+---------+----------+----------+-----+-----+---------+
        |construct_single_distance_LUT_U1_1  |construct_single_distance_LUT  |      296|      296| 2.114 us | 2.114 us |  296|  296|   none  |
        |compute_residual_vector_U1_1        |compute_residual_vector        |      134|      134| 0.957 us | 0.957 us |  134|  134|   none  |
        +------------------------------------+-------------------------------+---------+---------+----------+----------+-----+-----+---------+

```

### Resource 

Wrapper of 2 PEs:

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|       27|    -|
|FIFO                 |       36|     -|     5994|     2150|    -|
|Instance             |      258|  1732|   179242|   142041|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|       54|    -|
|Register             |        -|     -|        9|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |      294|  1732|   185245|   144272|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |       21|    57|       21|       33|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        7|    19|        7|       11|    0|
+---------------------+---------+------+---------+---------+-----+
```

Wrapper of a single PE:

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|      168|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        0|   866|    89375|    65946|    -|
|Memory               |      129|     -|        0|        0|    -|
|Multiplexer          |        -|     -|        -|     4202|    -|
|Register             |        -|     -|      126|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |      129|   866|    89501|    70316|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        9|    28|       10|       16|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        3|     9|        3|        5|    0|
+---------------------+---------+------+---------+---------+-----+
```

