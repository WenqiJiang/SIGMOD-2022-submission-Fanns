# FIFO behavior test

This folder tests when **using BRAM as FIFO**, the relationship between data type, depth, and BRAM consumption.

As in UG573, a BRAM 18K can be configured in several ways:

For the 18 Kb mode, the supported configurations are 4K x 4, 2K x 9, 1K x 18, and 512 x 36.

The supported configurations for the 36 Kb FIFO are 8K x 4, 4K x 9, 2K x 18, 1K x 36, and 512 x 72.

## 8-bit int

18K -> 8 bit * 2048

**Results: depth 0~2048 1 BRAM, 2049~4096 2 BRAM, etc.**

* depth=2048 -> 1 BRAM

* depth=2049 -> 2 BRAM

* depth=4096 -> 2 BRAM

## 16-bit int

18K -> 16 bit * 1024

**Results: depth 0~1024 1 BRAM, 1025~2048 2 BRAM, etc.**

* depth=1024 -> 1 BRAM

* depth=1025 -> 2 BRAM

* depth=2048 -> 2 BRAM

## 32-bit int

18K -> 16 bit * 1024 or 32 bit * 512

**Results: depth 0~512 1 BRAM, 513~1024 2 BRAM, etc.**

* depth=16 -> 1 BRAM

```
    hls::stream<int> s_FIFO;
#pragma HLS stream variable=s_FIFO depth=16
#pragma HLS resource variable=s_FIFO core=FIFO_BRAM
```

```
    +-----------+---------+-----+----+-----+------+-----+---------+
    |    Name   | BRAM_18K|  FF | LUT| URAM| Depth| Bits| Size:D*B|
    +-----------+---------+-----+----+-----+------+-----+---------+
    |s_FIFO1_U  |        1|  151|   0|    -|    16|   32|      512|
    +-----------+---------+-----+----+-----+------+-----+---------
```

* depth=512 -> 1 BRAM

* depth=513 -> 2 BRAM

* depth=1024 -> 2 BRAM

```
    +-----------+---------+-----+----+-----+------+-----+---------+
    |    Name   | BRAM_18K|  FF | LUT| URAM| Depth| Bits| Size:D*B|
    +-----------+---------+-----+----+-----+------+-----+---------+
    |s_FIFO1_U  |        2|  163|   0|    -|  1024|   32|    32768|
    +-----------+---------+-----+----+-----+------+-----+---------+
```


## 64-bit int

BRAM can be configured as at most 32-bit wide, thus at least 2 BRAM is needed

18K -> 16 bit * 1024 or 32 bit * 512

**Results: depth 0~512 2 BRAM, 513~1024 4 BRAM, etc.**

* depth=16 -> 2 BRAM

* depth=512 -> 2 BRAM

* depth=513 -> 4 BRAM

* depth=1024 -> 4 BRAM

## 512-bit int

For wider data type, BRAM width of 36 can become useful. 512 / 36 = 14.22, thus at least 15 BRAM is needed.

18K -> 36 bit * 512

**Results non-linear to depth: depth 0~512 15 BRAM, 513~1024 29 BRAM, etc. HLS is pretty smart at saving BRAMs by configuring each individual BRAM's width and depth.**

* depth=512 -> 15 BRAM

* depth=513 -> 29 BRAM

28 BRAMs (14 * 2) are configured as 36-bit width, 512 depth; another is configured as 8-bit width, 513 depth.

* depth=1024 -> 29 BRAM

## Struct: two char

HLS treats the content in a struct as separate variable, thus need at least 2 BRAM even if they are 8-bit width. If saving BRAM is the object, merge them as ap_uint<16>.

18K -> 8 bit * 2048

```
typedef struct {
    char c_0;
    char c_1;
} two_char_t;
```

**Results: depth 0~2048 2 BRAM, 2049~4096 4 BRAM, etc.**

* depth=2048 -> 2 BRAM

* depth=2049 -> 4 BRAM

* depth=4096 -> 4 BRAM

## Struct: two float

HLS treats the content in a struct as separate variable, thus need at least 2 BRAM even if they are 8-bit width. If saving BRAM is the object, merge them as ap_uint<16>.

18K -> 16 bit * 1024 or 32 bit * 512

```
typedef struct {
    float f[2];
} two_floats_t;
```

**Results: depth 0~512 2 BRAM, 513~1024 4 BRAM, etc.**


* depth=512 -> 2 BRAM

* depth=513 -> 4 BRAM

* depth=1024 -> 4 BRAM