# Optimization Strategy

Remove the loop-carried dependency of variable "distance" by adding an array "distance_buffer" and 

Before:

```
            float distance = 0;

            // Manually unroll 16, auto-unroll doesn't work well
            for (int d = 0; d < D / 16; d++) {
#pragma HLS pipeline II=1
            //#pragma HLS UNROLL factor=16
                float scalar_dist_0 = local_query_buffer[d * 16 + 0] - cell_centroids_partition[c * D + d * 16 + 0];
                ...

                float square_dist_0 = scalar_dist_0 * scalar_dist_0;
                ...

                distance += square_dist_0 + square_dist_1 + square_dist_2 + square_dist_3 + 
                    square_dist_4 + square_dist_5 + square_dist_6 + square_dist_7 + 
                    square_dist_8 + square_dist_9 + square_dist_10 + square_dist_11 + 
                    square_dist_12 + square_dist_13 + square_dist_14 + square_dist_15; 
            }
```

After: 

```
            float distance_buffer[D / 16];
#pragma HLS array_partition variable=distance_buffer complete

            // Manually unroll 16, auto-unroll doesn't work well
            for (int d = 0; d < D / 16; d++) {
#pragma HLS pipeline II=1
            //#pragma HLS UNROLL factor=16
                float scalar_dist_0 = local_query_buffer[d * 16 + 0] - cell_centroids_partition[c * D + d * 16 + 0];
                ... 

                float square_dist_0 = scalar_dist_0 * scalar_dist_0;
                ... 

                distance_buffer[d] = square_dist_0 + square_dist_1 + square_dist_2 + square_dist_3 + 
                    square_dist_4 + square_dist_5 + square_dist_6 + square_dist_7 + 
                    square_dist_8 + square_dist_9 + square_dist_10 + square_dist_11 + 
                    square_dist_12 + square_dist_13 + square_dist_14 + square_dist_15; 
            }

            float distance = 
                distance_buffer[0] + distance_buffer[1] + distance_buffer[2] +
                distance_buffer[3] + distance_buffer[4] + distance_buffer[5] +
                distance_buffer[6] + distance_buffer[7];
```

# Performance

query num = 16384

Suppose 140 MHz

16384 / (37306371 / 140 / 1e6) = 61484 QPS (32 PE)


Before optimization:

104464387 CC / 16384 queries

Suppose 140 MHz

16384 / (104464387 / 140 / 1e6) = 21957 QPS (32 PE)

Get ~3x by removing the distance dependency (before II=3, now II=1)

# Resource 

## Resource consumption (After optimization)

Actually only needs 16 PE (~30000 QPS)

DSP | FF | LUT

124|  14420|  11329

DSP = 124 * 16 = 1,984

FF = 14420 * 16 = 230,720

LUT = 11329 * 16 = 181,264

** Even more than before optimization! **

## Resource consumption (Before optimization)

32 PE


DSP | FF | LUT

40|  7623|  5400

DSP = 40 * 32 = 1,280

FF = 7623 * 32 = 243,936

LUT = 5400 * 32 = 172,800


# Report


```
+ Latency:
    * Summary:
    +----------+----------+-----------+-----------+----------+----------+----------+
    |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline |
    |    min   |    max   |    min    |    max    |    min   |    max   |   Type   |
    +----------+----------+-----------+-----------+----------+----------+----------+
    |  39403402|  39403402| 0.281 sec | 0.281 sec |  37306372|  37306372| dataflow |
    +----------+----------+-----------+-----------+----------+----------+----------+

    + Detail:
        * Instance:
        +------------------------------------+---------------------------------+----------+----------+-----------+-----------+----------+----------+---------+
        |                                    |                                 |   Latency (cycles)  |   Latency (absolute)  |       Interval      | Pipeline|
        |              Instance              |              Module             |    min   |    max   |    min    |    max    |    min   |    max   |   Type  |
        +------------------------------------+---------------------------------+----------+----------+-----------+-----------+----------+----------+---------+
        |compute_cell_distance_16384_U0      |compute_cell_distance_16384_s    |  37306371|  37306371| 0.266 sec | 0.266 sec |  37306371|  37306371|   none  |
        |write_result_4194304_U0             |write_result_4194304_s           |   4194312|   4194312| 29.960 ms | 29.960 ms |   4194312|   4194312|   none  |
        |broadcast_query_vector_16384_U0     |broadcast_query_vector_16384_s   |   2097162|   2097162| 14.980 ms | 14.980 ms |   2097162|   2097162|   none  |
        |broadcast_init_centroid_vectors_U0  |broadcast_init_centroid_vectors  |     32778|     32778|  0.234 ms |  0.234 ms |     32778|     32778|   none  |
        |vadd_entry10_U0                     |vadd_entry10                     |         0|         0|    0 ns   |    0 ns   |         0|         0|   none  |
        +------------------------------------+---------------------------------+----------+----------+-----------+-----------+----------+----------+---------+
```

```
* Summary:
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|       37|    -|
|FIFO                 |        2|      -|       84|      421|    -|
|Instance             |        6|    124|    18835|    18222|    8|
|Memory               |        -|      -|        -|        -|    -|
|Multiplexer          |        -|      -|        -|       54|    -|
|Register             |        -|      -|       15|        -|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |        8|    124|    18934|    18734|    8|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |      4|        2|        4|    2|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |    ~0   |      1|    ~0   |        1|  ~0 |
+---------------------+---------+-------+---------+---------+-----+

+ Detail:
    * Instance:
    +------------------------------------+---------------------------------+---------+-------+-------+-------+-----+
    |              Instance              |              Module             | BRAM_18K| DSP48E|   FF  |  LUT  | URAM|
    +------------------------------------+---------------------------------+---------+-------+-------+-------+-----+
    |broadcast_init_centroid_vectors_U0  |broadcast_init_centroid_vectors  |        0|      0|    127|    189|    0|
    |broadcast_query_vector_16384_U0     |broadcast_query_vector_16384_s   |        0|      0|    132|    186|    0|
    |compute_cell_distance_16384_U0      |compute_cell_distance_16384_s    |        0|    124|  14420|  11329|    8|
    |vadd_control_s_axi_U                |vadd_control_s_axi               |        0|      0|   2486|   4520|    0|
    |vadd_entry10_U0                     |vadd_entry10                     |        0|      0|      3|     55|    0|
    |vadd_gmem32_m_axi_U                 |vadd_gmem32_m_axi                |        2|      0|    512|    580|    0|
    |vadd_gmem33_m_axi_U                 |vadd_gmem33_m_axi                |        2|      0|    512|    580|    0|
    |vadd_gmem34_m_axi_U                 |vadd_gmem34_m_axi                |        2|      0|    512|    580|    0|
    |write_result_4194304_U0             |write_result_4194304_s           |        0|      0|    131|    203|    0|
    +------------------------------------+---------------------------------+---------+-------+-------+-------+-----+
    |Total                               |                                 |        6|    124|  18835|  18222|    8|
    +------------------------------------+---------------------------------+---------+-------+-------+-------+-----+

```