#pragma once

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