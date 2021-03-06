# Performance

## Real Performance 

715.926 ms

140 MHz

10000 query * 10000 iterations per query

Theoretical performance = 1e4 * 1e4 / (140 * 1e6) = 0.71428 s

Achieved 714.28 / 715.926 = 99.770% theoretical performance.


## HLS Performance

N/A runtime dependent

# Resource

Including control logic loop:

very close to fixed scan per query version

FF = 17907 (fixed) | 17982 (unfixed)

LUT = 26840 (fixed) | 27777 (unfixed)

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|       16|    -|
|FIFO                 |        0|      -|      605|     4576|    -|
|Instance             |        4|      0|    22516|    34946|    -|
|Memory               |        -|      -|        -|        -|    -|
|Multiplexer          |        -|      -|        -|        -|    -|
|Register             |        -|      -|        6|        -|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |        4|      0|    23127|    39538|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |      0|        2|        9|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |    ~0   |      0|    ~0   |        3|    0|
+---------------------+---------+-------+---------+---------+-----+

+ Detail: 
    * Instance: 
    +------------------------------------+---------------------------------+---------+-------+-------+-------+-----+
    |              Instance              |              Module             | BRAM_18K| DSP48E|   FF  |  LUT  | URAM|
    +------------------------------------+---------------------------------+---------+-------+-------+-------+-----+
    |control_signal_sender_10000_4_U0    |control_signal_sender_10000_4    |        0|      0|     22|    153|    0|
    |dummy_input_sender_10000_35_U0      |dummy_input_sender_10000_35      |        0|      0|     99|    496|    0|
    |dummy_input_sender_10000_36_U0      |dummy_input_sender_10000_36      |        0|      0|     99|    496|    0|
    |parallel_merge_sort_16_10000_37_U0  |parallel_merge_sort_16_10000_37  |        0|      0|  17982|  27777|    0|
    |vadd_control_s_axi_U                |vadd_control_s_axi               |        0|      0|   2486|   4520|    0|
    |vadd_gmem34_m_axi_U                 |vadd_gmem34_m_axi                |        4|      0|    566|    766|    0|
    |write_result_10000_38_U0            |write_result_10000_38            |        0|      0|   1262|    738|    0|
    +------------------------------------+---------------------------------+---------+-------+-------+-------+-----+
    |Total                               |                                 |        4|      0|  22516|  34946|    0|
    +------------------------------------+---------------------------------+---------+-------+-------+-------+-----+

```

