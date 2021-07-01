#pragma once
#include <ap_int.h>

#define QUERY_NUM 10000

typedef struct {
    float dist;
    int cell_ID;
} dist_cell_ID_t;

typedef struct {
    int vec_ID;
    float dist;
} single_PQ_result; 

//////////////////////////////   TEMPLATE START  //////////////////////////////

#define PLRAM_BANK_NUM 4
#define HBM_BANK_NUM 32
#define DDR_BANK 2

/////////////////////////   HBM   ///////////////////////// 
// alignment of tables to HBM: 
// table 0 ~ 31 -> HBM 0 ~ 31
// table 32 ~ 63 -> HBM 0 ~ 31

// 256 MB = 268435456 bytes = 67108864 float32
#define HBM_BANK0_SIZE 10000
#define HBM_BANK1_SIZE 10000
#define HBM_BANK2_SIZE 10000
#define HBM_BANK3_SIZE 10000
#define HBM_BANK4_SIZE 10000
#define HBM_BANK5_SIZE 10000
#define HBM_BANK6_SIZE 10000
#define HBM_BANK7_SIZE 10000
#define HBM_BANK8_SIZE 10000
#define HBM_BANK9_SIZE 10000
#define HBM_BANK10_SIZE 10000
#define HBM_BANK11_SIZE 10000
#define HBM_BANK12_SIZE 10000
#define HBM_BANK13_SIZE 10000
#define HBM_BANK14_SIZE 10000
#define HBM_BANK15_SIZE 10000
#define HBM_BANK16_SIZE 10000
#define HBM_BANK17_SIZE 10000
#define HBM_BANK18_SIZE 10000
#define HBM_BANK19_SIZE 10000
#define HBM_BANK20_SIZE 10000
#define HBM_BANK21_SIZE 10000
#define HBM_BANK22_SIZE 10000
#define HBM_BANK23_SIZE 10000
#define HBM_BANK24_SIZE 10000
#define HBM_BANK25_SIZE 10000
#define HBM_BANK26_SIZE 10000
#define HBM_BANK27_SIZE 10000
#define HBM_BANK28_SIZE 10000
#define HBM_BANK29_SIZE 10000
#define HBM_BANK30_SIZE 10000
#define HBM_BANK31_SIZE 10000

#define DDR_BANK0_SIZE 1600000
#define DDR_BANK1_SIZE 1600000

#define PLRAM_BANK0_SIZE 28000
#define PLRAM_BANK1_SIZE 84000
#define PLRAM_BANK2_SIZE 84000
#define PLRAM_BANK3_SIZE 84000

//////////////////////////////   TEMPLATE END  //////////////////////////////

//typedef ap_int<512> t_axi;
typedef float t_axi;
typedef float D_TYPE;

