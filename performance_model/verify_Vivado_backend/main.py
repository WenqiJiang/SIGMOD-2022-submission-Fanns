
"""
  Here we define a two-level dictionary:
    The first level stores the components, e.g., component["hmss"], componenet["vadd"], etc.
    Each component in the first level is another dictionary, e.g.,
        component["hmss"] = dict()
        component["hmss"]["LUT"] = 55643 
        component["hmss"]["FF"] = 103037
        component["hmss"]["BRAM_18K"] = 2 * 4
        component["hmss"]["URAM"] = 0
        component["hmss"]["DSP48E"] = 0
        component["hmss"]["HBM_bank"] = 0
"""

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

component = dict()
PE_num_dict = dict()

# FIFO depth=512, width=32 bit
PE_num_dict["FIFO_d512_w32"] = 0
component["FIFO_d512_w32"] = dict()
component["FIFO_d512_w32"]["LUT"] = 54
component["FIFO_d512_w32"]["FF"] = 95
component["FIFO_d512_w32"]["BRAM_18K"] = 2 * 0.5
component["FIFO_d512_w32"]["URAM"] = 0
component["FIFO_d512_w32"]["DSP48E"] = 0
component["FIFO_d512_w32"]["HBM_bank"] = 0

# FIFO depth=2, width=8 bit
PE_num_dict["FIFO_d2_w8"] = 0
component["FIFO_d2_w8"] = dict()
component["FIFO_d2_w8"]["LUT"] = 20
component["FIFO_d2_w8"]["FF"] = 6
component["FIFO_d2_w8"]["BRAM_18K"] = 2 * 0
component["FIFO_d2_w8"]["URAM"] = 0
component["FIFO_d2_w8"]["DSP48E"] = 0
component["FIFO_d2_w8"]["HBM_bank"] = 0


# FIFO depth=2, width=32 bit
PE_num_dict["FIFO_d2_w32"] = 0
component["FIFO_d2_w32"] = dict()
component["FIFO_d2_w32"]["LUT"] = 30
component["FIFO_d2_w32"]["FF"] = 6
component["FIFO_d2_w32"]["BRAM_18K"] = 2 * 0
component["FIFO_d2_w32"]["URAM"] = 0
component["FIFO_d2_w32"]["DSP48E"] = 0
component["FIFO_d2_w32"]["HBM_bank"] = 0

# FIFO depth=2, width=512 bit
PE_num_dict["FIFO_d2_w512"] = 0
component["FIFO_d2_w512"] = dict()
component["FIFO_d2_w512"]["LUT"] = 484
component["FIFO_d2_w512"]["FF"] = 964
component["FIFO_d2_w512"]["BRAM_18K"] = 2 * 0
component["FIFO_d2_w512"]["URAM"] = 0
component["FIFO_d2_w512"]["DSP48E"] = 0
component["FIFO_d2_w512"]["HBM_bank"] = 0

#####    Top-Level Kernel   #####

PE_num_dict["vadd"] = 1
component["vadd"] = dict()
component["vadd"]["LUT"] = 690312 
component["vadd"]["FF"] = 824557
component["vadd"]["BRAM_18K"] = 2 * 769
component["vadd"]["URAM"] = 694
component["vadd"]["DSP48E"] = 3518
component["vadd"]["HBM_bank"] = 27 # TODO: edit this

PE_num_dict["axi"] = 21 # for stoing PQ Codes
component["axi"] = dict()
component["axi"]["LUT"] = 1159
component["axi"]["FF"] = 3117
component["axi"]["BRAM_18K"] = 2 * 7.5
component["axi"]["URAM"] = 0
component["axi"]["DSP48E"] = 0
component["axi"]["HBM_bank"] = 1

PE_num_dict["axi_init"] = 7 # 5 * 32 bit input, 1 * 64 bit output
component["axi_init"] = dict()
component["axi_init"]["LUT"] = 406
component["axi_init"]["FF"] = 801
component["axi_init"]["BRAM_18K"] = 2 * 0.5
component["axi_init"]["URAM"] = 0
component["axi_init"]["DSP48E"] = 0
component["axi_init"]["HBM_bank"] = 1

PE_num_dict["type_conversion_and_split"] = 21 # for stoing PQ Codes
component["type_conversion_and_split"] = dict()
component["type_conversion_and_split"]["LUT"] = 290
component["type_conversion_and_split"]["FF"] = 1070
component["type_conversion_and_split"]["BRAM_18K"] = 2 * 0
component["type_conversion_and_split"]["URAM"] = 0
component["type_conversion_and_split"]["DSP48E"] = 0
component["type_conversion_and_split"]["HBM_bank"] = 0

### Stage 1
PE_num_dict["OPQ"] = 1
component["OPQ"] = dict()
component["OPQ"]["LUT"] = 1479
component["OPQ"]["FF"] = 1646
component["OPQ"]["BRAM_18K"] = 2 * 18.5
component["OPQ"]["URAM"] = 0
component["OPQ"]["DSP48E"] = 20
component["OPQ"]["HBM_bank"] = 0

### Stage 2
PE_num_dict["stage2_PE"] = 24
component["stage2_PE"] = dict()
component["stage2_PE"]["LUT"] = 5376
component["stage2_PE"]["FF"] = 6316
component["stage2_PE"]["BRAM_18K"] = 2 * 0
component["stage2_PE"]["URAM"] = 16
component["stage2_PE"]["DSP48E"] = 40
component["stage2_PE"]["HBM_bank"] = 0

PE_num_dict["FIFO_d512_w32"] += PE_num_dict["stage2_PE"] * 2

### Stage 3
PE_num_dict["priority_queue"] = 0
PE_num_dict["priority_queue"] += 3 * 1.7 # use linear model, use depth=10 as base
component["priority_queue"] = dict()
component["priority_queue"]["LUT"] = 1830
component["priority_queue"]["FF"] = 2172
component["priority_queue"]["BRAM_18K"] = 2 * 0
component["priority_queue"]["URAM"] = 0
component["priority_queue"]["DSP48E"] = 0
component["priority_queue"]["HBM_bank"] = 0

PE_num_dict["FIFO_d512_w32"] += 3 * 2

### Stage 4

PE_num_dict["stage4_PE"] = 3
component["stage4_PE"] = dict()
component["stage4_PE"]["LUT"] = 24746
component["stage4_PE"]["FF"] = 35870
component["stage4_PE"]["BRAM_18K"] = 2 * 0.5
component["stage4_PE"]["URAM"] = 16
component["stage4_PE"]["DSP48E"] = 216
component["stage4_PE"]["HBM_bank"] = 0

PE_num_dict["FIFO_d512_w32"] += 17 * (PE_num_dict["stage4_PE"])

### Stage 5
PE_num_dict["stage5_PE"] = 63
component["stage5_PE"] = dict()
component["stage5_PE"]["LUT"] = 3937
component["stage5_PE"]["FF"] = 3954
component["stage5_PE"]["BRAM_18K"] = 2 * 8
component["stage5_PE"]["URAM"] = 0
component["stage5_PE"]["DSP48E"] = 30
component["stage5_PE"]["HBM_bank"] = 0

PE_num_dict["FIFO_d2_w8"] += 16 * 3 * (PE_num_dict["stage5_PE"] / 3)
PE_num_dict["FIFO_d2_w32"] += 19 * 3 * (PE_num_dict["stage5_PE"] / 3)
PE_num_dict["FIFO_d2_w512"] += (PE_num_dict["stage5_PE"] / 3)

### Stage 6

PE_num_dict["bitonic_sort"] = 4
component["bitonic_sort"] = dict()
component["bitonic_sort"]["LUT"] = 10223
component["bitonic_sort"]["FF"] = 15561
component["bitonic_sort"]["BRAM_18K"] = 2 * 0
component["bitonic_sort"]["URAM"] = 0
component["bitonic_sort"]["DSP48E"] = 0
component["bitonic_sort"]["HBM_bank"] = 0

PE_num_dict["bitonic_merge"] = 3
component["bitonic_merge"] = dict()
component["bitonic_merge"]["LUT"] = 5588
component["bitonic_merge"]["FF"] = 9374
component["bitonic_merge"]["BRAM_18K"] = 2 * 0
component["bitonic_merge"]["URAM"] = 0
component["bitonic_merge"]["DSP48E"] = 0
component["bitonic_merge"]["HBM_bank"] = 0

PE_num_dict["priority_queue"] += 21

PE_num_dict["FIFO_d2_w32"] += 344 # for 64 -> 10
PE_num_dict["FIFO_d512_w32"] += 2

#####     Shell     #####
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

component_list_top_kernel = ["axi", "axi_init", "type_conversion_and_split", \
    "OPQ", "stage2_PE", "priority_queue", "stage4_PE", "stage5_PE", "bitonic_sort", \
    "bitonic_merge", "FIFO_d512_w32", "FIFO_d2_w32", "FIFO_d2_w8"]

component_list_shell = ["hmss", "System_DPA", "xdma", "static_region"]

component_list_all = component_list_top_kernel + component_list_shell

print(component_list_all)
estimated_consumption = add_resources(component, component_list_all, PE_num_dict)
# estimated_consumption = add_resources(component, ["vadd", "hmss", "System_DPA", "xdma", "static_region"])
# print(add_resources(component, component_list_all, PE_num_dict))

# the real consumption of the entire board
real_resource_consumption = dict() 
real_resource_consumption["LUT"] = 885836
real_resource_consumption["FF"] = 1153295
real_resource_consumption["BRAM_18K"] = 989*2
real_resource_consumption["URAM"] = 694
real_resource_consumption["DSP48E"] = 3522

def get_diff_rate(resource_dict_real, resource_dict_estimated):

    delta = dict()
    delta["LUT"] = (resource_dict_estimated["LUT"] - resource_dict_real["LUT"]) / resource_dict_real["LUT"]
    delta["FF"] = (resource_dict_estimated["FF"] - resource_dict_real["FF"]) / resource_dict_real["FF"]
    delta["BRAM_18K"] = (resource_dict_estimated["BRAM_18K"] - resource_dict_real["BRAM_18K"]) / resource_dict_real["BRAM_18K"]
    delta["URAM"] = (resource_dict_estimated["URAM"] - resource_dict_real["URAM"]) / resource_dict_real["URAM"]
    delta["DSP48E"] = (resource_dict_estimated["DSP48E"] - resource_dict_real["DSP48E"]) / resource_dict_real["DSP48E"]

    return delta

delta = get_diff_rate(real_resource_consumption, estimated_consumption)

print("Estimated Consumption: {}\nReal Consumption: {}\nDelta: {}".format(
    estimated_consumption, 
    real_resource_consumption, 
    delta))
