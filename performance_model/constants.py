""" Performannce related constants """
FREQ = 140 * 1e6

""" Algorithm related constants """
TOTAL_VECTORS = 1e8

D = 128
topK = 10


""" A number of constant consumption components that should be added to PEs"""

component = dict()

# FIFO depth=512, width=32 bit
# depth 512, width = 512 = 16 * FIFO_d512_w32
component["FIFO_d512_w32"] = dict()
component["FIFO_d512_w32"]["LUT"] = 54
component["FIFO_d512_w32"]["FF"] = 95
component["FIFO_d512_w32"]["BRAM_18K"] = 2 * 0.5
component["FIFO_d512_w32"]["URAM"] = 0
component["FIFO_d512_w32"]["DSP48E"] = 0
component["FIFO_d512_w32"]["HBM_bank"] = 0

# FIFO depth=2, width=8 bit
component["FIFO_d2_w8"] = dict()
component["FIFO_d2_w8"]["LUT"] = 20
component["FIFO_d2_w8"]["FF"] = 6
component["FIFO_d2_w8"]["BRAM_18K"] = 2 * 0
component["FIFO_d2_w8"]["URAM"] = 0
component["FIFO_d2_w8"]["DSP48E"] = 0
component["FIFO_d2_w8"]["HBM_bank"] = 0

# FIFO depth=2, width=32 bit
component["FIFO_d2_w32"] = dict()
component["FIFO_d2_w32"]["LUT"] = 30
component["FIFO_d2_w32"]["FF"] = 6
component["FIFO_d2_w32"]["BRAM_18K"] = 2 * 0
component["FIFO_d2_w32"]["URAM"] = 0
component["FIFO_d2_w32"]["DSP48E"] = 0
component["FIFO_d2_w32"]["HBM_bank"] = 0

# FIFO depth=2, width=512 bit
component["FIFO_d2_w512"] = dict()
component["FIFO_d2_w512"]["LUT"] = 484
component["FIFO_d2_w512"]["FF"] = 964
component["FIFO_d2_w512"]["BRAM_18K"] = 2 * 0
component["FIFO_d2_w512"]["URAM"] = 0
component["FIFO_d2_w512"]["DSP48E"] = 0
component["FIFO_d2_w512"]["HBM_bank"] = 0


#####     Shell     #####
PE_num_dict = dict()

PE_num_dict["network_kernel"] = 1
component["network_kernel"] = dict()
component["network_kernel"]["LUT"] = 126540
component["network_kernel"]["FF"] = 197124
component["network_kernel"]["BRAM_18K"] = 2 * 430
component["network_kernel"]["URAM"] = 9
component["network_kernel"]["DSP48E"] = 0
component["network_kernel"]["HBM_bank"] = 0


PE_num_dict["network_user_kernel_functions"] = 1
component["network_user_kernel_functions"] = dict()
component["network_user_kernel_functions"]["LUT"] = 11242 + 21 * component["FIFO_d2_w32"]["LUT"] + 32 * component["FIFO_d512_w32"]["LUT"]
component["network_user_kernel_functions"]["FF"] = 5124 + 21 * component["FIFO_d2_w32"]["FF"] + 32 * component["FIFO_d512_w32"]["FF"]
component["network_user_kernel_functions"]["BRAM_18K"] = 2 * 0.5 + 21 * component["FIFO_d2_w32"]["BRAM_18K"] + 32 * component["FIFO_d512_w32"]["BRAM_18K"]
component["network_user_kernel_functions"]["URAM"] = 0
component["network_user_kernel_functions"]["DSP48E"] = 0
component["network_user_kernel_functions"]["HBM_bank"] = 0

PE_num_dict["cmac_kernel"] = 1
component["cmac_kernel"] = dict()
component["cmac_kernel"]["LUT"] = 17256
component["cmac_kernel"]["FF"] = 58280
component["cmac_kernel"]["BRAM_18K"] = 2 * 18
component["cmac_kernel"]["URAM"] = 9
component["cmac_kernel"]["DSP48E"] = 0
component["cmac_kernel"]["HBM_bank"] = 0

PE_num_dict["hmss"] = 1
component["hmss"] = dict()
component["hmss"]["LUT"] = 55643 
component["hmss"]["FF"] = 103037
component["hmss"]["BRAM_18K"] = 2 * 4
component["hmss"]["URAM"] = 0
component["hmss"]["DSP48E"] = 0
component["hmss"]["HBM_bank"] = 0

PE_num_dict["System_DPA"] = 1
component["System_DPA"] = dict()
component["System_DPA"]["LUT"] = 35738
component["System_DPA"]["FF"] = 76789
component["System_DPA"]["BRAM_18K"] = 2 * 16
component["System_DPA"]["URAM"] = 0
component["System_DPA"]["DSP48E"] = 0
component["System_DPA"]["HBM_bank"] = 0

PE_num_dict["xdma"] = 1
component["xdma"] = dict()
component["xdma"]["LUT"] = 9100
component["xdma"]["FF"] = 15572
component["xdma"]["BRAM_18K"] = 2 * 0
component["xdma"]["URAM"] = 0
component["xdma"]["DSP48E"] = 0
component["xdma"]["HBM_bank"] = 0

PE_num_dict["static_region"] = 1
component["static_region"] = dict()
component["static_region"]["LUT"] = 93280
component["static_region"]["FF"] = 128746
component["static_region"]["BRAM_18K"] = 2 * 200
component["static_region"]["URAM"] = 0
component["static_region"]["DSP48E"] = 4
component["static_region"]["HBM_bank"] = 0


def add_resources(component_dict, resource_list, PE_num_dict=None):
    """
    given a list of resource name, e.g., ["hmss", "vadd"],
    return the total resource by a dict
    """
    total_resource_consumption = dict()
    total_resource_consumption["LUT"] = 0
    total_resource_consumption["FF"] = 0
    total_resource_consumption["BRAM_18K"] = 0
    total_resource_consumption["URAM"] = 0
    total_resource_consumption["DSP48E"] = 0
    total_resource_consumption["HBM_bank"] = 0 
    
    if PE_num_dict:
        for resource in resource_list:
            PE_num = PE_num_dict[resource]
            total_resource_consumption["LUT"] += component_dict[resource]["LUT"] * PE_num
            total_resource_consumption["FF"] += component_dict[resource]["FF"] * PE_num
            total_resource_consumption["BRAM_18K"] += component_dict[resource]["BRAM_18K"] * PE_num
            total_resource_consumption["URAM"] += component_dict[resource]["URAM"] * PE_num
            total_resource_consumption["DSP48E"] += component_dict[resource]["DSP48E"] * PE_num
            total_resource_consumption["HBM_bank"] += component_dict[resource]["HBM_bank"] * PE_num
    else:
        for resource in resource_list:
            total_resource_consumption["LUT"] += component_dict[resource]["LUT"]
            total_resource_consumption["FF"] += component_dict[resource]["FF"]
            total_resource_consumption["BRAM_18K"] += component_dict[resource]["BRAM_18K"]
            total_resource_consumption["URAM"] += component_dict[resource]["URAM"]
            total_resource_consumption["DSP48E"] += component_dict[resource]["DSP48E"]
            total_resource_consumption["HBM_bank"] += component_dict[resource]["HBM_bank"]

    return total_resource_consumption

component_list_shell = ["network_kernel", "cmac_kernel", "hmss", "System_DPA", "xdma", "static_region"]

shell_consumption = add_resources(component, component_list_shell)

""" Resource related constants """
MAX_UTIL_PERC = 0.8

TOTAL_BRAM_18K = 4032 
TOTAL_DSP48E = 9024
TOTAL_FF = 2607360 
TOTAL_LUT = 1303680
TOTAL_URAM = 960

MAX_HBM_bank = 32 - 2 - 2 - 1 # reserve 30, 31 unused due to their overlap with PCIe; 2 for Network; 1 for value init
MAX_BRAM_18K = TOTAL_BRAM_18K * MAX_UTIL_PERC
MAX_DSP48E = TOTAL_DSP48E * MAX_UTIL_PERC
MAX_FF = TOTAL_FF * MAX_UTIL_PERC
MAX_LUT = TOTAL_LUT * MAX_UTIL_PERC
MAX_URAM = TOTAL_URAM * MAX_UTIL_PERC

# 1 Bank = 256 MB = 4194304 512-bit = 4194304 * 3 = 12582912 vectors
# 100M / 12582912 = 7.94 (without considering padding)
MIN_HBM_bank = 9 # at least 9 banks to hold PQ16 version


"""
An example of expected scanned ratio of a single index

  e.g., suppose the query vectors has the same distribution as the trained vectors, 
    then the larger a Voronoi cell, the more likely they will be searched
  e.g., searching 32 cells over 8192 in 100 M dataset will not scan 32 / 8192 * 1e8 entries on average,
    we need to scan more
"""

scan_ratio_with_OPQ = {
    1024: 1.102495894347366,
    2048: 1.12463916710666,
    4096: 1.12302396550103,
    8192: 1.135891773928242,
    16384: 1.1527141392580655,
    32768: 1.1441353378627621,
    65536: 1.1411144965226643,
    131072: 1.1476783059960072,
    262144: 1.1543383003102523
}


scan_ratio_without_OPQ = {
    1024: 1.1023307648983034,
    2048: 1.1245342465011723,
    4096: 1.1230564521721877,
    8192: 1.135866022841546, 
    16384: 1.1523836603564073, 
    32768: 1.1440334275739672,
    65536: 1.1410689577844846,
    131072: 1.1476378583040157,
    262144: 1.1543274466049378
}
