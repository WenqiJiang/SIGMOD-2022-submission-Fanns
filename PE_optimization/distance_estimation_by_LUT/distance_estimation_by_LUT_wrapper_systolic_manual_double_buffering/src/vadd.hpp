#include <hls_stream.h>

#include "constants.hpp"

extern "C" {


void vadd(  
    const t_axi* HBM_in0, const t_axi* HBM_in1, 
    const t_axi* HBM_in2, const t_axi* HBM_in3, 
    const t_axi* HBM_in4, const t_axi* HBM_in5, 
    const t_axi* HBM_in6, const t_axi* HBM_in7, 
    const t_axi* HBM_in8, const t_axi* HBM_in9, 
    const t_axi* HBM_in10, const t_axi* HBM_in11, 
    const t_axi* HBM_in12, const t_axi* HBM_in13, 
    const t_axi* HBM_in14, const t_axi* HBM_in15, 
    const t_axi* HBM_in16, const t_axi* HBM_in17, 
    const t_axi* HBM_in18, const t_axi* HBM_in19, 
    const t_axi* HBM_in20, 
    // HBM_in21 assigned for HBM_info_start_addr_and_scanned_entries_every_cell
    const int* HBM_info_start_addr_and_scanned_entries_every_cell, 
    // const t_axi* HBM_in22, const t_axi* HBM_in23, 
    // const t_axi* HBM_in24, const t_axi* HBM_in25, 
    // const t_axi* HBM_in26, const t_axi* HBM_in27, 
    // const t_axi* HBM_in28, const t_axi* HBM_in29, 
    // const t_axi* HBM_in30, const t_axi* HBM_in31, 
    // const t_axi* table_DDR0, const t_axi* table_DDR1, 
    result_t* out_PLRAM
    );
}

