# Priority Queue

## Performance Modeling

* For a single queue

Total time = query_num * (L_control + (L_insertion + N_insertion * II_insertion) + (L_output + N_output * II_output))

Let's assume L_control = 3 CC, the HLS shows that L_insertion=5, II_insertion=2, L_output=2, II_output=1

* For a group of queue

In our design, we use a group of queue of 2 hierachies, the first one contains 20 queues, while the second one only contains one to gather all results. As long as the FIFO between two blocks is long enough, the two level can be pipelined. Such that the total time consumption can still is equal to one single queue. Thus we can still use the formula above.

Real Performance (query num = 10000, iteration per query = 10000, such that for the first level of wrapper, each queue is only responsible for half of the query) = 717.15 ms (140 MHz)

10000 * (3 + (5 + 10000 * 2) + (2 + 10 * 1)) / 2 = 100100000 CC = 100100000 / 140 / 1e6 = 0.715 s 

**Performance Verified on Real Hardware**

## Resource Modeling (Unfixed Length)

Given the fixed length result, len=100 is almost 10X expensive compared with len=10, thus we can use linear model to estimate the priority queue resource consumption.

### Single Priority Queue, length = 10 (Vitis Build Flow)

FF = 2177 

LUT = 3597

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|     2583|    -|
|FIFO                 |        -|      -|        -|        -|    -|
|Instance             |        -|      0|      330|      340|    -|
|Memory               |        -|      -|        -|        -|    -|
|Multiplexer          |        -|      -|        -|      674|    -|
|Register             |        -|      -|     1847|        -|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |        0|      0|     2177|     3597|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |        0|      0|    ~0   |    ~0   |    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |        0|      0|    ~0   |    ~0   |    0|
+---------------------+---------+-------+---------+---------+-----+

```

### Priority Queue Wrapper, length = 10

**We shouldn't use this in the accelerator integration model, should use 21 * queue as the estimated resource instead.**

folder: priority_queue_wrapper_unfixed

Only consumes 6% LUT. 

FF = 53224

LUT = 85989

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|       20|    -|
|FIFO                 |        2|     -|    10158|     6723|    -|
|Instance             |        -|     -|    43060|    79210|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|       36|    -|
|Register             |        -|     -|        6|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        2|     0|    53224|    85989|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |     0|        6|       19|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |    ~0   |     0|        2|        6|    0|
+---------------------+---------+------+---------+---------+-----+

+ Detail: 
    * Instance: 
    +-------------------------------------------------+----------------------------------------------+---------+----+------+------+-----+
    |                     Instance                    |                    Module                    | BRAM_18K| DSP|  FF  |  LUT | URAM|
    +-------------------------------------------------+----------------------------------------------+---------+----+------+------+-----+
    |consume_and_redirect_sorted_streams_10000_66_U0  |consume_and_redirect_sorted_streams_10000_66  |        0|   0|  4201|  7740|    0|
    |insert_wrapper_10000_189_U0                      |insert_wrapper_10000_189                      |        0|   0|  1849|  3337|    0|
    |insert_wrapper_10000_4567_U0                     |insert_wrapper_10000_4567                     |        0|   0|  1848|  3368|    0|
    |insert_wrapper_10000_4668_U0                     |insert_wrapper_10000_4668                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_4769_U0                     |insert_wrapper_10000_4769                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_4870_U0                     |insert_wrapper_10000_4870                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_4971_U0                     |insert_wrapper_10000_4971                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5072_U0                     |insert_wrapper_10000_5072                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5173_U0                     |insert_wrapper_10000_5173                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5274_U0                     |insert_wrapper_10000_5274                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5375_U0                     |insert_wrapper_10000_5375                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5476_U0                     |insert_wrapper_10000_5476                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5577_U0                     |insert_wrapper_10000_5577                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5678_U0                     |insert_wrapper_10000_5678                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5779_U0                     |insert_wrapper_10000_5779                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5880_U0                     |insert_wrapper_10000_5880                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_5981_U0                     |insert_wrapper_10000_5981                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_6082_U0                     |insert_wrapper_10000_6082                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_6183_U0                     |insert_wrapper_10000_6183                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_6284_U0                     |insert_wrapper_10000_6284                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_6385_U0                     |insert_wrapper_10000_6385                     |        0|   0|  1847|  3359|    0|
    |insert_wrapper_10000_6486_U0                     |insert_wrapper_10000_6486                     |        0|   0|  1847|  3359|    0|
    |merge_streams_10000_10_87_U0                     |merge_streams_10000_10_87                     |        0|   0|    47|   835|    0|
    |send_iter_num_10000_200_88_U0                    |send_iter_num_10000_200_88                    |        0|   0|    22|   109|    0|
    +-------------------------------------------------+----------------------------------------------+---------+----+------+------+-----+
    |Total                                            |                                              |        0|   0| 43060| 79210|    0|
    +-------------------------------------------------+----------------------------------------------+---------+----+------+------+-----+
```

## Fixed Length

### Single Priority Queue (length = 10)

folder: priority_queue_wrapper_unfixed

Only consumes 3337 LUTs (~0.3% of all).

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|     2520|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|     -|        0|      161|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|      656|    -|
|Register             |        -|     -|     1849|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        0|     0|     1849|     3337|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        0|     0|    ~0   |    ~0   |    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        0|     0|    ~0   |    ~0   |    0|
+---------------------+---------+------+---------+---------+-----+
```

### Single Priority Queue (length = 100)

folder: priority_queue_distance_results_fixed_len100

Almost 10X expensive compared with len=10, thus we can use linear model to estimate the priority queue resource consumption.

This consumes 32377 LUTs (~2.5% of all). As a result, if we have topK=100, then for each HBM channel we need 6 priority queues of length 100. Suppose 21 used HBM channels, that is 21 * 6 * 32377 = 4079502 LUTs, 3 times of all LUT resources. Thus topK=100 is impossible.

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|    26095|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|     -|        0|     1045|    -|
|Memory               |        -|     -|        -|        -|    -|
|Multiplexer          |        -|     -|        -|     5237|    -|
|Register             |        -|     -|    16291|        -|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        0|     0|    16291|    32377|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |        0|     0|        1|        7|    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |        0|     0|    ~0   |        2|    0|
+---------------------+---------+------+---------+---------+-----+
```

