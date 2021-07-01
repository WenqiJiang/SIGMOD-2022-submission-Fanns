#pragma once

#define NLIST 8192
#define NPROBE 17
#define D 128
#define M 16
#define K 256

#define HBM_CHANNEL_NUM 11
#define PRIORITY_QUEUE_LEN 10

// 16 = 15 equal one + 1 (all equal) diff must be 1!
#define PE_NUM_CENTER_DIST_COMP 16
#define PE_NUM_CENTER_DIST_COMP_EVEN 15
#define CENTROIDS_PER_PARTITION_EVEN 512
#define CENTROIDS_PER_PARTITION_LAST_PE 512

#define MERGE_ITER (NLIST/PE_NUM_CENTER_DIST_COMP) // the number of input sorted array (32 elements per partition)

// first PE: construct 9 tables per query, last one construct 8
#define PE_NUM_TABLE_CONSTRUCTION 9 // 2
#define PE_NUM_TABLE_CONSTRUCTION_LARGER 8 // 1
#define PE_NUM_TABLE_CONSTRUCTION_SMALLER 1
#define NPROBE_PER_TABLE_CONSTRUCTION_PE_LARGER 2 //9
#define NPROBE_PER_TABLE_CONSTRUCTION_PE_SMALLER 1 //8

#define QUERY_NUM 10 // sw emu
// #define QUERY_NUM 5000 // for bitstream generation use 5000 otherwise the result size (NLIST * QUERY_NUM) will exceed 1 HBM bank capacity
// #define QUERY_NUM 10000

#define LARGE_NUM 99999999 // used to init the heap

//////////////////////////////   TEMPLATE START  //////////////////////////////

#define PLRAM_BANK_NUM 4
#define HBM_BANK_NUM 32
#define DDR_BANK 2

/////////////////////////   HBM   ///////////////////////// 
// alignment of tables to HBM: 
// table 0 ~ 31 -> HBM 0 ~ 31
// table 32 ~ 63 -> HBM 0 ~ 31

// 256 MB = 268435456 bytes = 67108864 float32 = 4194304 ap_int<512>
// 128 MB = 2097152 ap_int<512>
#define HBM_BANK0_SIZE 2097152
#define HBM_BANK1_SIZE 2097152
#define HBM_BANK2_SIZE 2097152
#define HBM_BANK3_SIZE 2097152
#define HBM_BANK4_SIZE 2097152
#define HBM_BANK5_SIZE 2097152
#define HBM_BANK6_SIZE 2097152
#define HBM_BANK7_SIZE 2097152
#define HBM_BANK8_SIZE 2097152
#define HBM_BANK9_SIZE 2097152
#define HBM_BANK10_SIZE 2097152
#define HBM_BANK11_SIZE 2097152
#define HBM_BANK12_SIZE 2097152
#define HBM_BANK13_SIZE 2097152
#define HBM_BANK14_SIZE 2097152
#define HBM_BANK15_SIZE 2097152
#define HBM_BANK16_SIZE 2097152
#define HBM_BANK17_SIZE 2097152
#define HBM_BANK18_SIZE 2097152
#define HBM_BANK19_SIZE 2097152
#define HBM_BANK20_SIZE 2097152
#define HBM_BANK21_SIZE 2097152
#define HBM_BANK22_SIZE 2097152
#define HBM_BANK23_SIZE 2097152
#define HBM_BANK24_SIZE 2097152
#define HBM_BANK25_SIZE 2097152
#define HBM_BANK26_SIZE 2097152
#define HBM_BANK27_SIZE 2097152
#define HBM_BANK28_SIZE 2097152
#define HBM_BANK29_SIZE 2097152
#define HBM_BANK30_SIZE 2097152
#define HBM_BANK31_SIZE 2097152

#define DDR_BANK0_SIZE 1600000
#define DDR_BANK1_SIZE 1600000

#define PLRAM_BANK0_SIZE 28000
#define PLRAM_BANK1_SIZE 84000
#define PLRAM_BANK2_SIZE 84000
#define PLRAM_BANK3_SIZE 84000

//////////////////////////////   TEMPLATE END  //////////////////////////////



