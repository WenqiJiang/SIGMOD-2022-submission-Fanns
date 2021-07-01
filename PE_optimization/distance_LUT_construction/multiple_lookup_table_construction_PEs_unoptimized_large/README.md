# Single Lookup Table Construction PE

see "lookup_table_construction_PE.png" for details.

# Performance

This project tests the performance of a single table construction PE.

We use QUERY_NUM = 1024, and each query needs NPROBE = 32 tables, thus 1024 * 32= 32768 tables in total.

The core function is "lookup_table_construction", which construct table row by row.

Each row takes II=4 cycles, bound by BRAM ports.

There are K=256 rows in total

As in report, finish computing one table takes 1067 Cycles. 

Considering loading residual vector (133 CC), construct each table takes 1200 CC.

Every query needs additional 128 CCs to load query vector.

Thus, modeling the performance of the model can be

((1067 + 133) * NPROBE + 128) * QUERY_NUM

which is (1200 * 32 + 128) * 1024 = 39,452,672 CC

The real vivado HLS CC = 39,751,680 (PQ quantizer initialization still costs).

Throughput = 1024 / (39,751,680 * 5 * 1e-9) = 5151 (Suppose 200 MHz)

Desired Performance: at least 10,000

# Performance Estimation of multi-PEs

```
#define PE_NUM_TABLE_CONSTRUCTION 4
#define NPROBE_PER_TABLE_CONSTRUCTION_PE (NPROBE / PE_NUM_TABLE_CONSTRUCTION) // 8
```

((1067 + 133) * NPROBE_PER_TABLE_CONSTRUCTION_PE + 128) * QUERY_NUM

which is (1200 * 8 + 128) * 1024 = 9,961,472 CC

Throughput = 1024 / (9,961,472 * 5 * 1e-9) = 20559 (Suppose 200 MHz)