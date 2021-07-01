import numpy as np

"""
Reach 95% Recall:

2x14 nprobe=4096
2x12 nprobe=1024
2x10 nprobe=256
2x9 nprobe=256
2x8 nprobe=128
"""

# For each item (PQ16):
# Random Access = 16 -> each 
# Computation = 128 * 2 = 256
halve_cluster_list = np.array([2 ** i for i in [14, 12, 10, 9, 8]])
cell_number_list = np.array([halve_cluster_list[i] ** 2 for i in range(len(halve_cluster_list))])
nprobe_list = np.array([4096, 1024, 256, 256, 128])
elements_per_cell_list = 1e9 * nprobe_list / cell_number_list

random_access_needed = 16 * elements_per_cell_list
random_access_latency = 200 * 1e-6
memory_channel_number = 32
throughput_random_access = 1 / (random_access_needed * random_access_latency / memory_channel_number)
print(random_access_needed)
print("QPS by Random access: {}".format(throughput_random_access))

##### Coarese-grained distance not counted yet
computation_needed = 256 * elements_per_cell_list
computation_speed = 200 * 1e9
throughput_computation = computation_speed / computation_needed
print("QPS by computation: {}".format(throughput_computation))


"""
Too many random accesses.

For example, 2^14 per halve -> 2^28 centroids
The average number of vectors per centroid = 1e9 / (2 ^ 28)
Random Access Required = 1e9 / (2 ^ 28) * 4096 * 16 = 244140

But if this is true, how does CPU be better? Double check the paper

[  244140.625   976562.5    3906250.    15625000.    31250000.   ]
QPS by Random access: [0.65536 0.16384 0.04096 0.01024 0.00512]
QPS by computation: [51200. 12800.  3200.   800.   400.]

"""


