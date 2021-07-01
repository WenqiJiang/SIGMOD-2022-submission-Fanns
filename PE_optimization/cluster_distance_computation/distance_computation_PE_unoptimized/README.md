```
define NLIST 8192
#define NPROBE 32
#define D 128
#define M 16
#define K 256
#define CENTROID_PARTITIONS 32
#define CENTROIDS_PER_PARTITION (NLIST / CENTROID_PARTITIONS) // 256
#define QUERY_NUM 1024
```

Expected: 256 elements * 8 CC per element * 1024 queries = 2,097,152 CC

What I got: 6,539,267 (II=3)

6,539,267 / (2048 * 1024) = 3.118

6386 CC per query

32 PE Throughput: 1 / (6,539,267/1024 * 5 * 1e-9) = 31318

Wenqi: for this throughput, I can definitely use Priority Queue, it only takes 2 CC per insertion.

```
* Summary: 
    + Detail: 
        * Instance: 
        +------------------------------------+---------------------------------+---------+---------+-----------+-----------+---------+---------+---------+
        |                                    |                                 |  Latency (cycles) |   Latency (absolute)  |      Interval     | Pipeline|
        |              Instance              |              Module             |   min   |   max   |    min    |    max    |   min   |   max   |   Type  |
        +------------------------------------+---------------------------------+---------+---------+-----------+-----------+---------+---------+---------+
        |compute_cell_distance_1024_U0       |compute_cell_distance_1024_s     |  6539267|  6539267| 46.710 ms | 46.710 ms |  6539267|  6539267|   none  |
        |write_result_262144_U0              |write_result_262144_s            |   262152|   262152|  1.873 ms |  1.873 ms |   262152|   262152|   none  |
        |broadcast_query_vector_1024_U0      |broadcast_query_vector_1024_s    |   131082|   131082|  0.936 ms |  0.936 ms |   131082|   131082|   none  |
        |broadcast_init_centroid_vectors_U0  |broadcast_init_centroid_vectors  |    32778|    32778|  0.234 ms |  0.234 ms |    32778|    32778|   none  |
        |vadd_entry10_U0                     |vadd_entry10                     |        0|        0|    0 ns   |    0 ns   |        0|        0|   none  |
        +------------------------------------+---------------------------------+---------+---------+-----------+-----------+---------+---------+---------+

================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|       37|    -|
|FIFO                 |        2|      -|       79|      377|    -|
|Instance             |        6|     40|    11695|    12330|    8|
|Memory               |        -|      -|        -|        -|    -|
|Multiplexer          |        -|      -|        -|       54|    -|
|Register             |        -|      -|       15|        -|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |        8|     40|    11789|    12798|    8|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |      1|        1|        2|    2|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |    ~0   |   ~0  |    ~0   |    ~0   |  ~0 |
+---------------------+---------+-------+---------+---------+-----+

+ Detail: 
    * Instance: 
    +------------------------------------+---------------------------------+---------+-------+------+------+-----+
    |              Instance              |              Module             | BRAM_18K| DSP48E|  FF  |  LUT | URAM|
    +------------------------------------+---------------------------------+---------+-------+------+------+-----+
    |broadcast_init_centroid_vectors_U0  |broadcast_init_centroid_vectors  |        0|      0|   127|   189|    0|
    |broadcast_query_vector_1024_U0      |broadcast_query_vector_1024_s    |        0|      0|   128|   182|    0|
    |compute_cell_distance_1024_U0       |compute_cell_distance_1024_s     |        0|     40|  7288|  5456|    8|
    |vadd_control_s_axi_U                |vadd_control_s_axi               |        0|      0|  2486|  4520|    0|
    |vadd_entry10_U0                     |vadd_entry10                     |        0|      0|     3|    55|    0|
    |vadd_gmem32_m_axi_U                 |vadd_gmem32_m_axi                |        2|      0|   512|   580|    0|
    |vadd_gmem33_m_axi_U                 |vadd_gmem33_m_axi                |        2|      0|   512|   580|    0|
    |vadd_gmem34_m_axi_U                 |vadd_gmem34_m_axi                |        2|      0|   512|   580|    0|
    |write_result_262144_U0              |write_result_262144_s            |        0|      0|   127|   188|    0|
    +------------------------------------+---------------------------------+---------+-------+------+------+-----+
    |Total                               |                                 |        6|     40| 11695| 12330|    8|
    +------------------------------------+---------------------------------+---------+-------+------+------+-----+

```

```
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'compute_cell_distance_1024_s' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'Loop 1'.
INFO: [SCHED 204-61] Pipelining result : Target II = 1, Final II = 1, Depth = 2.
INFO: [SCHED 204-61] Pipelining loop 'Loop 2.1'.
INFO: [SCHED 204-61] Pipelining result : Target II = 1, Final II = 1, Depth = 2.
INFO: [SCHED 204-61] Pipelining loop 'Loop 2.2'.
WARNING: [SCHED 204-68] The II Violation in module 'compute_cell_distance_1024_s' (Loop: Loop 2.2): Unable to enforce a carried dependence constraint (II = 1, distance = 1, offset = 0)
   between 'fadd' operation ('distance', /mnt/scratch/wenqi/FPGA-ANNS/node1_building_blocks/distance_computation_PE/src/vadd.cpp:234) and 'select' operation ('select_ln197', /mnt/scratch/wenqi/FPGA-ANNS/node1_building_blocks/distance_computation_PE/src/vadd.cpp:197).
WARNING: [SCHED 204-68] The II Violation in module 'compute_cell_distance_1024_s' (Loop: Loop 2.2): Unable to enforce a carried dependence constraint (II = 2, distance = 1, offset = 0)
   between 'fadd' operation ('distance', /mnt/scratch/wenqi/FPGA-ANNS/node1_building_blocks/distance_computation_PE/src/vadd.cpp:234) and 'select' operation ('select_ln197', /mnt/scratch/wenqi/FPGA-ANNS/node1_building_blocks/distance_computation_PE/src/vadd.cpp:197).
INFO: [SCHED 204-61] Pipelining result : Target II = 1, Final II = 3, Depth = 81.
WARNING: [SCHED 204-21] Estimated clock period (5.783ns) exceeds the target (target clock period: 7.143ns, clock uncertainty: 1.929ns, effective delay budget: 5.214ns).
WARNING: [SCHED 204-21] The critical path in module 'compute_cell_distance_1024_s' consists of the following:
	'fadd' operation ('distance', /mnt/scratch/wenqi/FPGA-ANNS/node1_building_blocks/distance_computation_PE/src/vadd.cpp:234) [284]  (2.78 ns)
	'phi' operation ('tmp') with incoming values : ('distance', /mnt/scratch/wenqi/FPGA-ANNS/node1_building_blocks/distance_computation_PE/src/vadd.cpp:234) [149]  (0 ns)
	'select' operation ('select_ln197', /mnt/scratch/wenqi/FPGA-ANNS/node1_building_blocks/distance_computation_PE/src/vadd.cpp:197) [157]  (0.227 ns)
	'fadd' operation ('distance', /mnt/scratch/wenqi/FPGA-ANNS/node1_building_blocks/distance_computation_PE/src/vadd.cpp:234) [284]  (2.78 ns)
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111]  Elapsed time: 0.61 seconds; current allocated memory: 169.349 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
WARNING: [BIND 205-102] The specified resource core for memory 'cell_centroids_partition_0' will be ignored if a simpler one can be used.
WARNING: [BIND 205-102] The specified resource core for memory 'cell_centroids_partition_1' will be ignored if a simpler one can be used.
WARNING: [BIND 205-102] The specified resource core for memory 'cell_centroids_partition_2' will be ignored if a simpler one can be used.
WARNING: [BIND 205-102] The specified resource core for memory 'cell_centroids_partition_3' will be ignored if a simpler one can be used.
WARNING: [BIND 205-102] The specified resource core for memory 'cell_centroids_partition_4' will be ignored if a simpler one can be used.
WARNING: [BIND 205-102] The specified resource core for memory 'cell_centroids_partition_5' will be ignored if a simpler one can be used.
WARNING: [BIND 205-102] The specified resource core for memory 'cell_centroids_partition_6' will be ignored if a simpler one can be used.
WARNING: [BIND 205-102] The specified resource core for memory 'cell_centroids_partition_7' will be ignored if a simpler one can be used.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111]  Elapsed time: 0.56 seconds; current allocated memory: 171.156 MB.
```