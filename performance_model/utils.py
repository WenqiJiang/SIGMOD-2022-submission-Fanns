""" Helper functions & Unit test """

from constants import *

def max_of_two(a, b):
    if a >= b:
        return a
    else:
        return b

def max_of_three(a, b, c):
    if a >= b:
        if a >= c:
            return a
        else: 
            return c
    else:
        if b >= c:
            return b
        else:
            return c

def get_bottleneck(perf_resource_dict_list, PE_num_list):
    """
    Given a list of stages (each stage is a perf_resource_dict),
       return (a) which stage (ID in the list) is the bottleneck
              (b) the overall accelerator QPS
    """
    min_QPS = 9999999999
    min_QPS_ID = 0

    for i, perf_resource_dict in enumerate(perf_resource_dict_list):
        if perf_resource_dict["QPS"] * PE_num_list[i] < min_QPS:
            min_QPS = perf_resource_dict["QPS"] * PE_num_list[i]
            min_QPS_ID = i

    assert min_QPS != 9999999999
    accelerator_QPS = min_QPS

    return min_QPS_ID, accelerator_QPS

def resource_consumption_A_less_than_B(
    perf_resource_dict_list_A, PE_num_list_A,
    perf_resource_dict_list_B, PE_num_list_B):

    consumed_HBM_bank_A = 0
    consumed_BRAM_18K_A = 0
    consumed_DSP48E_A = 0
    consumed_FF_A = 0
    consumed_LUT_A = 0
    consumed_URAM_A = 0

    consumed_HBM_bank_B = 0
    consumed_BRAM_18K_B = 0
    consumed_DSP48E_B = 0
    consumed_FF_B = 0
    consumed_LUT_B = 0
    consumed_URAM_B = 0

    for i, perf_resource_dict in enumerate(perf_resource_dict_list_A):

        consumed_HBM_bank_A = consumed_HBM_bank_A + perf_resource_dict["HBM_bank"] * PE_num_list_A[i]
        consumed_BRAM_18K_A = consumed_BRAM_18K_A + perf_resource_dict["BRAM_18K"] * PE_num_list_A[i] 
        consumed_DSP48E_A = consumed_DSP48E_A + perf_resource_dict["DSP48E"] * PE_num_list_A[i] 
        consumed_FF_A = consumed_FF_A + perf_resource_dict["FF"] * PE_num_list_A[i] 
        consumed_LUT_A = consumed_LUT_A + perf_resource_dict["LUT"] * PE_num_list_A[i] 
        consumed_URAM_A = consumed_URAM_A + perf_resource_dict["URAM"] * PE_num_list_A[i] 
    
    for i, perf_resource_dict in enumerate(perf_resource_dict_list_B):

        consumed_HBM_bank_B = consumed_HBM_bank_B + perf_resource_dict["HBM_bank"] * PE_num_list_B[i]
        consumed_BRAM_18K_B = consumed_BRAM_18K_B + perf_resource_dict["BRAM_18K"] * PE_num_list_B[i] 
        consumed_DSP48E_B = consumed_DSP48E_B + perf_resource_dict["DSP48E"] * PE_num_list_B[i] 
        consumed_FF_B = consumed_FF_B + perf_resource_dict["FF"] * PE_num_list_B[i] 
        consumed_LUT_B = consumed_LUT_B + perf_resource_dict["LUT"] * PE_num_list_B[i] 
        consumed_URAM_B = consumed_URAM_B + perf_resource_dict["URAM"] * PE_num_list_B[i] 
    
    # Priority: LUT is the most important one
    if consumed_LUT_A < consumed_LUT_B:
        return True
    else:
        return False

def fit_resource_constraints(perf_resource_dict_list, PE_num_list, count_shell=False):
    """
    Given a list of stages (each stage is a perf_resource_dict),
       return whether it is within the resource constraint
    """
    consumed_HBM_bank = 0
    consumed_BRAM_18K = 0
    consumed_DSP48E = 0
    consumed_FF = 0
    consumed_LUT = 0
    consumed_URAM = 0

    for i, perf_resource_dict in enumerate(perf_resource_dict_list):

        consumed_HBM_bank = consumed_HBM_bank + perf_resource_dict["HBM_bank"] * PE_num_list[i]
        consumed_BRAM_18K = consumed_BRAM_18K + perf_resource_dict["BRAM_18K"] * PE_num_list[i] 
        consumed_DSP48E = consumed_DSP48E + perf_resource_dict["DSP48E"] * PE_num_list[i] 
        consumed_FF = consumed_FF + perf_resource_dict["FF"] * PE_num_list[i] 
        consumed_LUT = consumed_LUT + perf_resource_dict["LUT"] * PE_num_list[i] 
        consumed_URAM = consumed_URAM + perf_resource_dict["URAM"] * PE_num_list[i] 

    if count_shell:
        consumed_HBM_bank += shell_consumption["HBM_bank"]
        consumed_BRAM_18K += shell_consumption["BRAM_18K"]
        consumed_DSP48E += shell_consumption["DSP48E"]
        consumed_FF += shell_consumption["FF"]
        consumed_LUT += shell_consumption["LUT"]
        consumed_URAM += shell_consumption["URAM"]

    if consumed_HBM_bank <= MAX_HBM_bank and consumed_BRAM_18K <= MAX_BRAM_18K and \
        consumed_DSP48E <= MAX_DSP48E and consumed_FF <= MAX_FF and \
        consumed_LUT < MAX_LUT and consumed_URAM < MAX_URAM:
        return True
    else: 
        return False


def get_resource_consumption(perf_resource_dict_list, PE_num_list, count_shell=False):
    """
    Given a list of stages (each stage is a perf_resource_dict),
       return the resource consumption dictionary
    """
    consumed_HBM_bank = 0
    consumed_BRAM_18K = 0
    consumed_DSP48E = 0
    consumed_FF = 0
    consumed_LUT = 0
    consumed_URAM = 0

    for i, perf_resource_dict in enumerate(perf_resource_dict_list):

        consumed_HBM_bank = consumed_HBM_bank + perf_resource_dict["HBM_bank"] * PE_num_list[i]
        consumed_BRAM_18K = consumed_BRAM_18K + perf_resource_dict["BRAM_18K"] * PE_num_list[i] 
        consumed_DSP48E = consumed_DSP48E + perf_resource_dict["DSP48E"] * PE_num_list[i] 
        consumed_FF = consumed_FF + perf_resource_dict["FF"] * PE_num_list[i] 
        consumed_LUT = consumed_LUT + perf_resource_dict["LUT"] * PE_num_list[i] 
        consumed_URAM = consumed_URAM + perf_resource_dict["URAM"] * PE_num_list[i] 

    perf_resource_dict = dict()
    
    if not count_shell:
        perf_resource_dict["HBM_bank"] = consumed_HBM_bank
        perf_resource_dict["BRAM_18K"] = consumed_BRAM_18K
        perf_resource_dict["DSP48E"] = consumed_DSP48E
        perf_resource_dict["FF"] = consumed_FF
        perf_resource_dict["LUT"] = consumed_LUT
        perf_resource_dict["URAM"] = consumed_URAM
    else:
        perf_resource_dict["HBM_bank"] = consumed_HBM_bank + shell_consumption["HBM_bank"]
        perf_resource_dict["BRAM_18K"] = consumed_BRAM_18K + shell_consumption["BRAM_18K"]
        perf_resource_dict["DSP48E"] = consumed_DSP48E + shell_consumption["DSP48E"]
        perf_resource_dict["FF"] = consumed_FF + shell_consumption["FF"]
        perf_resource_dict["LUT"] = consumed_LUT + shell_consumption["LUT"]
        perf_resource_dict["URAM"] = consumed_URAM + shell_consumption["URAM"]

    return perf_resource_dict

def get_utilization_rate(perf_resource_dict):

    utilization_rate = dict()

    utilization_rate["BRAM_18K"] = "{}%".format(perf_resource_dict["BRAM_18K"] / TOTAL_BRAM_18K * 100)
    utilization_rate["DSP48E"] = "{}%".format(perf_resource_dict["DSP48E"] / TOTAL_DSP48E * 100)
    utilization_rate["FF"] = "{}%".format(perf_resource_dict["FF"] / TOTAL_FF * 100)
    utilization_rate["LUT"] = "{}%".format(perf_resource_dict["LUT"] / TOTAL_LUT * 100)
    utilization_rate["URAM"] = "{}%".format(perf_resource_dict["URAM"] / TOTAL_URAM * 100)

    return utilization_rate

def unit_test():
    """ Print the options of each function unit """

    print("\nget_priority_queue_info:\n")
    perf_resource_dict = get_priority_queue_info(queue_len=32, N_insertion=8192)
    print(perf_resource_dict)

    print("\nget_bitonic_sort_16_info:\n")
    perf_resource_dict = get_bitonic_sort_16_info(N_insertion=1e8/8192/64*32)
    print(perf_resource_dict)

    print("\nget_parallel_merge_32_to_16_info:\n")
    perf_resource_dict = get_parallel_merge_32_to_16_info(N_insertion=1e8/8192/64*32)
    print(perf_resource_dict)

    option_list = get_options_preprocessing_OPQ()
    print("\nget_options_preprocessing_OPQ:\n")
    for option in option_list:
        print(option)

    print("\nget_options_stage_1_cluster_distance_computation:\n")
    nlist_options = [2**10, 2**11, 2**12, 2**13, 2**14, 2**15, 2**16, 2**17, 2**18]
    for nlist in nlist_options:
        option_list = get_options_stage_1_cluster_distance_computation(nlist)
        print("nlist={}".format(nlist))
        for option in option_list:
            print(option)

    print("\nget_options_stage_2_select_Voronoi_cells:\n")
    option_list = get_options_stage_2_select_Voronoi_cells(nlist=8192, nprobe=32)
    for option in option_list:
        print(option)

    print("\nget_options_stage_3_distance_LUT_construction:\n")    
    option_list = get_options_stage_3_distance_LUT_construction(nprobe=32)
    for option in option_list:
        print(option)

    print("\nget_options_stage_4_distance_estimation_by_LUT:\n")    
    option_list = get_options_stage_4_distance_estimation_by_LUT(
        PE_num=63, nprobe=32, N_compute_per_nprobe=int(1e8/8192/63))
    for option in option_list:
        print(option)

    # for a small amount of number being scanned, hierachical priority queue is not
    #   really an option
    print("\nget_options_stage_5_sort_reduction:\n")    
    # for small number of Voronoi cells, only 2 level is required
    print("nlist=8192, nprobe=32, nstreams=64")
    option_list = get_options_stage_5_sort_reduction(
        input_stream_num=64, 
        N_insertion_per_stream=int(1e8/8192*32/64))
    for option in option_list:
        print(option)
    # for large number of Voronoi cells, 4 level is required
    print("nlist=262144, nprobe=32, nstreams=64")
    option_list = get_options_stage_5_sort_reduction(
        input_stream_num=64, 
        N_insertion_per_stream=int(1e8/262144*32/64))
    for option in option_list:
        print(option)
    # try different stream num
    print("nlist=8192, nprobe=32, nstreams=48")
    option_list = get_options_stage_5_sort_reduction(
        input_stream_num=48, 
        N_insertion_per_stream=int(1e8/8192*32/64))
    for option in option_list:
        print(option)
    print("nlist=8192, nprobe=32, nstreams=32")
    option_list = get_options_stage_5_sort_reduction(
        input_stream_num=32, 
        N_insertion_per_stream=int(1e8/8192*32/64))
    for option in option_list:
        print(option)
    print("nlist=8192, nprobe=32, nstreams=16")
    option_list = get_options_stage_5_sort_reduction(
        input_stream_num=16, 
        N_insertion_per_stream=int(1e8/8192*32/64))
    for option in option_list:
        print(option)
