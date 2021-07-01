import numpy as np

from constants import * 
from queue_and_sorting import *
from stages import *
from utils import *

"""
An example of the dict for Performance & Resource:

dict = {
    "name"     : "option_name",
    # Performance: 
    "cycles_per_query" : 1000,
    "QPS"      : 10000,
    # Resource:
    "HBM_bank" : 0,
    "BRAM_18K" : 10,
    "DSP48E"   : 100,    
    "FF"       : 2000,   
    "LUT"      : 1200, 
    "URAM"     : 6
}

perf_resource_dict["name"] = "option name"

perf_resource_dict["cycles_per_query"] = cycles_per_query
perf_resource_dict["QPS"] = QPS

perf_resource_dict["HBM_bank"] = 
perf_resource_dict["BRAM_18K"] = 
perf_resource_dict["DSP48E"] = 
perf_resource_dict["FF"] = 
perf_resource_dict["LUT"] = 
perf_resource_dict["URAM"] = 

"""

total_valid_design = 0 # design that is within resource consumption range

## TODO: add HBM interconnection, the shell, the global interconnection, and network stack

def get_best_hardware(nlist, nprobe, OPQ_enable=True):
    """
    given a single algorithm setting, return the hardware combination that achieves
        the highest performance
    """

    best_solution_QPS = 0
    best_solution_stage_option_list = []
    best_solution_PE_num_list = []
    global total_valid_design

    options_stage_1_OPQ = get_options_stage_1_OPQ()
    options_stage_2_cluster_distance_computation = \
        get_options_stage_2_cluster_distance_computation(nlist)
    options_stage_3_select_Voronoi_cells = get_options_stage_3_select_Voronoi_cells(nlist, nprobe)
    options_stage_4_distance_LUT_construction = get_options_stage_4_distance_LUT_construction(nlist, nprobe)

    # only allow 3x PE number, because each HBM can deliver 3 numbers per cycle
    for stage_5_PE_num in range(3 * MIN_HBM_bank, 27 * 3 + 1, 3): 
        # N_compute_per_nprobe * nprobe == N_insertion_per_stream
        # PE_num == input_stream_num
        if OPQ_enable:
            N_compute_per_nprobe = int(scan_ratio_with_OPQ[nlist] * TOTAL_VECTORS / nlist / stage_5_PE_num) + 1
        else:
            N_compute_per_nprobe = int(scan_ratio_without_OPQ[nlist] * TOTAL_VECTORS / nlist / stage_5_PE_num) + 1
        N_insertion_per_stream = int(nprobe * N_compute_per_nprobe)

        options_stage_5_distance_estimation_by_LUT = \
            get_options_stage_5_distance_estimation_by_LUT(
                stage_5_PE_num, nprobe, N_compute_per_nprobe)
        options_stage_6_sort_reduction = \
            get_options_stage6_sort_reduction(
                stage_5_PE_num, N_insertion_per_stream)

        if OPQ_enable:

            for option_stage_1_OPQ in options_stage_1_OPQ:
                for option_stage_2 in options_stage_2_cluster_distance_computation:
                    for option_stage_3 in options_stage_3_select_Voronoi_cells:
                        for option_stage_4 in options_stage_4_distance_LUT_construction:
                            for option_stage_5 in options_stage_5_distance_estimation_by_LUT:
                                for option_stage_6 in options_stage_6_sort_reduction:

                                    option_list = \
                                        [option_stage_1_OPQ, option_stage_2, option_stage_3, \
                                            option_stage_4, option_stage_5, option_stage_6]
                                    PE_num_list = [1, 1, 1, 1, 1, 1]

                                    if fit_resource_constraints(option_list, PE_num_list, count_shell=True):

                                        total_valid_design = total_valid_design + 1 # each valide design counts

                                        bottleneck_ID, accelerator_QPS = get_bottleneck(option_list, PE_num_list)
                                        if accelerator_QPS > best_solution_QPS:
                                            best_solution_QPS = accelerator_QPS
                                            best_solution_stage_option_list = option_list
                                            best_solution_PE_num_list = PE_num_list
                                        elif accelerator_QPS == best_solution_QPS:
                                            if resource_consumption_A_less_than_B(
                                                option_list, PE_num_list,
                                                best_solution_stage_option_list, best_solution_PE_num_list):
                                                    best_solution_QPS = accelerator_QPS
                                                    best_solution_stage_option_list = option_list
                                                    best_solution_PE_num_list = PE_num_list
        else: # no OPQ
            for option_stage_2 in options_stage_2_cluster_distance_computation:
                for option_stage_3 in options_stage_3_select_Voronoi_cells:
                    for option_stage_4 in options_stage_4_distance_LUT_construction:
                        for option_stage_5 in options_stage_5_distance_estimation_by_LUT:
                            for option_stage_6 in options_stage_6_sort_reduction:

                                option_list = \
                                    [option_stage_2, option_stage_3, \
                                        option_stage_4, option_stage_5, option_stage_6]
                                PE_num_list = [1, 1, 1, 1, 1]

                                if fit_resource_constraints(option_list, PE_num_list, count_shell=True):

                                    total_valid_design = total_valid_design + 1 # each valide design counts

                                    bottleneck_ID, accelerator_QPS = get_bottleneck(option_list, PE_num_list)
                                    if accelerator_QPS > best_solution_QPS:
                                        best_solution_QPS = accelerator_QPS
                                        best_solution_stage_option_list = option_list
                                        best_solution_PE_num_list = PE_num_list
                                    elif accelerator_QPS == best_solution_QPS:
                                        if resource_consumption_A_less_than_B(
                                            option_list, PE_num_list,
                                            best_solution_stage_option_list, best_solution_PE_num_list):
                                                best_solution_QPS = accelerator_QPS
                                                best_solution_stage_option_list = option_list
                                                best_solution_PE_num_list = PE_num_list

    return best_solution_QPS, best_solution_stage_option_list, best_solution_PE_num_list


if __name__ == "__main__":

    # unit_test()

    # tuple: (setting_name, nlist, nprobe, OPQ_or_not)
    algorithm_settings = [ \
        ("IVF2048", 2048, 28, False),
        ("IVF4096", 4096, 29, False),
        ("IVF8192", 8192, 22, False),
        ("IVF16384", 16384, 29, False),
        ("IVF32768", 32768, 29, False),
        ("IVF65536", 65536, 33, False),
        ("IVF131072", 131072, 40, False),
        ("IVF262144", 262144, 45, False),
        ("IVF1024,OPQ", 1024, 13, True),
        ("IVF2048,OPQ", 2048, 13, True),
        ("IVF4096,OPQ", 4096, 17, True),
        ("IVF8192,OPQ", 8192, 17, True),
        ("IVF16384,OPQ", 16384, 21, True),
        ("IVF32768,OPQ", 32768, 24, True),
        ("IVF65536,OPQ", 65536, 30, True),
        ("IVF131072,OPQ", 131072, 37, True),
        ("IVF262144,OPQ", 262144, 42, True)
    ]

    best_solution_name = None
    best_solution_QPS = 0 
    best_solution_stage_option_list = None 
    best_solution_PE_num_list = None

    for name, nlist, nprobe, OPQ_enable in algorithm_settings:
        print(name, nlist, nprobe, OPQ_enable)
        current_solution_QPS, current_solution_stage_option_list, current_solution_PE_num_list = \
            get_best_hardware(nlist=nlist, nprobe=nprobe, OPQ_enable=OPQ_enable)

        if current_solution_QPS > best_solution_QPS:
            best_solution_name = name
            best_solution_QPS = current_solution_QPS
            best_solution_stage_option_list = current_solution_stage_option_list
            best_solution_PE_num_list = current_solution_PE_num_list
            
        print("option name", name)
        print("QPS", current_solution_QPS)
        print("stage_option_list")
        for option in current_solution_stage_option_list:
            print(option)
        print("PE_num_list", current_solution_PE_num_list)

    print("\n\n======== Result =========\n")
    print("best option name", best_solution_name)
    print("QPS", best_solution_QPS)
    print("stage_option_list")
    for option in best_solution_stage_option_list:
        print(option)
    print("PE_num_list", best_solution_PE_num_list)
    print("total_valid_design:", total_valid_design)

    if len(best_solution_stage_option_list) == 5: # without OPQ
        total_consumption_dict = get_resource_consumption(
            best_solution_stage_option_list, 
            PE_num_list=[1,1,1,1,1], 
            count_shell=True)
        print("Total resource consumption:\n{}".format(total_consumption_dict))
        print("Utilization rate:\n{}".format(get_utilization_rate(total_consumption_dict)))
    elif len(best_solution_stage_option_list) == 6: # with OPQ
        total_consumption_dict = get_resource_consumption(
            best_solution_stage_option_list, 
            PE_num_list=[1,1,1,1,1,1], 
            count_shell=True)
        print("Total resource consumption:\n{}".format(total_consumption_dict))
        print("Utilization rate:\n{}".format(get_utilization_rate(total_consumption_dict)))