from constants import * 

""" Basic building blocks, i.e., priority queue and sorting network """
def get_priority_queue_info(queue_len, N_insertion):
    
    """ Return a single result (no multiple options) """

    perf_resource_dict = dict()

    # Performance
    L_control = 3 
    L_insertion=5
    II_insertion=2
    L_perf_resource_dictput=2
    N_output=queue_len
    II_output=1

    cycles_per_query = L_control + (L_insertion + N_insertion * II_insertion) + (L_perf_resource_dictput + N_output * II_output)
    QPS = 1 / (cycles_per_query / FREQ)

    perf_resource_dict['cycles_per_query'] = cycles_per_query
    perf_resource_dict['QPS'] = QPS

    # The resource consumption is almost linear to the queue_len

    #####  HLS Prediction #####
    # perf_resource_dict["HBM_bank"] = 0
    # perf_resource_dict["BRAM_18K"] = 0
    # perf_resource_dict["DSP48E"] = 0
    # perf_resource_dict["FF"] = 2177 / 10 * queue_len
    # perf_resource_dict["LUT"] = 3597 / 10 * queue_len 
    # perf_resource_dict["URAM"] = 0

    #####  Vivado Measured #####
    perf_resource_dict["LUT"] = 1830 / 10 * queue_len
    perf_resource_dict["FF"] = 2172 / 10 * queue_len
    perf_resource_dict["BRAM_18K"] = 2 * 0
    perf_resource_dict["URAM"] = 0
    perf_resource_dict["DSP48E"] = 0
    perf_resource_dict["HBM_bank"] = 0

    return perf_resource_dict
    
def get_bitonic_sort_16_info(N_insertion):

    """ Return a single result (no multiple options) """

    perf_resource_dict = dict()

    # Performance
    L_insertion = 12
    II_insertion = 1
    cycles_per_query = L_insertion + N_insertion * II_insertion
    QPS = 1 / (cycles_per_query / FREQ)
    perf_resource_dict['cycles_per_query'] = cycles_per_query
    perf_resource_dict['QPS'] = QPS

    # Resource
    #####  HLS Prediction #####
    # perf_resource_dict["HBM_bank"] = 0
    # perf_resource_dict["BRAM_18K"] = 0
    # perf_resource_dict["DSP48E"] = 0
    # perf_resource_dict["FF"] = 15693
    # perf_resource_dict["LUT"] = 20373 
    # perf_resource_dict["URAM"] = 0

    #####  Vivado Measured #####
    perf_resource_dict["LUT"] = 10223
    perf_resource_dict["FF"] = 15561
    perf_resource_dict["BRAM_18K"] = 2 * 0
    perf_resource_dict["URAM"] = 0
    perf_resource_dict["DSP48E"] = 0
    perf_resource_dict["HBM_bank"] = 0

    return perf_resource_dict


def get_parallel_merge_32_to_16_info(N_insertion):

    """ Return a single result (no multiple options) """

    perf_resource_dict = dict()

    # Performance
    L_insertion = 7
    II_insertion = 1
    cycles_per_query = L_insertion + N_insertion * II_insertion
    QPS = 1 / (cycles_per_query / FREQ)
    perf_resource_dict['cycles_per_query'] = cycles_per_query
    perf_resource_dict['QPS'] = QPS

    # Resource
    #####  HLS Prediction #####
    # perf_resource_dict["HBM_bank"] = 0
    # perf_resource_dict["BRAM_18K"] = 0
    # perf_resource_dict["DSP48E"] = 0
    # perf_resource_dict["FF"] = 9480
    # perf_resource_dict["LUT"] = 11861 
    # perf_resource_dict["URAM"] = 0

    #####  Vivado Measured #####
    perf_resource_dict["LUT"] = 5588
    perf_resource_dict["FF"] = 9374
    perf_resource_dict["BRAM_18K"] = 2 * 0
    perf_resource_dict["URAM"] = 0
    perf_resource_dict["DSP48E"] = 0
    perf_resource_dict["HBM_bank"] = 0

    return perf_resource_dict
