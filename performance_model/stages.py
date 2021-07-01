import numpy as np

from constants import * 
from queue_and_sorting import * 
from utils import * 

""" The 6 Stages (optional preprocessing + 5 stages) """

def get_options_stage_1_OPQ():
    
    option_list = []

    """ Option 1: UNROLL 8 """
    perf_resource_dict = dict()
    perf_resource_dict["name"] = "UNROLL 8"

    L_load = 128
    L_write = 128
    L_compute = 37
    UNROLL_FACTOR = 8
    II_compute = 1
    cycles_per_query = L_load + (L_compute + (D * D) / UNROLL_FACTOR * II_compute) + L_write
    QPS = 1 / (cycles_per_query / FREQ)
    perf_resource_dict['cycles_per_query'] = cycles_per_query
    perf_resource_dict['QPS'] = QPS
    
    #####  HLS Prediction #####
    perf_resource_dict["HBM_bank"] = 0
    perf_resource_dict["BRAM_18K"] = 33
    perf_resource_dict["DSP48E"] = 40
    perf_resource_dict["FF"] = 5134
    perf_resource_dict["LUT"] = 3660 
    perf_resource_dict["URAM"] = 0

    option_list.append(perf_resource_dict)

    """ Option 2: UNROLL 4 """
    perf_resource_dict = dict()
    perf_resource_dict["name"] = "UNROLL 4"

    L_load = 128
    L_write = 128
    L_compute = 21
    UNROLL_FACTOR = 4
    II_compute = 1
    cycles_per_query = L_load + (L_compute + (D * D) / UNROLL_FACTOR * II_compute) + L_write
    QPS = 1 / (cycles_per_query / FREQ)
    perf_resource_dict['cycles_per_query'] = cycles_per_query
    perf_resource_dict['QPS'] = QPS
   
    #####  HLS Prediction ##### 
    perf_resource_dict["HBM_bank"] = 0
    perf_resource_dict["BRAM_18K"] = 37
    perf_resource_dict["DSP48E"] = 20
    perf_resource_dict["FF"] = 2659
    perf_resource_dict["LUT"] = 2152 
    perf_resource_dict["URAM"] = 0

    option_list.append(perf_resource_dict)

    """ Option 3: UNROLL 2 """
    perf_resource_dict = dict()
    perf_resource_dict["name"] = "UNROLL 2"

    L_load = 128
    L_write = 128
    L_compute = 13
    UNROLL_FACTOR = 2
    II_compute = 1
    cycles_per_query = L_load + (L_compute + (D * D) / UNROLL_FACTOR * II_compute) + L_write
    QPS = 1 / (cycles_per_query / FREQ)
    perf_resource_dict['cycles_per_query'] = cycles_per_query
    perf_resource_dict['QPS'] = QPS

    #####  HLS Prediction #####
    # perf_resource_dict["HBM_bank"] = 0
    # perf_resource_dict["BRAM_18K"] = 33
    # perf_resource_dict["DSP48E"] = 10
    # perf_resource_dict["FF"] = 1534
    # perf_resource_dict["LUT"] = 1428 
    # perf_resource_dict["URAM"] = 0

    #####  Vivado Measured #####
    perf_resource_dict["LUT"] = 1479
    perf_resource_dict["FF"] = 1646
    perf_resource_dict["BRAM_18K"] = 2 * 18.5
    perf_resource_dict["URAM"] = 0
    perf_resource_dict["DSP48E"] = 20
    perf_resource_dict["HBM_bank"] = 0

    option_list.append(perf_resource_dict)

    """ Option 4: no UNROLL  """
    perf_resource_dict = dict()
    perf_resource_dict["name"] = "no UNROL"

    L_load = 128
    L_write = 128
    L_compute = 9
    UNROLL_FACTOR = 1
    II_compute = 1
    cycles_per_query = L_load + (L_compute + (D * D) / UNROLL_FACTOR * II_compute) + L_write
    QPS = 1 / (cycles_per_query / FREQ)
    perf_resource_dict['cycles_per_query'] = cycles_per_query
    perf_resource_dict['QPS'] = QPS
   
    #####  HLS Prediction ##### 
    perf_resource_dict["HBM_bank"] = 0
    perf_resource_dict["BRAM_18K"] = 31
    perf_resource_dict["DSP48E"] = 5
    perf_resource_dict["FF"] = 1055
    perf_resource_dict["LUT"] = 1125 
    perf_resource_dict["URAM"] = 0

    option_list.append(perf_resource_dict)

    return option_list

def get_options_stage_2_cluster_distance_computation(nlist):
    
    """ The performance / resource of a single PE
        currently only the most optimized option is included """

    option_list = []

    """ UNUSED Broadcast-Gather Pattern """
    """
    for PE_num in range(1, 128):

        perf_resource_dict = dict()
        perf_resource_dict["name"] = "distance_computation_PE_unoptimized {} PEs".format(PE_num)

        centroids_Per_PE = int(np.ceil(nlist / PE_num))
        N_comp = int(np.ceil(nlist * 8 / PE_num)) # 8 = SIMD width
        L_load = 128
        L_comp = 81
        II_comp = 3
        cycles_per_query = L_load + (L_comp + N_comp * II_comp)
        QPS = 1 / (cycles_per_query / FREQ)
        perf_resource_dict['cycles_per_query'] = cycles_per_query
        perf_resource_dict['QPS'] = QPS

        #####  HLS Prediction #####
        # perf_resource_dict["BRAM_18K"] = 0
        # perf_resource_dict["DSP48E"] = 40 * PE_num
        # perf_resource_dict["FF"] = 7288 * PE_num
        # perf_resource_dict["LUT"] = 5456 * PE_num

        #####  Vivado Measured #####
        perf_resource_dict["LUT"] = 5376 * PE_num
        perf_resource_dict["FF"] = 6316 * PE_num
        perf_resource_dict["BRAM_18K"] = 2 * 0 * PE_num
        perf_resource_dict["DSP48E"] = 40 * PE_num

        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * PE_num * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * PE_num * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * PE_num * 2

        # on-chip or off-chip storage given different nlist size
        i = np.ceil(centroids_Per_PE / 256) # centroids_Per_PE <= 256, URAM = 8
        if i * 8 * PE_num < MAX_URAM:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = i * 8 * PE_num
        else:
            #####  HLS Prediction & Vivado Measured #####
            # Per PE: SIMD width = 16 float = 512 bit, II=3, then
            perf_resource_dict["HBM_bank"] = int(np.ceil(PE_num / 3))
            perf_resource_dict["URAM"] = 0

        if (fit_resource_constraints([perf_resource_dict], [1])):
            option_list.append(perf_resource_dict)
    """

    """ Systolic array """
    for PE_num in range(1, 128):

        perf_resource_dict = dict()
        perf_resource_dict["name"] = "distance_computation_PE_systolic_optimized {} PEs".format(PE_num)

        centroids_Per_PE = int(np.ceil(nlist / PE_num))
        N_comp = int(np.ceil(nlist * 8 / PE_num)) # 8 = SIMD width
        L_load = 128
        L_comp = 81
        II_comp = 3
        cycles_per_query = L_load + (L_comp + N_comp * II_comp)
        QPS = 1 / (cycles_per_query / FREQ)
        # when forwarding results become the performance bottleneck
        MAX_QPS = 1 / (PE_num / FREQ)
        if QPS > MAX_QPS:
            QPS = MAX_QPS
            cycles_per_query = PE_num
        perf_resource_dict['cycles_per_query'] = cycles_per_query
        perf_resource_dict['QPS'] = QPS

        #####  Vivado Measured #####
        perf_resource_dict["LUT"] = 5202 * PE_num
        perf_resource_dict["FF"] = 6246 * PE_num
        perf_resource_dict["BRAM_18K"] = 2 * 0 * PE_num
        perf_resource_dict["DSP48E"] = 40 * PE_num

        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 2 + component["FIFO_d2_w32"]["LUT"] * 4 * PE_num
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 2 + component["FIFO_d2_w32"]["FF"] * 4 * PE_num
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 2 + component["FIFO_d2_w32"]["BRAM_18K"] * 4 * PE_num

        # on-chip or off-chip storage given different nlist size
        i = np.ceil(centroids_Per_PE / 512) # centroids_Per_PE <= 512, URAM = 8
        if i * 8 * PE_num < MAX_URAM:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = i * 8 * PE_num
        else:
            #####  HLS Prediction & Vivado Measured #####
            # Per PE: SIMD width = 16 float = 512 bit, II=3, then
            perf_resource_dict["HBM_bank"] = int(np.ceil(PE_num / 3))
            perf_resource_dict["URAM"] = 0

        if (fit_resource_constraints([perf_resource_dict], [1])):
            option_list.append(perf_resource_dict)

    return option_list

def get_options_stage_3_select_Voronoi_cells(nlist, nprobe):
    
    """ 
    Insertion_per_cycle should equal to the PE num of stage 2, suppose
        it can output 1 distance per cycle  
    Here, we try insertion_per_cycle from 1 to 8, that should cover most cases
        (larger insertion per cycle will lead to overutilization of stage 2 PEs)
    """

    option_list = []

    """ Option 1: single priority queue """
    queue_size = nprobe
    N_insertion_per_queue = nlist
    perf_resource_dict = get_priority_queue_info(queue_size, N_insertion_per_queue)
    perf_resource_dict["name"] = "single prirority queue"
       
    #####   FIFO Consumption (Vivado Measured)   #####
    perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] 
    perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] 
    perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] 

    option_list.append(perf_resource_dict)
    
    """ Option 2: hierachical priority queue """
    for insertion_per_cycle in range(1, 8 + 1):
        queue_size_level_A = nprobe
        queue_size_level_B = nprobe
        # 2 level of queues, 
        #  the first level collect the output of stage 2 in parallel
        #  the second level collect the result of level 1
        queue_num_level_A = int(insertion_per_cycle * 2)
        queue_num_level_B = 1
        N_insertion_per_queue_level_A = int(nlist / queue_num_level_A)
        N_insertion_level_B = int(queue_num_level_A * queue_size_level_A)

        perf_resource_dict_level_A = get_priority_queue_info(queue_size_level_A, N_insertion_per_queue_level_A)
        perf_resource_dict_level_B = get_priority_queue_info(queue_size_level_B, N_insertion_level_B)

        perf_resource_dict = dict()
        perf_resource_dict["name"] = "hierachical prirority queue level A: {} queues, level B: {} queue".format(
            queue_num_level_A, queue_num_level_B)

        if perf_resource_dict_level_A['cycles_per_query'] > perf_resource_dict_level_B['cycles_per_query']:
            perf_resource_dict['cycles_per_query'] = perf_resource_dict_level_A['cycles_per_query'] 
        else:
            perf_resource_dict['cycles_per_query'] = perf_resource_dict_level_B['cycles_per_query'] 
        perf_resource_dict['QPS'] = 1 / (perf_resource_dict['cycles_per_query'] / FREQ) 
        
        perf_resource_dict["BRAM_18K"] = queue_num_level_A * perf_resource_dict_level_A["BRAM_18K"] + \
            queue_num_level_B * perf_resource_dict_level_B["BRAM_18K"]
        perf_resource_dict["DSP48E"] = queue_num_level_A * perf_resource_dict_level_A["DSP48E"] + \
            queue_num_level_B * perf_resource_dict_level_B["DSP48E"]
        perf_resource_dict["FF"] = queue_num_level_A * perf_resource_dict_level_A["FF"] + \
            queue_num_level_B * perf_resource_dict_level_B["FF"]
        perf_resource_dict["LUT"] = queue_num_level_A * perf_resource_dict_level_A["LUT"] + \
            queue_num_level_B * perf_resource_dict_level_B["LUT"]
        perf_resource_dict["HBM_bank"] = queue_num_level_A * perf_resource_dict_level_A["HBM_bank"] + \
            queue_num_level_B * perf_resource_dict_level_B["HBM_bank"]
        perf_resource_dict["URAM"] = queue_num_level_A * perf_resource_dict_level_A["URAM"] + \
            queue_num_level_B * perf_resource_dict_level_B["URAM"]

        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * (queue_num_level_A + queue_num_level_B) * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * (queue_num_level_A + queue_num_level_B) * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * (queue_num_level_A + queue_num_level_B) * 2

        option_list.append(perf_resource_dict)

    return option_list

def get_options_stage_4_distance_LUT_construction(nlist, nprobe):
    
    """ Now we only use the most optimized version, i.e.,
          multiple_lookup_table_construction_PEs_optimized_version4_systolic """

    option_list = []

    for PE_num in range(1, 128):

        perf_resource_dict = dict()
        perf_resource_dict["name"] = "multiple_lookup_table_construction_PEs_optimized_version4_systolic {} PEs".format(PE_num)

        nprobe_per_PE_max = int(np.ceil(nprobe / PE_num))
        L_load_query = 128
        L_load_and_compute_residual = 132
        L_compute = 68
        N_comupte = 256
        II_compute = 16
        cycles_per_query = L_load_query + nprobe_per_PE_max * (L_load_and_compute_residual + L_compute + N_comupte * II_compute)
        QPS = 1 / (cycles_per_query / FREQ)
        perf_resource_dict['cycles_per_query'] = cycles_per_query
        perf_resource_dict['QPS'] = QPS
        
        #####  HLS Prediction #####
        # perf_resource_dict["BRAM_18K"] = 1 * PE_num
        # perf_resource_dict["DSP48E"] = 54 * PE_num
        # perf_resource_dict["FF"] = 7497 * PE_num
        # perf_resource_dict["LUT"] = 6233 * PE_num
        # perf_resource_dict["URAM"] = 8 * PE_num

        #####   Vivado Measured   #####
        perf_resource_dict["LUT"] = 6128 * PE_num
        perf_resource_dict["FF"] = 7577 * PE_num
        perf_resource_dict["BRAM_18K"] = 2 * 9 * PE_num
        perf_resource_dict["URAM"] = 8 * PE_num
        perf_resource_dict["DSP48E"] = 54 * PE_num

        if nlist <= 1024:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 16
        elif nlist <= 2048:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 32
        elif nlist <= 4096:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 64
        elif nlist <= 8192:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 128
        elif nlist <= 16384:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 256
        elif nlist <= 32768:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 512
        else:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 1

        #####   FIFO Consumption (Vivado Measured)   #####

        # PQ quantizer init
        perf_resource_dict["LUT"] += component["FIFO_d2_w32"]["LUT"] * PE_num
        perf_resource_dict["FF"] += component["FIFO_d2_w32"]["FF"] * PE_num
        perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w32"]["BRAM_18K"] * PE_num

        # forward query vector + center vector dispatcher
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 2 * PE_num
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 2 * PE_num
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 2 * PE_num

        # output FIFO to stage 5
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 16 * int(np.ceil(PE_num / 2))
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 16 * int(np.ceil(PE_num / 2))
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 16 * int(np.ceil(PE_num / 2))

        # LUT forward between PEs
        perf_resource_dict["LUT"] += component["FIFO_d2_w512"]["LUT"] * PE_num
        perf_resource_dict["FF"] += component["FIFO_d2_w512"]["FF"] * PE_num
        perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w512"]["BRAM_18K"] * PE_num

        if (fit_resource_constraints([perf_resource_dict], [1])):
            option_list.append(perf_resource_dict)

    """ The large version of PE fail placement & routing after adding network stack """    
    """ Plus UNOPTIMIZED centroid lookup URAM """
    """
    for PE_num in range(1, 128):

        perf_resource_dict = dict()
        perf_resource_dict["name"] = "multiple_lookup_table_construction_PEs_unoptimized_large {} PEs".format(PE_num)

        nprobe_per_PE_max = int(np.ceil(nprobe / PE_num))
        L_load_query = 128
        L_load_and_compute_residual = 132
        L_compute = 39
        N_comupte = 256
        II_compute = 4
        cycles_per_query = L_load_query + nprobe_per_PE_max * (L_load_and_compute_residual + L_compute + N_comupte * II_compute)
        QPS = 1 / (cycles_per_query / FREQ)
        perf_resource_dict['cycles_per_query'] = cycles_per_query
        perf_resource_dict['QPS'] = QPS
        
        #####  HLS Prediction #####
        # perf_resource_dict["BRAM_18K"] = 1 * PE_num
        # perf_resource_dict["DSP48E"] = 216 * PE_num
        # perf_resource_dict["FF"] = 40288 * PE_num
        # perf_resource_dict["LUT"] = 27176 * PE_num
        # perf_resource_dict["URAM"] = 16 * PE_num

        #####   Vivado Measured   #####
        perf_resource_dict["LUT"] = 24746 * PE_num
        perf_resource_dict["FF"] = 35870 * PE_num
        perf_resource_dict["BRAM_18K"] = 2 * 0.5 * PE_num
        perf_resource_dict["URAM"] = 16 * PE_num
        perf_resource_dict["DSP48E"] = 216 * PE_num

        if nlist <= 1024:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 32
        elif nlist <= 2048:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 64
        elif nlist <= 4096:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 128
        elif nlist <= 8192:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 256
        elif nlist <= 16384:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 0
            perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 512
        else:
            #####  HLS Prediction & Vivado Measured #####
            perf_resource_dict["HBM_bank"] = 1

        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 17 * PE_num
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 17 * PE_num
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 17 * PE_num

        if (fit_resource_constraints([perf_resource_dict], [1])):
            option_list.append(perf_resource_dict)
    """


    """ Medium version of PEs might survive placement & routing, although much less resource efficient """
    """ Plus optimized centroid lookup URAM """
    # for PE_num in range(1, 128):

    #     perf_resource_dict = dict()
    #     perf_resource_dict["name"] = "multiple_lookup_table_construction_PEs_unoptimized_medium {} PEs".format(PE_num)

    #     nprobe_per_PE_max = int(np.ceil(nprobe / PE_num))
    #     L_load_query = 128
    #     L_load_and_compute_residual = 132
    #     L_compute = 53
    #     N_comupte = 256
    #     II_compute = 8
    #     cycles_per_query = L_load_query + nprobe_per_PE_max * (L_load_and_compute_residual + L_compute + N_comupte * II_compute)
    #     QPS = 1 / (cycles_per_query / FREQ)
    #     perf_resource_dict['cycles_per_query'] = cycles_per_query
    #     perf_resource_dict['QPS'] = QPS
        
    #     #####  HLS Prediction #####
    #     # perf_resource_dict["BRAM_18K"] = 1 * PE_num
    #     # perf_resource_dict["DSP48E"] = 216 * PE_num
    #     # perf_resource_dict["FF"] = 40288 * PE_num
    #     # perf_resource_dict["LUT"] = 27176 * PE_num
    #     # perf_resource_dict["URAM"] = 16 * PE_num

    #     #####   Vivado Measured   #####
    #     perf_resource_dict["LUT"] = 19541 * PE_num
    #     perf_resource_dict["FF"] = 27767 * PE_num
    #     perf_resource_dict["BRAM_18K"] = 2 * 0.5 * PE_num
    #     perf_resource_dict["URAM"] = 16 * PE_num
    #     perf_resource_dict["DSP48E"] = 108 * PE_num

    #     if nlist <= 1024:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 16
    #     elif nlist <= 2048:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 32
    #     elif nlist <= 4096:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 64
    #     elif nlist <= 8192:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 128
    #     elif nlist <= 16384:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 256
    #     elif nlist <= 32768:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 512
    #     else:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 1

    #     #####   FIFO Consumption (Vivado Measured)   #####
    #     perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 17 * PE_num
    #     perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 17 * PE_num
    #     perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 17 * PE_num

    #     if (fit_resource_constraints([perf_resource_dict], [1])):
    #         option_list.append(perf_resource_dict)

    """ Use smaller version of PEs to survive placement & routing, although much less resource efficient """
    """ Plus optimized centroid lookup URAM """
    # for PE_num in range(1, 128):

    #     perf_resource_dict = dict()
    #     perf_resource_dict["name"] = "multiple_lookup_table_construction_PEs_unoptimized_small {} PEs".format(PE_num)

    #     nprobe_per_PE_max = int(np.ceil(nprobe / PE_num))
    #     L_load_query = 128
    #     L_load_and_compute_residual = 132
    #     L_compute = 68
    #     N_comupte = 256
    #     II_compute = 16
    #     cycles_per_query = L_load_query + nprobe_per_PE_max * (L_load_and_compute_residual + L_compute + N_comupte * II_compute)
    #     QPS = 1 / (cycles_per_query / FREQ)
    #     perf_resource_dict['cycles_per_query'] = cycles_per_query
    #     perf_resource_dict['QPS'] = QPS
        
    #     #####  HLS Prediction #####
    #     # perf_resource_dict["BRAM_18K"] = 1 * PE_num
    #     # perf_resource_dict["DSP48E"] = 216 * PE_num
    #     # perf_resource_dict["FF"] = 40288 * PE_num
    #     # perf_resource_dict["LUT"] = 27176 * PE_num
    #     # perf_resource_dict["URAM"] = 16 * PE_num

    #     #####   Vivado Measured   #####
    #     perf_resource_dict["LUT"] = 15288 * PE_num
    #     perf_resource_dict["FF"] = 21875 * PE_num
    #     perf_resource_dict["BRAM_18K"] = 2 * 0.5 * PE_num
    #     perf_resource_dict["URAM"] = 16 * PE_num
    #     perf_resource_dict["DSP48E"] = 54 * PE_num

    #     if nlist <= 1024:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 16
    #     elif nlist <= 2048:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 32
    #     elif nlist <= 4096:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 64
    #     elif nlist <= 8192:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 128
    #     elif nlist <= 16384:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 256
    #     elif nlist <= 32768:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 0
    #         perf_resource_dict["URAM"] = perf_resource_dict["URAM"] + 512
    #     else:
    #         #####  HLS Prediction & Vivado Measured #####
    #         perf_resource_dict["HBM_bank"] = 1

    #     #####   FIFO Consumption (Vivado Measured)   #####
    #     perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 17 * PE_num
    #     perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 17 * PE_num
    #     perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 17 * PE_num

    #     if (fit_resource_constraints([perf_resource_dict], [1])):
    #         option_list.append(perf_resource_dict)

    return option_list

def get_options_stage_5_distance_estimation_by_LUT(PE_num, nprobe, N_compute_per_nprobe):
    
    """ this function returns the performance and resource consumption of
          the entire systolic array """

    option_list = []

    perf_resource_dict = dict()
    perf_resource_dict["name"] = "systolic array with {} PEs".format(PE_num)

    L_load = 2
    N_load = 256
    II_load = 1
    L_compute = 63
    II_compute = 1
    cycles_per_query = \
        nprobe * ((L_load + N_load * II_load + PE_num - 1) + \
            (L_compute + N_compute_per_nprobe * II_compute))
    QPS = 1 / (cycles_per_query / FREQ)
    perf_resource_dict['cycles_per_query'] = cycles_per_query
    perf_resource_dict['QPS'] = QPS

    #####  HLS Prediction #####
    # perf_resource_dict["HBM_bank"] = 0 * PE_num 
    # perf_resource_dict["BRAM_18K"] = 16 * PE_num
    # perf_resource_dict["DSP48E"] = 30 * PE_num
    # perf_resource_dict["FF"] = 5437 * PE_num
    # perf_resource_dict["LUT"] = 5329 * PE_num
    # perf_resource_dict["URAM"] = 0 * PE_num

    #####   Vivado Measured   #####
    perf_resource_dict["LUT"] = 3937 * PE_num
    perf_resource_dict["FF"] = 3954 * PE_num
    perf_resource_dict["BRAM_18K"] = 2 * 8 * PE_num
    perf_resource_dict["URAM"] = 0 * PE_num
    perf_resource_dict["DSP48E"] = 30 * PE_num
    perf_resource_dict["HBM_bank"] = 0 * PE_num

    #####   FIFO Consumption (Vivado Measured)   #####
    perf_resource_dict["LUT"] += component["FIFO_d2_w8"]["LUT"] * 16 * PE_num
    perf_resource_dict["FF"] += component["FIFO_d2_w8"]["FF"] * 16 * PE_num
    perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w8"]["BRAM_18K"] * 16 * PE_num

    perf_resource_dict["LUT"] += component["FIFO_d2_w32"]["LUT"] * 19 * 3 * (PE_num / 3)
    perf_resource_dict["FF"] += component["FIFO_d2_w32"]["FF"] * 19 * 3 * (PE_num / 3)
    perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w32"]["BRAM_18K"] * 19 * 3 * (PE_num / 3)

    perf_resource_dict["LUT"] += component["FIFO_d2_w512"]["LUT"] * (PE_num / 3)
    perf_resource_dict["FF"] += component["FIFO_d2_w512"]["FF"] * (PE_num / 3)
    perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w512"]["BRAM_18K"] * (PE_num / 3)


    #####   AXI interface & Parser (Vivado Measured)   #####
    # AXI interface
    perf_resource_dict["LUT"] += 1159 * (PE_num / 3)
    perf_resource_dict["FF"] += 3117 * (PE_num / 3)
    perf_resource_dict["BRAM_18K"] += 2 * 7.5 * (PE_num / 3)
    perf_resource_dict["URAM"] += 0 * (PE_num / 3)
    perf_resource_dict["DSP48E"] += 0 * (PE_num / 3)
    perf_resource_dict["HBM_bank"] += 1 * (PE_num / 3)

    # Type conversion (axi512 -> tuples paser)
    perf_resource_dict["LUT"] += 290 * (PE_num / 3)
    perf_resource_dict["FF"] += 1070 * (PE_num / 3)
    perf_resource_dict["BRAM_18K"] += 2 * 0 * (PE_num / 3)
    perf_resource_dict["URAM"] += 0 * (PE_num / 3)
    perf_resource_dict["DSP48E"] += 0 * (PE_num / 3)
    perf_resource_dict["HBM_bank"] += 0 * (PE_num / 3)

    option_list.append(perf_resource_dict)

    return option_list


def get_options_stage6_sort_reduction(input_stream_num, N_insertion_per_stream):

    """
        return 1 or 2 options 
        TODO: constraint: HBM_channel_num_for_PQ_code * 3 ~= input_stream_num
    """

    option_list = []

    """ Option 1: hierachical priority queue """
    queue_num_level_A = int(input_stream_num * 2)
    N_insertion_per_stream_level_A = int(N_insertion_per_stream / 2)
    perf_resource_dict_level_A = get_priority_queue_info(
        queue_len=topK, N_insertion=N_insertion_per_stream_level_A)

    def find_num_queues(cycles_per_query_upper_level, N_insertion_total_level, upper_level_queue_num):
        """ find an option that the lower level can match the upper level's performance """
        queue_num_level_N = 1
        for i in range(1, upper_level_queue_num):
            perf_resource_dict_level_N = get_priority_queue_info(
                queue_len=topK, N_insertion=int(N_insertion_total_level / queue_num_level_N))
            if perf_resource_dict_level_N["cycles_per_query"] > cycles_per_query_upper_level:
                queue_num_level_N = queue_num_level_N + 1
            else:
                return (queue_num_level_N, perf_resource_dict_level_N)
        return False

    def find_hierachical_queue_structure_recursive(
            cycles_per_query_upper_level, N_insertion_total_level, upper_level_queue_num):

        if not find_num_queues(cycles_per_query_upper_level, N_insertion_total_level, upper_level_queue_num):
            return False
        else: 
            queue_num_level_N, perf_resource_dict_level_N = \
                find_num_queues(cycles_per_query_upper_level, N_insertion_total_level, upper_level_queue_num)

            if queue_num_level_N == 1:
                return [1]
            else: 
                queue_num_array = find_hierachical_queue_structure_recursive(
                    cycles_per_query_upper_level=perf_resource_dict_level_N["cycles_per_query"],
                    N_insertion_total_level=int(queue_num_level_N * topK),
                    upper_level_queue_num=queue_num_level_N)
                if queue_num_array:
                    queue_num_array.append(queue_num_level_N)
                else:
                    return False

            return queue_num_array


    # if lower level cannot reduce the number of queues used to match upper level, 
    #   then hierarchical priority queue is not an option
    queue_num_array = find_hierachical_queue_structure_recursive(
        cycles_per_query_upper_level=perf_resource_dict_level_A["cycles_per_query"], 
        N_insertion_total_level=int(queue_num_level_A * topK),
        upper_level_queue_num=queue_num_level_A)

    if queue_num_array:

        queue_num_array.append(queue_num_level_A)

        total_priority_queue_num = np.sum(np.array(queue_num_array))

        perf_resource_dict = dict()
        perf_resource_dict["name"] = "hierarchical priority queue: {} total: {}".format(
            queue_num_array, total_priority_queue_num)

        # all levels use the same priority queue of depth=topK
        perf_resource_dict["HBM_bank"] = \
            total_priority_queue_num * perf_resource_dict_level_A["HBM_bank"]
        perf_resource_dict["BRAM_18K"] = \
            total_priority_queue_num * perf_resource_dict_level_A["BRAM_18K"]
        perf_resource_dict["DSP48E"] = \
            total_priority_queue_num * perf_resource_dict_level_A["DSP48E"]
        perf_resource_dict["FF"] = \
            total_priority_queue_num * perf_resource_dict_level_A["FF"]
        perf_resource_dict["LUT"] = \
            total_priority_queue_num * perf_resource_dict_level_A["LUT"]
        perf_resource_dict["URAM"] = \
            total_priority_queue_num * perf_resource_dict_level_A["URAM"]

        # lower level is faster than upper level
        perf_resource_dict["cycles_per_query"] = perf_resource_dict_level_A["cycles_per_query"]
        perf_resource_dict["QPS"] = 1 / (perf_resource_dict["cycles_per_query"] / FREQ)
   
        #####   FIFO Consumption (Vivado Measured)   #####
        # Note I use depth=512 here, in practical it could be smaller but will consume more LUT/FF
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * np.sum(queue_num_array) * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * np.sum(queue_num_array) * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * np.sum(queue_num_array) * 2

        option_list.append(perf_resource_dict) 

    """ Option 2: sort reduction network """
    perf_resource_dict_bitonic_sort_16 = \
        get_bitonic_sort_16_info(N_insertion=N_insertion_per_stream)
    perf_resource_dict_parallel_merge_32_to_16 = \
        get_parallel_merge_32_to_16_info(N_insertion=N_insertion_per_stream)

    if input_stream_num <= 16:

        perf_resource_dict = dict()
        perf_resource_dict["name"] = \
            "sort reduction (1 sort 0 merge) + priority queue group (21)"

        perf_resource_dict["HBM_bank"] = \
            21 * perf_resource_dict_level_A["HBM_bank"] + \
            1 * perf_resource_dict_bitonic_sort_16["HBM_bank"] + \
            0 * perf_resource_dict_parallel_merge_32_to_16["HBM_bank"]
        perf_resource_dict["BRAM_18K"] = \
            21 * perf_resource_dict_level_A["BRAM_18K"] + \
            1 * perf_resource_dict_bitonic_sort_16["BRAM_18K"] + \
            0 * perf_resource_dict_parallel_merge_32_to_16["BRAM_18K"]
        perf_resource_dict["DSP48E"] = \
            21 * perf_resource_dict_level_A["DSP48E"] + \
            1 * perf_resource_dict_bitonic_sort_16["DSP48E"] + \
            0 * perf_resource_dict_parallel_merge_32_to_16["DSP48E"]
        perf_resource_dict["FF"] = \
            21 * perf_resource_dict_level_A["FF"] + \
            1 * perf_resource_dict_bitonic_sort_16["FF"] + \
            0 * perf_resource_dict_parallel_merge_32_to_16["FF"]
        perf_resource_dict["LUT"] = \
            21 * perf_resource_dict_level_A["LUT"] + \
            1 * perf_resource_dict_bitonic_sort_16["LUT"] + \
            0 * perf_resource_dict_parallel_merge_32_to_16["LUT"]
        perf_resource_dict["URAM"] = \
            21 * perf_resource_dict_level_A["URAM"] + \
            1 * perf_resource_dict_bitonic_sort_16["URAM"] + \
            0 * perf_resource_dict_parallel_merge_32_to_16["URAM"]
           
        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 2

        perf_resource_dict["LUT"] += component["FIFO_d2_w32"]["LUT"] * 152
        perf_resource_dict["FF"] += component["FIFO_d2_w32"]["FF"] * 152
        perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w32"]["BRAM_18K"] * 152
        

        perf_resource_dict["cycles_per_query"] = N_insertion_per_stream
        perf_resource_dict["QPS"] = 1 / (perf_resource_dict["cycles_per_query"] / FREQ)

        option_list.append(perf_resource_dict) 

    elif input_stream_num > 16 and input_stream_num <= 32:

        perf_resource_dict = dict()
        perf_resource_dict["name"] = \
            "sort reduction (2 sort 1 merge) + priority queue group (21)"

        perf_resource_dict["HBM_bank"] = \
            21 * perf_resource_dict_level_A["HBM_bank"] + \
            2 * perf_resource_dict_bitonic_sort_16["HBM_bank"] + \
            1 * perf_resource_dict_parallel_merge_32_to_16["HBM_bank"]
        perf_resource_dict["BRAM_18K"] = \
            21 * perf_resource_dict_level_A["BRAM_18K"] + \
            2 * perf_resource_dict_bitonic_sort_16["BRAM_18K"] + \
            1 * perf_resource_dict_parallel_merge_32_to_16["BRAM_18K"]
        perf_resource_dict["DSP48E"] = \
            21 * perf_resource_dict_level_A["DSP48E"] + \
            2 * perf_resource_dict_bitonic_sort_16["DSP48E"] + \
            1 * perf_resource_dict_parallel_merge_32_to_16["DSP48E"]
        perf_resource_dict["FF"] = \
            21 * perf_resource_dict_level_A["FF"] + \
            2 * perf_resource_dict_bitonic_sort_16["FF"] + \
            1 * perf_resource_dict_parallel_merge_32_to_16["FF"]
        perf_resource_dict["LUT"] = \
            21 * perf_resource_dict_level_A["LUT"] + \
            2 * perf_resource_dict_bitonic_sort_16["LUT"] + \
            1 * perf_resource_dict_parallel_merge_32_to_16["LUT"]
        perf_resource_dict["URAM"] = \
            21 * perf_resource_dict_level_A["URAM"] + \
            2 * perf_resource_dict_bitonic_sort_16["URAM"] + \
            1 * perf_resource_dict_parallel_merge_32_to_16["URAM"]
           
        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 2

        perf_resource_dict["LUT"] += component["FIFO_d2_w32"]["LUT"] * 216
        perf_resource_dict["FF"] += component["FIFO_d2_w32"]["FF"] * 216
        perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w32"]["BRAM_18K"] * 216


        perf_resource_dict["cycles_per_query"] = N_insertion_per_stream
        perf_resource_dict["QPS"] = 1 / (perf_resource_dict["cycles_per_query"] / FREQ)

        option_list.append(perf_resource_dict) 

    elif input_stream_num > 32 and input_stream_num <= 48:

        perf_resource_dict = dict()
        perf_resource_dict["name"] = \
            "sort reduction (3 sort 2 merge) + priority queue group (21)"

        perf_resource_dict["HBM_bank"] = \
            21 * perf_resource_dict_level_A["HBM_bank"] + \
            3 * perf_resource_dict_bitonic_sort_16["HBM_bank"] + \
            2 * perf_resource_dict_parallel_merge_32_to_16["HBM_bank"]
        perf_resource_dict["BRAM_18K"] = \
            21 * perf_resource_dict_level_A["BRAM_18K"] + \
            3 * perf_resource_dict_bitonic_sort_16["BRAM_18K"] + \
            2 * perf_resource_dict_parallel_merge_32_to_16["BRAM_18K"]
        perf_resource_dict["DSP48E"] = \
            21 * perf_resource_dict_level_A["DSP48E"] + \
            3 * perf_resource_dict_bitonic_sort_16["DSP48E"] + \
            2 * perf_resource_dict_parallel_merge_32_to_16["DSP48E"]
        perf_resource_dict["FF"] = \
            21 * perf_resource_dict_level_A["FF"] + \
            3 * perf_resource_dict_bitonic_sort_16["FF"] + \
            2 * perf_resource_dict_parallel_merge_32_to_16["FF"]
        perf_resource_dict["LUT"] = \
            21 * perf_resource_dict_level_A["LUT"] + \
            3 * perf_resource_dict_bitonic_sort_16["LUT"] + \
            2 * perf_resource_dict_parallel_merge_32_to_16["LUT"]
        perf_resource_dict["URAM"] = \
            21 * perf_resource_dict_level_A["URAM"] + \
            3 * perf_resource_dict_bitonic_sort_16["URAM"] + \
            2 * perf_resource_dict_parallel_merge_32_to_16["URAM"]
           
        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 2

        perf_resource_dict["LUT"] += component["FIFO_d2_w32"]["LUT"] * 280
        perf_resource_dict["FF"] += component["FIFO_d2_w32"]["FF"] * 280
        perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w32"]["BRAM_18K"] * 280

 
        perf_resource_dict["cycles_per_query"] = N_insertion_per_stream
        perf_resource_dict["QPS"] = 1 / (perf_resource_dict["cycles_per_query"] / FREQ)

        option_list.append(perf_resource_dict) 

    elif input_stream_num > 48 and input_stream_num <= 64:

        perf_resource_dict = dict()
        perf_resource_dict["name"] = \
            "sort reduction (4 sort 3 merge) + priority queue group (21)"

        perf_resource_dict["HBM_bank"] = \
            21 * perf_resource_dict_level_A["HBM_bank"] + \
            4 * perf_resource_dict_bitonic_sort_16["HBM_bank"] + \
            3 * perf_resource_dict_parallel_merge_32_to_16["HBM_bank"]
        perf_resource_dict["BRAM_18K"] = \
            21 * perf_resource_dict_level_A["BRAM_18K"] + \
            4 * perf_resource_dict_bitonic_sort_16["BRAM_18K"] + \
            3 * perf_resource_dict_parallel_merge_32_to_16["BRAM_18K"]
        perf_resource_dict["DSP48E"] = \
            21 * perf_resource_dict_level_A["DSP48E"] + \
            4 * perf_resource_dict_bitonic_sort_16["DSP48E"] + \
            3 * perf_resource_dict_parallel_merge_32_to_16["DSP48E"]
        perf_resource_dict["FF"] = \
            21 * perf_resource_dict_level_A["FF"] + \
            4 * perf_resource_dict_bitonic_sort_16["FF"] + \
            3 * perf_resource_dict_parallel_merge_32_to_16["FF"]
        perf_resource_dict["LUT"] = \
            21 * perf_resource_dict_level_A["LUT"] + \
            4 * perf_resource_dict_bitonic_sort_16["LUT"] + \
            3 * perf_resource_dict_parallel_merge_32_to_16["LUT"]
        perf_resource_dict["URAM"] = \
            21 * perf_resource_dict_level_A["URAM"] + \
            4 * perf_resource_dict_bitonic_sort_16["URAM"] + \
            3 * perf_resource_dict_parallel_merge_32_to_16["URAM"]
           
        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 2

        perf_resource_dict["LUT"] += component["FIFO_d2_w32"]["LUT"] * 344
        perf_resource_dict["FF"] += component["FIFO_d2_w32"]["FF"] * 344
        perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w32"]["BRAM_18K"] * 344

  
        perf_resource_dict["cycles_per_query"] = N_insertion_per_stream
        perf_resource_dict["QPS"] = 1 / (perf_resource_dict["cycles_per_query"] / FREQ)

        option_list.append(perf_resource_dict) 

    elif input_stream_num > 64 and input_stream_num <= 80:

        perf_resource_dict = dict()
        perf_resource_dict["name"] = \
            "sort reduction (5 sort 4 merge) + priority queue group (21)"

        perf_resource_dict["HBM_bank"] = \
            21 * perf_resource_dict_level_A["HBM_bank"] + \
            5 * perf_resource_dict_bitonic_sort_16["HBM_bank"] + \
            4 * perf_resource_dict_parallel_merge_32_to_16["HBM_bank"]
        perf_resource_dict["BRAM_18K"] = \
            21 * perf_resource_dict_level_A["BRAM_18K"] + \
            5 * perf_resource_dict_bitonic_sort_16["BRAM_18K"] + \
            4 * perf_resource_dict_parallel_merge_32_to_16["BRAM_18K"]
        perf_resource_dict["DSP48E"] = \
            21 * perf_resource_dict_level_A["DSP48E"] + \
            5 * perf_resource_dict_bitonic_sort_16["DSP48E"] + \
            4 * perf_resource_dict_parallel_merge_32_to_16["DSP48E"]
        perf_resource_dict["FF"] = \
            21 * perf_resource_dict_level_A["FF"] + \
            5 * perf_resource_dict_bitonic_sort_16["FF"] + \
            4 * perf_resource_dict_parallel_merge_32_to_16["FF"]
        perf_resource_dict["LUT"] = \
            21 * perf_resource_dict_level_A["LUT"] + \
            5 * perf_resource_dict_bitonic_sort_16["LUT"] + \
            4 * perf_resource_dict_parallel_merge_32_to_16["LUT"]
        perf_resource_dict["URAM"] = \
            21 * perf_resource_dict_level_A["URAM"] + \
            5 * perf_resource_dict_bitonic_sort_16["URAM"] + \
            4 * perf_resource_dict_parallel_merge_32_to_16["URAM"]
           
        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 2

        perf_resource_dict["LUT"] += component["FIFO_d2_w32"]["LUT"] * 408
        perf_resource_dict["FF"] += component["FIFO_d2_w32"]["FF"] * 408
        perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w32"]["BRAM_18K"] * 408

   
        perf_resource_dict["cycles_per_query"] = N_insertion_per_stream
        perf_resource_dict["QPS"] = 1 / (perf_resource_dict["cycles_per_query"] / FREQ)

        option_list.append(perf_resource_dict) 

    elif input_stream_num > 80 and input_stream_num <= 96:

        perf_resource_dict = dict()
        perf_resource_dict["name"] = \
            "sort reduction (6sort 5 merge) + priority queue group (21)"

        perf_resource_dict["HBM_bank"] = \
            21 * perf_resource_dict_level_A["HBM_bank"] + \
            6 * perf_resource_dict_bitonic_sort_16["HBM_bank"] + \
            5 * perf_resource_dict_parallel_merge_32_to_16["HBM_bank"]
        perf_resource_dict["BRAM_18K"] = \
            21 * perf_resource_dict_level_A["BRAM_18K"] + \
            6 * perf_resource_dict_bitonic_sort_16["BRAM_18K"] + \
            5 * perf_resource_dict_parallel_merge_32_to_16["BRAM_18K"]
        perf_resource_dict["DSP48E"] = \
            21 * perf_resource_dict_level_A["DSP48E"] + \
            6 * perf_resource_dict_bitonic_sort_16["DSP48E"] + \
            5 * perf_resource_dict_parallel_merge_32_to_16["DSP48E"]
        perf_resource_dict["FF"] = \
            21 * perf_resource_dict_level_A["FF"] + \
            6 * perf_resource_dict_bitonic_sort_16["FF"] + \
            5 * perf_resource_dict_parallel_merge_32_to_16["FF"]
        perf_resource_dict["LUT"] = \
            21 * perf_resource_dict_level_A["LUT"] + \
            6 * perf_resource_dict_bitonic_sort_16["LUT"] + \
            5 * perf_resource_dict_parallel_merge_32_to_16["LUT"]
        perf_resource_dict["URAM"] = \
            21 * perf_resource_dict_level_A["URAM"] + \
            6 * perf_resource_dict_bitonic_sort_16["URAM"] + \
            5 * perf_resource_dict_parallel_merge_32_to_16["URAM"]
           
        #####   FIFO Consumption (Vivado Measured)   #####
        perf_resource_dict["LUT"] += component["FIFO_d512_w32"]["LUT"] * 2
        perf_resource_dict["FF"] += component["FIFO_d512_w32"]["FF"] * 2
        perf_resource_dict["BRAM_18K"] += component["FIFO_d512_w32"]["BRAM_18K"] * 2

        perf_resource_dict["LUT"] += component["FIFO_d2_w32"]["LUT"] * 472
        perf_resource_dict["FF"] += component["FIFO_d2_w32"]["FF"] * 472
        perf_resource_dict["BRAM_18K"] += component["FIFO_d2_w32"]["BRAM_18K"] * 472

    
        perf_resource_dict["cycles_per_query"] = N_insertion_per_stream
        perf_resource_dict["QPS"] = 1 / (perf_resource_dict["cycles_per_query"] / FREQ)

        option_list.append(perf_resource_dict) 

    return option_list
