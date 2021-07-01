#pragma once
#include <ap_int.h>

#define NLIST 8192
#define NPROBE 32
#define D 128
#define M 16
#define K 256
#define CENTROID_PARTITIONS 32
#define CENTROIDS_PER_PARTITION (NLIST / CENTROID_PARTITIONS)
#define QUERY_NUM 10000
#define SCANNED_ENTRIES_PER_CELL 193

#define LARGE_NUM 99999999 // used to init the heap

typedef struct {
    // (4 + 16) * 3 * 8 = 480 bit
    int vec_ID_A;
    unsigned char PQ_code_A[M];

    int vec_ID_B;
    unsigned char PQ_code_B[M];

    int vec_ID_C;
    unsigned char PQ_code_C[M];
} PQ_codes;

typedef struct {
    int vec_ID;
    unsigned char PQ_code[M];
} single_PQ;
 
typedef struct {
    // a wrapper for single_PQ
    // used in the ap_uint<480> to 3 x PQ split function
    single_PQ PQ_0;
    single_PQ PQ_1;
    single_PQ PQ_2;
} three_PQ_codes;

typedef struct {
    int vec_ID;
    float dist;
} single_PQ_result; 

typedef struct {
    // each distance LUT has K=256 such row
    // each distance_LUT_PQ16_t is the content of a single row (16 floats)
    float dist_0; 
    float dist_1; 
    float dist_2; 
    float dist_3; 
    float dist_4; 
    float dist_5; 
    float dist_6;
    float dist_7; 
    float dist_8; 
    float dist_9; 
    float dist_10; 
    float dist_11; 
    float dist_12; 
    float dist_13;
    float dist_14; 
    float dist_15;
} distance_LUT_PQ16_t;
// typedef struct {
//     // (4 + 16) * 3 * 8 = 480 bit
//     single_PQ_result single_PQ_result_0;
//     single_PQ_result single_PQ_result_1;
//     single_PQ_result single_PQ_result_2;
// } result_t;

typedef ap_uint<256> result_t;
typedef ap_uint<512> t_axi;
// typedef float D_TYPE;

//////////////////////////////   TEMPLATE START  //////////////////////////////

#define PLRAM_BANK_NUM 4
#define HBM_BANK_NUM 32
#define DDR_BANK 2

/////////////////////////   HBM   ///////////////////////// 
// alignment of tables to HBM: 
// table 0 ~ 31 -> HBM 0 ~ 31
// table 32 ~ 63 -> HBM 0 ~ 31

// 256 MB = 268435456 bytes = 67108864 float32
#define HBM_BANK0_SIZE 100000
#define HBM_BANK1_SIZE 100000
#define HBM_BANK2_SIZE 100000
#define HBM_BANK3_SIZE 100000
#define HBM_BANK4_SIZE 100000
#define HBM_BANK5_SIZE 100000
#define HBM_BANK6_SIZE 100000
#define HBM_BANK7_SIZE 100000
#define HBM_BANK8_SIZE 100000
#define HBM_BANK9_SIZE 100000
#define HBM_BANK10_SIZE 100000
#define HBM_BANK11_SIZE 100000
#define HBM_BANK12_SIZE 100000
#define HBM_BANK13_SIZE 100000
#define HBM_BANK14_SIZE 100000
#define HBM_BANK15_SIZE 100000
#define HBM_BANK16_SIZE 100000
#define HBM_BANK17_SIZE 100000
#define HBM_BANK18_SIZE 100000
#define HBM_BANK19_SIZE 100000
#define HBM_BANK20_SIZE 100000
#define HBM_BANK21_SIZE 100000
#define HBM_BANK22_SIZE 100000
#define HBM_BANK23_SIZE 100000
#define HBM_BANK24_SIZE 100000
#define HBM_BANK25_SIZE 100000
#define HBM_BANK26_SIZE 100000
#define HBM_BANK27_SIZE 100000
#define HBM_BANK28_SIZE 100000
#define HBM_BANK29_SIZE 100000
#define HBM_BANK30_SIZE 100000
#define HBM_BANK31_SIZE 100000

#define DDR_BANK0_SIZE 1600000
#define DDR_BANK1_SIZE 1600000

#define PLRAM_BANK0_SIZE 28000
#define PLRAM_BANK1_SIZE 84000
#define PLRAM_BANK2_SIZE 84000
#define PLRAM_BANK3_SIZE 84000

//////////////////////////////   TEMPLATE END  //////////////////////////////



