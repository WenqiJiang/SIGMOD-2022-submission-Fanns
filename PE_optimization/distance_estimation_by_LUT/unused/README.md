# Distance Estimation PE Optmization Log

## Without distance LUT init

### Unoptimized version 1

Use independent BRAM banks for different distance LUT columns (each bank for 2 columns):

FF 4895

LUT 4035

```
    // each BRAM stores 2 tables, which can be looked up concurrently by 2 ports
    float distance_lookup_table_local_0[512], distance_lookup_table_local_1[512], 
        distance_lookup_table_local_2[512], distance_lookup_table_local_3[512], 
        distance_lookup_table_local_4[512], distance_lookup_table_local_5[512], 
        distance_lookup_table_local_6[512], distance_lookup_table_local_7[512];
#pragma HLS resource variable=distance_lookup_table_local_0 core=RAM_2P_BRAM
#pragma HLS resource variable=distance_lookup_table_local_1 core=RAM_2P_BRAM
#pragma HLS resource variable=distance_lookup_table_local_2 core=RAM_2P_BRAM
#pragma HLS resource variable=distance_lookup_table_local_3 core=RAM_2P_BRAM
#pragma HLS resource variable=distance_lookup_table_local_4 core=RAM_2P_BRAM
#pragma HLS resource variable=distance_lookup_table_local_5 core=RAM_2P_BRAM
#pragma HLS resource variable=distance_lookup_table_local_6 core=RAM_2P_BRAM
#pragma HLS resource variable=distance_lookup_table_local_7 core=RAM_2P_BRAM
```

Instances consume 3416 LUTs, and that is inevitable, because those are floating point additions and indicated by the instance names, they are already full_dsp units.

Register consumes 512 LUTs, but those are intermediate results. 

```
================================================================
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+---------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT   | URAM|
+---------------------+---------+------+---------+---------+-----+
|DSP                  |        -|     -|        -|        -|    -|
|Expression           |        -|     -|        0|       32|    -|
|FIFO                 |        -|     -|        -|        -|    -|
|Instance             |        -|    30|     2655|     3416|    -|
|Memory               |        8|     -|        0|        0|    -|
|Multiplexer          |        -|     -|        -|       75|    -|
|Register             |        -|     -|     2240|      512|    -|
+---------------------+---------+------+---------+---------+-----+
|Total                |        8|    30|     4895|     4035|    0|
+---------------------+---------+------+---------+---------+-----+
|Available SLR        |     1344|  3008|   869120|   434560|  320|
+---------------------+---------+------+---------+---------+-----+
|Utilization SLR (%)  |    ~0   |  ~0  |    ~0   |    ~0   |    0|
+---------------------+---------+------+---------+---------+-----+
|Available            |     4032|  9024|  2607360|  1303680|  960|
+---------------------+---------+------+---------+---------+-----+
|Utilization (%)      |    ~0   |  ~0  |    ~0   |    ~0   |    0|
+---------------------+---------+------+---------+---------+-----+

+ Detail: 
    * Instance: 
    +-----------------------------------------+-------------------------------------+---------+----+-----+-----+-----+
    |                 Instance                |                Module               | BRAM_18K| DSP|  FF | LUT | URAM|
    +-----------------------------------------+-------------------------------------+---------+----+-----+-----+-----+
    |vadd_add_26ns_26ns_26_1_1_U21            |vadd_add_26ns_26ns_26_1_1            |        0|   0|    0|   26|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U6   |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U7   |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U8   |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U9   |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U10  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U11  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U12  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U13  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U14  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U15  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U16  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U17  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U18  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U19  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U20  |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
    +-----------------------------------------+-------------------------------------+---------+----+-----+-----+-----+
    |Total                                    |                                     |        0|  30| 2655| 3416|    0|
    +-----------------------------------------+-------------------------------------+---------+----+-----+-----+-----+

    * Register: 
    +-----------------------------------------------+----+----+-----+-----------+
    |                      Name                     | FF | LUT| Bits| Const Bits|
    +-----------------------------------------------+----+----+-----+-----------+
    |add10_reg_994                                  |  32|   0|   32|          0|
    |add11_reg_1009                                 |  32|   0|   32|          0|
    |add12_reg_1024                                 |  32|   0|   32|          0|
    |add13_reg_1039                                 |  32|   0|   32|          0|
    |add14_reg_1049                                 |  32|   0|   32|          0|
    |add1_reg_859                                   |  32|   0|   32|          0|
    |add2_reg_874                                   |  32|   0|   32|          0|
    |add3_reg_889                                   |  32|   0|   32|          0|
    |add4_reg_904                                   |  32|   0|   32|          0|
    |add5_reg_919                                   |  32|   0|   32|          0|
    |add6_reg_934                                   |  32|   0|   32|          0|
    |add7_reg_949                                   |  32|   0|   32|          0|
    |add8_reg_964                                   |  32|   0|   32|          0|
    |add9_reg_979                                   |  32|   0|   32|          0|
    |add_reg_844                                    |  32|   0|   32|          0|
    |ap_CS_fsm                                      |   3|   0|    3|          0|
    |ap_done_reg                                    |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter0                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter1                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter10                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter11                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter12                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter13                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter14                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter15                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter16                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter17                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter18                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter19                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter2                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter20                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter21                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter22                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter23                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter24                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter25                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter26                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter27                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter28                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter29                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter3                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter30                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter31                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter32                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter33                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter34                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter35                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter36                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter37                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter38                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter39                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter4                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter40                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter41                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter42                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter43                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter44                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter45                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter46                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter47                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter48                       |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter5                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter6                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter7                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter8                        |   1|   0|    1|          0|
    |ap_enable_reg_pp0_iter9                        |   1|   0|    1|          0|
    |distance_lookup_table_local_0_load_1_reg_939   |  32|   0|   32|          0|
    |distance_lookup_table_local_0_load_reg_829     |  32|   0|   32|          0|
    |distance_lookup_table_local_1_load_1_reg_954   |  32|   0|   32|          0|
    |distance_lookup_table_local_1_load_reg_834     |  32|   0|   32|          0|
    |distance_lookup_table_local_2_load_1_reg_969   |  32|   0|   32|          0|
    |distance_lookup_table_local_2_load_reg_849     |  32|   0|   32|          0|
    |distance_lookup_table_local_3_load_1_reg_984   |  32|   0|   32|          0|
    |distance_lookup_table_local_3_load_reg_864     |  32|   0|   32|          0|
    |distance_lookup_table_local_4_load_1_reg_999   |  32|   0|   32|          0|
    |distance_lookup_table_local_4_load_reg_879     |  32|   0|   32|          0|
    |distance_lookup_table_local_5_load_1_reg_1014  |  32|   0|   32|          0|
    |distance_lookup_table_local_5_load_reg_894     |  32|   0|   32|          0|
    |distance_lookup_table_local_6_load_1_reg_1029  |  32|   0|   32|          0|
    |distance_lookup_table_local_6_load_reg_909     |  32|   0|   32|          0|
    |distance_lookup_table_local_7_load_1_reg_1044  |  32|   0|   32|          0|
    |distance_lookup_table_local_7_load_reg_924     |  32|   0|   32|          0|
    |icmp_ln168_reg_735                             |   1|   0|    1|          0|
    |indvar_flatten11_reg_347                       |  26|   0|   26|          0|
    |tmp_10_reg_794                                 |   8|   0|    8|          0|
    |tmp_11_reg_799                                 |   8|   0|    8|          0|
    |tmp_12_reg_804                                 |   8|   0|    8|          0|
    |tmp_13_reg_809                                 |   8|   0|    8|          0|
    |tmp_14_reg_814                                 |   8|   0|    8|          0|
    |tmp_15_reg_819                                 |   8|   0|    8|          0|
    |tmp_3_reg_749                                  |   8|   0|    8|          0|
    |tmp_4_reg_754                                  |   8|   0|    8|          0|
    |tmp_5_reg_789                                  |   8|   0|    8|          0|
    |tmp_6_reg_769                                  |   8|   0|    8|          0|
    |tmp_7_reg_774                                  |   8|   0|    8|          0|
    |tmp_8_reg_779                                  |   8|   0|    8|          0|
    |tmp_9_reg_784                                  |   8|   0|    8|          0|
    |tmp_reg_744                                    |  32|   0|   32|          0|
    |trunc_ln_reg_824                               |   8|   0|    8|          0|
    |icmp_ln168_reg_735                             |  64|  32|    1|          0|
    |tmp_10_reg_794                                 |  64|  32|    8|          0|
    |tmp_11_reg_799                                 |  64|  32|    8|          0|
    |tmp_12_reg_804                                 |  64|  32|    8|          0|
    |tmp_13_reg_809                                 |  64|  32|    8|          0|
    |tmp_14_reg_814                                 |  64|  32|    8|          0|
    |tmp_15_reg_819                                 |  64|  32|    8|          0|
    |tmp_3_reg_749                                  |  64|  32|    8|          0|
    |tmp_4_reg_754                                  |  64|  32|    8|          0|
    |tmp_5_reg_789                                  |  64|  32|    8|          0|
    |tmp_6_reg_769                                  |  64|  32|    8|          0|
    |tmp_7_reg_774                                  |  64|  32|    8|          0|
    |tmp_8_reg_779                                  |  64|  32|    8|          0|
    |tmp_9_reg_784                                  |  64|  32|    8|          0|
    |tmp_reg_744                                    |  64|  32|   32|          0|
    |trunc_ln_reg_824                               |  64|  32|    8|          0|
    +-----------------------------------------------+----+----+-----+-----------+
    |Total                                          |2240| 512| 1361|          0|
    +-----------------------------------------------+----+----+-----+-----------+
```

### Unoptmized Version 2

Use array partition pragma instead of manually doing so.

```
    float distance_LUT[8][512];
#pragma HLS array_partition variable=distance_LUT dim=1
#pragma HLS resource variable=distance_LUT core=RAM_2P_BRAM
```

Resource consumption is exactly the same as version 1.

FF 4895

LUT 4035

Each fadd consumes 226 LUTs, although the name indicates fulldsp is used, we can still try to add pragma.

```
    * Instance: 
    +-----------------------------------------+-------------------------------------+---------+----+-----+-----+-----+
    |                 Instance                |                Module               | BRAM_18K| DSP|  FF | LUT | URAM|
    +-----------------------------------------+-------------------------------------+---------+----+-----+-----+-----+
    |vadd_add_26ns_26ns_26_1_1_U21            |vadd_add_26ns_26ns_26_1_1            |        0|   0|    0|   26|    0|
    |vadd_fadd_32ns_32ns_32_3_full_dsp_1_U6   |vadd_fadd_32ns_32ns_32_3_full_dsp_1  |        0|   2|  177|  226|    0|
```

### Unoptmized Version 3 

Adding full dsp pragma. Didn't change LUT usage at all compared with version 1 and version 2 (they already used full DSP).

```
#pragma HLS resource variable=out.dist core=FAddSub_fulldsp
                out.dist = 
                    distance_LUT[0][PQ_local.PQ_code[0]] + 
                    distance_LUT[1][PQ_local.PQ_code[1]] + 
                    distance_LUT[2][PQ_local.PQ_code[2]] + 
                    distance_LUT[3][PQ_local.PQ_code[3]] + 
```