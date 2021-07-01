# BRAM behavior test

This folder tests the resource consumption & performance of BRAM, given different data type, array depth, and port number.

Vitis HLS version: 2019.2

## Key takeways

* For float / int, use BRAM_1P
  * for some reason, by using 2 BRAM_2P, it only allows 1 write / CC or 2 reads / CC
  * on the other hand, use 2 BRAM_1P allows 2 writes / CC or 2 reads / CC

## ap_uint<64> 2 ports

```
    ap_uint<64> bram_array[bram_depth];
#pragma HLS resource variable=bram_array core=RAM_2P_BRAM

    for (int i = 0; i < bram_depth; i++) {
        float tmp_32_A_float = table_HBM0[2 * i];
        float tmp_32_B_float = table_HBM0[2 * i + 1];
        ap_uint<32> tmp_32_A_ap = *((ap_uint<32>*) (&tmp_32_A_float));
        ap_uint<32> tmp_32_B_ap = *((ap_uint<32>*) (&tmp_32_B_float));
        bram_array[i].range(31, 0) = tmp_32_A_ap;
        bram_array[i].range(63, 32) = tmp_32_B_ap; 
    }

    ap_uint<64> write_val_A = bram_array[bram_depth - 1];
    ap_uint<64> write_val_B = bram_array[bram_depth - 1];
    write_loop:
    for (int i = 0; i < bram_depth - 1; i++) {
#pragma HLS pipeline II=1
        bram_array[i] = write_val_A;
        bram_array[i + 1] = write_val_B;
    }

    ap_uint<64> read_val_A;
    ap_uint<64> read_val_B;
    read_loop:
    for (int i = 0; i < bram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = bram_array[i];
        read_val_B = bram_array[bram_depth - 1 - i];
    }
```

### Performance 

* performance: 1 write / CC; 2 read / CC

```
        * Loop: 
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |              |  Latency (cycles) | Iteration|  Initiation Interval  | Trip |          |
        |   Loop Name  |   min   |   max   |  Latency |  achieved |   target  | Count| Pipelined|
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |- Loop 1      |       33|       33|         4|          2|          1|    16|    yes   |
        |- write_loop  |       30|       30|         2|          2|          1|    15|    yes   |
        |- read_loop   |       16|       16|         2|          1|          1|    16|    yes   |
        |- Loop 4      |       16|       16|         2|          1|          1|    16|    yes   |
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
```

### Resource / Depth

* Use small array -> 4 BRAM

bram_depth = 16

Each BRAM slice has a width of 16bit, use 4 slices to form a 64-bit width array.

```
    * Memory: 
    +----------------+-----------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |     Memory     |               Module              | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +----------------+-----------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_V_U  |bram_ap_uint_64_16_s_bram_array_V  |        4|  0|   0|    0|    16|   64|     1|         1024|
    +----------------+-----------------------------------+---------+---+----+-----+------+-----+------+-------------+

```

* Use full 16Kbit (depth=1024) -> 4 BRAM

```
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |     Memory     |                   Module                   | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_V_U  |bram_ap_uint_64_2ports_1024_s_bram_array_V  |        4|  0|   0|    0|  1024|   64|     1|        65536|
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
```

* Slightly more than 16Kbit (depth=1025) -> 8 BRAM

```
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |     Memory     |                   Module                   | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_V_U  |bram_ap_uint_64_2ports_1025_s_bram_array_V  |        8|  0|   0|    0|  1025|   64|     1|        65600|
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
```

* 2 * 1024 = depth 2048 -> 8 BRAM

```
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |     Memory     |                   Module                   | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_V_U  |bram_ap_uint_64_2ports_2048_s_bram_array_V  |        8|  0|   0|    0|  2048|   64|     1|       131072|
    +----------------+--------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
```

## ap_uint<64> 1 port

```
template<const int bram_depth>
void bram_ap_uint_64_1port(
    const float* table_HBM0,
    float* out_HBM) {
#pragma HLS inline off    

    ap_uint<64> bram_array[bram_depth];
#pragma HLS resource variable=bram_array core=RAM_1P_BRAM

    for (int i = 0; i < bram_depth; i++) {
        float tmp_32_A_float = table_HBM0[2 * i];
        float tmp_32_B_float = table_HBM0[2 * i + 1];
        ap_uint<32> tmp_32_A_ap = *((ap_uint<32>*) (&tmp_32_A_float));
        ap_uint<32> tmp_32_B_ap = *((ap_uint<32>*) (&tmp_32_B_float));
        bram_array[i].range(31, 0) = tmp_32_A_ap;
        bram_array[i].range(63, 32) = tmp_32_B_ap; 
    }

    ap_uint<64> write_val_A = bram_array[bram_depth - 1];
    ap_uint<64> write_val_B = bram_array[bram_depth - 1];
    write_loop:
    for (int i = 0; i < bram_depth - 1; i++) {
#pragma HLS pipeline II=1
        bram_array[i] = write_val_A;
        bram_array[i + 1] = write_val_B;
    }

    ap_uint<64> read_val_A;
    ap_uint<64> read_val_B;
    read_loop:
    for (int i = 0; i < bram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = bram_array[i];
        read_val_B = bram_array[bram_depth - 1 - i];
    }

    for (int i = 0; i < bram_depth; i++) {
        ap_uint<64> tmp_64 = read_val_A + read_val_B;
        ap_uint<32> tmp_32 = tmp_64.range(31, 0);
        out_HBM[i] = *((float*) (&tmp_32));
    }
}
```


### Performance 

* performance: 1 write / CC; 1 read / CC

```
        * Loop: 
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |              |  Latency (cycles) | Iteration|  Initiation Interval  | Trip |          |
        |   Loop Name  |   min   |   max   |  Latency |  achieved |   target  | Count| Pipelined|
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |- Loop 1      |       33|       33|         4|          2|          1|    16|    yes   |
        |- write_loop  |       30|       30|         2|          2|          1|    15|    yes   |
        |- read_loop   |       32|       32|         3|          2|          1|    16|    yes   |
        |- Loop 4      |       16|       16|         2|          1|          1|    16|    yes   |
        +--------------+---------+---------+----------+-----------+-----------+------+----------+

```

### Resource / Depth

* Use small array -> 2 BRAM

bram_depth = 16

Each BRAM merge 2 ports to form a 32-bit wide single-port BRAM. Use 2 slices of single-port 32-bit BRAM to form a 64-bit width array.

```
    +----------------+-----------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |     Memory     |                  Module                 | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +----------------+-----------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_V_U  |bram_ap_uint_64_1port_16_s_bram_array_V  |        2|  0|   0|    0|    16|   64|     1|         1024|
    +----------------+-----------------------------------------+---------+---+----+-----+------+-----+------+-------------
```


* depth = 512 (32bit * 512 = 16Kbit) -> 2 BRAM

```
    +----------------+------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |     Memory     |                  Module                  | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +----------------+------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_V_U  |bram_ap_uint_64_1port_512_s_bram_array_V  |        2|  0|   0|    0|   512|   64|     1|        32768|
    +----------------+------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
```

* slightly more than 512, depth = 513 -> 4 BRAM

```
    +----------------+------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |     Memory     |                  Module                  | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +----------------+------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_V_U  |bram_ap_uint_64_1port_513_s_bram_array_V  |        4|  0|   0|    0|   513|   64|     1|        32832|
    +----------------+------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
```

* depth = 512  * 2 = 1024 -> 4 BRAM

```
    +----------------+-------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |     Memory     |                   Module                  | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +----------------+-------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_V_U  |bram_ap_uint_64_1port_1024_s_bram_array_V  |        4|  0|   0|    0|  1024|   64|     1|        65536|
    +----------------+-------------------------------------------+---------+---+----+-----+------+-----+------+-------------+
```


## float 2 ports

```
    float bram_array[bram_depth];
#pragma HLS resource variable=bram_array core=RAM_2P_BRAM

    for (int i = 0; i < bram_depth; i++) {
        bram_array[i] = table_HBM0[i];
    }

    float write_val_A = bram_array[bram_depth - 1];
    float write_val_B = bram_array[bram_depth - 1];
    write_loop:
    for (int i = 0; i < bram_depth - 1; i++) {
#pragma HLS pipeline II=1
        bram_array[i] = write_val_A;
        bram_array[i + 1] = write_val_B;
    }

    float read_val_A;
    float read_val_B;
    read_loop:
    for (int i = 0; i < bram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = bram_array[i];
        read_val_B = bram_array[bram_depth - 1 - i];
    }
```

### Performance 

* performance: 1 write / CC; 2 read / CC

```
        * Loop: 
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |              |  Latency (cycles) | Iteration|  Initiation Interval  | Trip |          |
        |   Loop Name  |   min   |   max   |  Latency |  achieved |   target  | Count| Pipelined|
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |- Loop 1      |       17|       17|         3|          1|          1|    16|    yes   |
        |- write_loop  |       30|       30|         2|          2|          1|    15|    yes   |
        |- read_loop   |       16|       16|         2|          1|          1|    16|    yes   |
        |- Loop 4      |       16|       16|         2|          1|          1|    16|    yes   |
        +--------------+---------+---------+----------+-----------+-----------+------+----------+

```
### Resource / Depth

* Use small array -> 2 BRAM

bram_depth = 16

Each BRAM 16-bit, use 2 BRAM_2P to form one 32-bit width bank.

```
    +--------------+-----------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |    Memory    |               Module              | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +--------------+-----------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_U  |bram_float_2ports_16_s_bram_array  |        2|  0|   0|    0|    16|   32|     1|          512|
```

* depth = 512 (32bit * 512 = 16Kbit) -> 2 BRAM

```
    * Memory: 
    +--------------+-------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |    Memory    |                Module               | BRAM_18K| FF| LUT| URAM| Words| Bits| Banks| W*Bits*Banks|
    +--------------+-------------------------------------+---------+---+----+-----+------+-----+------+-------------+
    |bram_array_U  |bram_float_2ports_1024_s_bram_array  |        2|  0|   0|    0|  1024|   32|     1|        32768|
    +--------------+-------------------------------------+---------+---+----+-----+------+-----+------+-------------
```


* depth = 513 (slightly more than 512) -> 4 BRAM


* depth = 1024 = 512 * 2 -> 4 BRAM


## float 1 port

```
    float bram_array[bram_depth];
#pragma HLS resource variable=bram_array core=RAM_1P_BRAM

    for (int i = 0; i < bram_depth; i++) {
        bram_array[i] = table_HBM0[i];
    }

    float write_val_A = bram_array[bram_depth - 1];
    float write_val_B = bram_array[bram_depth - 1];
    write_loop:
    for (int i = 0; i < bram_depth - 1; i++) {
#pragma HLS pipeline II=1
        bram_array[i] = write_val_A;
        bram_array[i + 1] = write_val_B;
    }

    float read_val_A;
    float read_val_B;
    read_loop:
    for (int i = 0; i < bram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = bram_array[i];
        read_val_B = bram_array[bram_depth - 1 - i];
    }
```

### Performance 

* performance: 1 write / CC; 1 read / CC

```
        * Loop: 
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |              |  Latency (cycles) | Iteration|  Initiation Interval  | Trip |          |
        |   Loop Name  |   min   |   max   |  Latency |  achieved |   target  | Count| Pipelined|
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |- Loop 1      |       17|       17|         3|          1|          1|    16|    yes   |
        |- write_loop  |       30|       30|         2|          2|          1|    15|    yes   |
        |- read_loop   |       32|       32|         3|          2|          1|    16|    yes   |
        |- Loop 4      |       16|       16|         2|          1|          1|    16|    yes   |
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
```

### Resource / Depth

* Use small array -> 1 BRAM

bram_depth = 16

Each BRAM 32-bit 1 port, use 2 BRAM_2P to form one 32-bit width bank.

* depth = 512 (512 * 32b = 16Kb) -> 1 BRAM

* depth = 513 (slightly more than 16Kb) -> 2 BRAM

* depth = 1024 = 2 * 512 -> 2 BRAM