# distance estimation PE (by LUT addition)

There are two options offered for the users: with / without double buffering the distance LUT.

Option 1: without double buffering

Resource consumption refer to distance_estimation_by_LUT_systolic

For performance estimation, refer to the wrapper version (distance_estimation_by_LUT_wrapper_systolic) since there's propagation delay between PEs.

Option 2: with double buffering

## distance_estimation_by_LUT_systolic (fixed iteration number)

Used for performance estimation.

```
        * Loop: 
        +-------------+-----------+-----------+----------+-----------+-----------+--------+----------+
        |             |    Latency (cycles)   | Iteration|  Initiation Interval  |  Trip  |          |
        |  Loop Name  |    min    |    max    |  Latency |  achieved |   target  |  Count | Pipelined|
        +-------------+-----------+-----------+----------+-----------+-----------+--------+----------+
        |- Loop 1     |  164800000|  164800000|       515|          -|          -|  320000|    no    |
        | + Loop 1.1  |        256|        256|         2|          1|          1|     256|    yes   |
        | + Loop 1.2  |        254|        254|        63|          1|          1|     193|    yes   |
        +-------------+-----------+-----------+----------+-----------+-----------+--------+----------+
```

Total time = query_num * nprobe * ((L_load + N_load * II_compute) + (L_compute + N_compute * II_compute))

here, L_load = 2, N_load = 256, L_compute = 63

Take N_compute = 193, nprobe = 32 and query_num = 10000

10000 * 32 * ((2 + 256) + (63 + 193)) = 164480000 (very close to HLS estimation)

## distance_estimation_by_LUT_systolic 

The unfixed scan per iteration version, used as single PE resource estimation.

### Resource Consumption

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|      288|    -|
|FIFO                 |        -|      -|        -|        -|    -|
|Instance             |        -|     30|     3465|     3375|    -|
|Memory               |       16|      -|        0|        0|    0|
|Multiplexer          |        -|      -|        -|      842|    -|
|Register             |        0|      -|     1972|      824|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |       16|     30|     5437|     5329|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |        1|   ~0  |    ~0   |        1|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |    ~0   |   ~0  |    ~0   |    ~0   |    0|
+---------------------+---------+-------+---------+---------+-----+
```

## distance_estimation_by_LUT_wrapper_systolic

### Performance 

Single PE + delay

Total time = query_num * nprobe * ((L_load + N_load * II_load + PE_num - 1) + (L_compute + N_compute * II_compute))

here, L_load = 2, N_load = 256, L_compute = 63, PE_num = 63

Take N_compute = 193, nprobe = 32 and query_num = 10000

10000 * 32 * ((2 + 256 + 63) + (62 + 193)) = 184320000 cycles = 184320000 / 140 / 1e6 = 1.317 s

Real Performance: 1306.35 ms (140MHz, suppose scanning 193 entries per Voronoi cell)

And seems random DRAM access does not count (hide by the pipeline).

**Performance verified on hardware**

### Resource Utilization

**Should use 63 * single PE instead of using this for integrating the entire accelerator.**

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|      482|    -|
|FIFO                 |        0|      -|     6804|    51030|    -|
|Instance             |     1008|   1890|   342727|   324084|    0|
|Memory               |        -|      -|        -|        -|    -|
|Multiplexer          |        -|      -|        -|     1188|    -|
|Register             |        -|      -|      198|        -|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |     1008|   1890|   349729|   376784|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |       75|     62|       40|       86|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |       25|     20|       13|       28|    0|
+---------------------+---------+-------+---------+---------+-----+
```

## distance_estimation_by_LUT_systolic_fixed_iteration_manual_double_buffering

Correct version: control logic guarantees no bank conflict, allowing II=1

```
buffer_A, buffer_B;

// Note! Use the if else clause such that A and B certainly
//   won't have bank conflict in a single CC
for (int nprobe_id = 0; nprobe_id < nprobe + 1; nprobe_id++) {

    max_iter = max(load_iter, comp_iter);

    for (int common_iter = 0; common_iter < max_iter; common_iter++) {
#pragma HLS pipeline II=1

        // even: load to buffer A, compute using B
        if (nprobe_id even) {

            // load
            if (nprobe_id < nprobe) { // last iter not read 
                if (common_iter < load_iter) {
                    load to buffer A;
                }
            }

            // compute
            if (nprobe_id > 0) {
                if (common_iter < comp_iter) { // first iter not compute
                    
                    compute using buffer B;
                        
                    if (common_iter = last compute iter) {
                        check padding
                    }
                }
            }
        }
        // odd: load to buffer B, compute using A
        else {
            // load
            if (nprobe_id < nprobe) { // last iter not read 
                if (common_iter < load_iter) {
                    load to buffer B;
                }
            }

            // compute
            if (nprobe_id > 0) {
                if (common_iter < comp_iter) { // first iter not compute
                    
                    compute using buffer A;
                        
                    if (common_iter = last compute iter) {
                        check padding
                    }
                }
            }
        }
    }
}
```

Wrong: cannot infer II=1 due to the potential of bank conflict on a single buffer

```
buffer_A, buffer_B;

for (int nprobe_id = 0; nprobe_id < nprobe + 1; nprobe_id++) {

    max_iter = max(load_iter, comp_iter);

    for (int common_iter = 0; common_iter < max_iter; common_iter++) {
#pragma HLS pipeline II=1

        // load
        if (nprobe_id < nprobe) { // last iter not read 
            if (common_iter < load_iter) {
                if (nprobe_id even) {
                    load to buffer A;
                }
                else {
                    load to buffer B;
                }
            }
        }

        // compute
        if (nprobe_id > 0) {
            if (common_iter < comp_iter) { // first iter not compute
                if (nprobe_id odd) {
                    compute using buffer A;
                }
                else {
                    compute using buffer B;
                }
                if (common_iter = last compute iter) {
                    check padding
                }
            }
        }
    }
}

```

### Performance

Total time = query_num * (L_load_first_iter + (nprobe - 1) * (L_load_compute + max(K, scan_per_query) * II_load_compute) + (L_load_compute + N_last_iter * II))

There are nprobe + 1 iterations by using double buffering. The first iteration only has read (256 CC), while the last iteration only has computation (L_load_compute + N_last_iter * II), where statistically N_last_iter = N_db_vectors / nlist / PE_num, e.g., 193 given 100M vector and 8192 cells and 63 PEs.

L_load_first_iter = 256

L_load_compute = 63

K = 256

scan_per_query depends on data distribution in the index.

N_last_iter = N_db_vectors / nlist / PE_numss

For example, given scan_per_query = 193, query num = 10000, nprobe = 32

total time = 10000 * (256 + 31 * (63 + 256) + 63 + 193) = 104010000 CC = 0.7429 sec

Here the HLS reports is not accurate since it does not consider the first and last iterations.

```
        * Loop: 
        +-------------+-----------+-----------+----------+-----------+-----------+--------+----------+
        |             |    Latency (cycles)   | Iteration|  Initiation Interval  |  Trip  |          |
        |  Loop Name  |    min    |    max    |  Latency |  achieved |   target  |  Count | Pipelined|
        +-------------+-----------+-----------+----------+-----------+-----------+--------+----------+
        |- Loop 1     |  105600000|  105600000|       320|          -|          -|  330000|    no    |
        | + Loop 1.1  |        317|        317|        63|          1|          1|     256|    yes   |
        +-------------+-----------+-----------+----------+-----------+-----------+--------+----------+
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
|Expression           |        -|      -|        0|      409|    -|
|FIFO                 |        -|      -|        -|        -|    -|
|Instance             |        -|     30|     3465|     3375|    -|
|Memory               |       32|      -|        0|        0|    0|
|Multiplexer          |        -|      -|        -|     1326|    -|
|Register             |        0|      -|     3794|     1696|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |       32|     30|     7259|     6806|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |        2|   ~0  |    ~0   |        1|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |    ~0   |   ~0  |    ~0   |    ~0   |    0|
+---------------------+---------+-------+---------+---------+-----+

```

## distance_estimation_by_LUT_systolic_manual_double_buffering

### Performance (Same as the fixed iteration version)

Note that scan_per_query is a variable (depends on data distribution) and need to be averaged.

Total time = query_num * (L_load_first_iter + (nprobe - 1) * (L_load_compute + max(K, scan_per_query) * II_load_compute) + (L_load_compute + N_last_iter * II))

There are nprobe + 1 iterations by using double buffering. The first iteration only has read (256 CC), while the last iteration only has computation (L_load_compute + N_last_iter * II), where statistically N_last_iter = N_db_vectors / nlist / PE_num, e.g., 193 given 100M vector and 8192 cells and 63 PEs.

L_load_first_iter = 256

L_load_compute = 63

K = 256

scan_per_query depends on data distribution in the index.

N_last_iter = N_db_vectors / nlist / PE_numss

For example, given scan_per_query = 193, query num = 10000, nprobe = 32

total time = 10000 * (256 + 31 * (63 + 256) + 63 + 193) = 104010000 CC = 0.7429 sec

Here the HLS reports is not accurate since it does not consider the first and last iterations.

### Resource Consumption

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+-------+---------+---------+-----+
|         Name        | BRAM_18K| DSP48E|    FF   |   LUT   | URAM|
+---------------------+---------+-------+---------+---------+-----+
|DSP                  |        -|      -|        -|        -|    -|
|Expression           |        -|      -|        0|      559|    -|
|FIFO                 |        -|      -|        -|        -|    -|
|Instance             |        -|     30|     3465|     3375|    -|
|Memory               |       32|      -|        0|        0|    0|
|Multiplexer          |        -|      -|        -|     1359|    -|
|Register             |        0|      -|     3935|     1696|    -|
+---------------------+---------+-------+---------+---------+-----+
|Total                |       32|     30|     7400|     6989|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available SLR        |     1344|   3008|   869120|   434560|  320|
+---------------------+---------+-------+---------+---------+-----+
|Utilization SLR (%)  |        2|   ~0  |    ~0   |        1|    0|
+---------------------+---------+-------+---------+---------+-----+
|Available            |     4032|   9024|  2607360|  1303680|  960|
+---------------------+---------+-------+---------+---------+-----+
|Utilization (%)      |    ~0   |   ~0  |    ~0   |    ~0   |    0|
+---------------------+---------+-------+---------+---------+-----+
```

## Unused Version

There are two major stages for each nprobe: load distance LUT, and computation by LUT. Although dataflow should be able to cover the distance LUT loading, Vitis HLS fail to synthesize hardware that can double buffer the LUT under complex control flow (see "dataflow_behavior_test" for more details).

### distance_estimation_by_LUT_wrapper_systolic_double_buffer

Real Performance: 1357.5 ms (135MHz, suppose scanning 193 entries per Voronoi cell), this performance is exactly the same as without double buffer given the same frequency...
