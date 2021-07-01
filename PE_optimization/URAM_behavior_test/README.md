# URAM behavior test

This folder tests the resource consumption & performance of URAM, given different data type, array depth, and port number.

Vitis HLS version: 2019.2

## Key conclusions

URAM has fixed width of 72 bit, fixed size of 288Kbit, and fixed 2 ports for either read or write. (wp477)

* the depth of URAM array is fixed, thus narrow data type will waste storage
  * depth is 288 K / 72 = 4096
  * for 64 bit common data width, the effective size is 64 / 72 * 288 K = 256 Kbit
  * for 32 bit float, the effective size is 32 / 72 * 288 K = 128 Kbit
* given the same width of 64 bit, struct of 2 floats is not equivalent to ap_uint<64>
  * struct of 2 floats will be treated as 2 independent float array (128 Kbit, under-utilized space)
  * thus to fully use the URAM space, use ap_uint<64> and translate the contents to float
* for each URAM slice, it allows 1 write / CC or 2 reads / CC
  * not sure why only 1 write given 2 ports
* 2 ports settting core=RAM_2P_URAM won't increase the usage at all compare to 1 port since URAM has 2 ports physically


## ap_uint<64>

```
    ap_uint<64> uram_array[uram_depth];
#pragma HLS resource variable=uram_array core=RAM_2P_URAM

    ap_uint<64> write_val_A = uram_array[uram_depth - 1];
    ap_uint<64> write_val_B = uram_array[uram_depth - 1];
    write_loop:
    for (int i = 0; i < uram_depth - 1; i++) {
#pragma HLS pipeline II=1
        uram_array[i] = write_val_A;
        uram_array[i + 1] = write_val_B;
    }

    ap_uint<64> read_val_A;
    ap_uint<64> read_val_B;
    read_loop:
    for (int i = 0; i < uram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = uram_array[i];
        read_val_B = uram_array[uram_depth - 1 - i];
    }
```

### Performance 

* performance: 1 write / CC; 2 read / CC

Not sure why only 1 write per cycle for URAM_2P...

```
        * Loop: 
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |              |  Latency (cycles) | Iteration|  Initiation Interval  | Trip |          |
        |   Loop Name  |   min   |   max   |  Latency |  achieved |   target  | Count| Pipelined|
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |- Loop 1      |       33|       33|         4|          2|          1|    16|    yes   |
        |- write_loop  |       32|       32|         2|          2|          1|    16|    yes   |
        |- read_loop   |       16|       16|         2|          1|          1|    16|    yes   |
        |- Loop 4      |       16|       16|         2|          1|          1|    16|    yes   |
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
```

### Resource / Depth

* Use small array -> 1 URAM

uram_depth = 16

```
    ap_uint<64> uram_array[uram_depth];
#pragma HLS resource variable=uram_array core=RAM_2P_URAM
```

* Use full 256Kbit -> 1 URAM

for 64 bit common data width, the effective size should be 64 / 72 * 288 K = 256 Kbit

uram_depth = 256 * 1024 / 64 = 4096 

* Use between 256Kbit and 288Kbit -> 2 URAM

uram_depth = 4097

This proves "for 64 bit common data width, the effective size should be 64 / 72 * 288 K = 256 Kbit"

* 512Kbit -> 2 URAM

uram_depth = 8192


## float

```
    float uram_array[uram_depth];
#pragma HLS resource variable=uram_array core=RAM_2P_URAM

    float write_val_A = uram_array[uram_depth - 1];
    float write_val_B = uram_array[uram_depth - 1];
    write_loop:
    for (int i = 0; i < uram_depth - 1; i++) {
#pragma HLS pipeline II=1
        uram_array[i] = write_val_A;
        uram_array[i + 1] = write_val_B;
    }

    float read_val_A;
    float read_val_B;
    read_loop:
    for (int i = 0; i < uram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = uram_array[i];
        read_val_B = uram_array[uram_depth - 1 - i];
    }
```

### Performance

1 write / CC

2 read / CC

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

### Resource

* Use small array -> 1 URAM

uram_depth = 16

```
    float uram_array[uram_depth];
#pragma HLS resource variable=uram_array core=RAM_2P_URAM
```

* Use full uram_depth=4096 -> 1 URAM

for 32 bit common data width, the effective size should be 64 / 72 * 288 K = 128 Kbit

* Use more than 4096, buram_depth = 4097 -> 2 URAM

* 2x depth = 8192 -> 2 URAM


## struct (2 floats)

```
    typedef struct {
        float f_0;
        float f_1;
    } two_floats_t;


    two_floats_t uram_array[uram_depth];
#pragma HLS resource variable=uram_array core=RAM_2P_URAM

    two_floats_t write_val_A = uram_array[uram_depth - 1];
    two_floats_t write_val_B = uram_array[uram_depth - 1];
    write_loop:
    for (int i = 0; i < uram_depth - 1; i++) {
#pragma HLS pipeline II=1
        uram_array[i] = write_val_A;
        uram_array[i + 1] = write_val_B;
    }

    two_floats_t read_val_A;
    two_floats_t read_val_B;
    read_loop:
    for (int i = 0; i < uram_depth; i++) {
#pragma HLS pipeline II=1
        read_val_A = uram_array[i];
        read_val_B = uram_array[uram_depth - 1 - i];
    }
```

### Performance

1 write / CC

2 reads / CC

```
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |              |  Latency (cycles) | Iteration|  Initiation Interval  | Trip |          |
        |   Loop Name  |   min   |   max   |  Latency |  achieved |   target  | Count| Pipelined|
        +--------------+---------+---------+----------+-----------+-----------+------+----------+
        |- Loop 1      |       33|       33|         4|          2|          1|    16|    yes   |
        |- write_loop  |       30|       30|         2|          2|          1|    15|    yes   |
        |- read_loop   |       16|       16|         2|          1|          1|    16|    yes   |
        |- Loop 4      |       16|       16|         2|          1|          1|    16|    yes   |
        +--------------+---------+---------+----------+-----------+-----------+------+----------
```



### Resource

By using a struct of 2 floats, HLS treats it like 2 independent float URAM array. Thus it takes 2X resources compared with ap_uint<64>.

* Use small array -> 2 URAM

uram_depth = 16

* Use full uram_depth=4096 -> 2 URAM

* Use more than 4096, buram_depth = 4097 -> 4 URAM

* Use 2x depth (8192) -> 4 URAM
