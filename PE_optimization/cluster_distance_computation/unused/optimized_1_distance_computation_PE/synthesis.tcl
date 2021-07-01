open_project hls_output 
open_solution xcu280-fsvh2892-2L-e  
add_files -cflags "-std=c++11" src/vadd.cpp 
set_top vadd 
set_part xcu280-fsvh2892-2L-e 
create_clock -period 140MHz
config_interface -m_axi_addr64
csynth_design
exit
