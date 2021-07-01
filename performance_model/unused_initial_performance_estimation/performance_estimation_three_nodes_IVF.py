import numpy as np
import argparse 

"""
TO BE IMPROVED: 
1. not considered sort network yet, e.g., topK=100
2. ####  IN THE COMPUTATION STAGE, analyze memory vs computation which is bottleneck, e.g., distance computation performance decided by HBM or computation
"""

"""
Supposed storage of IVF

Node 1:
(Storage on chip) Coarse codebook
(Storage in HBM) PQ codebook
(Storage on chip) Computed distance table

Node 2&3:
(Storage on chip) Computed distance table
(Storage in HBM) PQ code
"""

"""
IVF PQ16 reach 95% recall:
nlist=1024 nprobe=16
nlist=2048 nprobe=32
nlist=4096 nprobe=32
nlist=8192 nprobe=32
nlist=16384 nprobe=64
nlist=32768 nprobe=64
nlist=65536 nprobe=64
"""

parser = argparse.ArgumentParser()

# coarse-grained clustering info
parser.add_argument('--nlist', type=int, default=4096, help="the number of Voronoi cells in total")
parser.add_argument('--nprobe', type=int, default=32, help="the number of Voronoi cells to search")

# PQ and fine-grained clustering info
parser.add_argument('--D', type=int, default=128, help="full length of a vector")
parser.add_argument('--m', type=int, default=16, help="8 or 16, the segment number of PQ")
parser.add_argument('--k', type=int, default=256, help="number of PQ centroids, ususally 8 bit -> 2^8 = 256")

# FPGA parameters
parser.add_argument('--FLOPS', type=float, default=200 * 1e9, help="floating point operations per second")
parser.add_argument('--onchip_storage_limit', type=float, default=30 * 1e6, help="on-chip memory in bytes")
parser.add_argument('--HBM_bandwidth', type=float, default=200 * 1e9, help="HBM bandwidth, note that AXI interface is not necessarily full 512-bit")
parser.add_argument('--DDR_bandwidth', type=float, default=32 * 1e9, help="DDR bandwidth, note that AXI interface is not necessarily full 512-bit")
parser.add_argument('--network_bandwidth', type=float, default=10 * 1e9, help="Network bandwidth, default 10 GB/s (bytes instead of bits)")


# two ways
# 1st -> decode one by one
args = parser.parse_args()


class Node:

    def __init__(self, FLOPS, onchip_storage_limit, HBM_bandwidth, DDR_bandwidth):


        self.FLOPS = FLOPS
        self.onchip_storage_limit = onchip_storage_limit
        self.HBM_bandwidth = HBM_bandwidth
        self.DDR_bandwidth = DDR_bandwidth

        """ Storage Limit """
        self.HBM_storage_limit = 8 * (1024 ** 3)
        self.DDR_storage_limit = 32 * (1024 ** 3)

        """ Fixed paramters """
        self.vector_num = 1e9
        self.float_size = 4
        self.DRAM_access_latency = 300 * 1e-9
        self.DDR_channels = 2
        self.HBM_channels = 32

        """ Init overflow check parameters """
        self.on_chip_consumption = 0
        self.HBM_consumption = 0
        self.DDR_consumption = 0

        """ Performance count """
        self.HBM_scan = 0
        self.DDR_scan = 0
        self.flops_needed = 0
        self.DDR_access_count = 0
        self.HBM_access_count = 0

    def check_memory_fit(self):
        assert self.on_chip_consumption < self.onchip_storage_limit
        assert self.HBM_consumption < self.HBM_storage_limit
        assert self.DDR_consumption < self.DDR_storage_limit

    def compute_latency_throughput(self):
        # in seconds
        self.HBM_latency = self.HBM_scan / self.HBM_bandwidth + \
            self.HBM_access_count / self.HBM_channels * self.DRAM_access_latency
        self.DDR_latency = self.DDR_scan / self.DDR_bandwidth + \
            self.DDR_access_count / self.DDR_channels * self.DRAM_access_latency
        self.computation_latency = self.flops_needed / self.FLOPS

        self.max_stage_latency = np.max([self.HBM_latency, self.DDR_latency, self.computation_latency])
        self.total_latency = self.HBM_latency + self.DDR_latency + self.computation_latency

        self.throughput = 1 / self.max_stage_latency


class IVF:

    def __init__(self, nlist, nprobe, D, m, k, FLOPS, onchip_storage_limit, HBM_bandwidth, DDR_bandwidth, network_bandwidth):

        """ Parameters """
        self.nlist = nlist
        self.nprobe = nprobe
        self.D = D
        self.m = m
        self.k = k

        self.vector_num = 1e9
        self.float_size = 4
        self.DRAM_access_latency = 200 * 1e-9
        self.DDR_channels = 2
        self.HBM_channels = 32
        self.DRAM_access_latency = 200 * 1e-9
        self.network_latency_single_hop = 100 * 1e-6 # single hop

        self.FLOPS = FLOPS
        self.onchip_storage_limit = onchip_storage_limit
        self.HBM_bandwidth = HBM_bandwidth
        self.DDR_bandwidth = DDR_bandwidth
        self.network_bandwidth = network_bandwidth

        """ Init overflow check parameters """
        self.node1 = Node(FLOPS, onchip_storage_limit, HBM_bandwidth, DDR_bandwidth) # distance table construction 
        self.node2 = Node(FLOPS, onchip_storage_limit, HBM_bandwidth, DDR_bandwidth) # PQ scan
        self.node3 = Node(FLOPS, onchip_storage_limit, HBM_bandwidth, DDR_bandwidth) # PQ scan

    def estimate_performance(self):
        self.node1_distance_table_construction(self.node1)
        self.node23_PQ_scan(self.node2)
        self.node23_PQ_scan(self.node3)

        self.node1.compute_latency_throughput()
        self.node2.compute_latency_throughput()
        self.node3.compute_latency_throughput()
        self.node1.check_memory_fit()
        self.node2.check_memory_fit()
        self.node3.check_memory_fit()

        # node 2 and 3 should have the same performance
        assert self.node2.max_stage_latency == self.node3.max_stage_latency and \
            self.node2.total_latency == self.node3.total_latency and \
            self.node2.throughput == self.node3.throughput

        network_latency, network_throughput = self.network_latency_throughput()

        total_latency = self.node1.total_latency + self.node2.total_latency + network_latency
        total_throughput = np.min([self.node1.throughput, self.node2.throughput, network_throughput])

        print("Overall:\nTotal Latency: {:.2f} us\tThroughput: {:.2f} QPS\n".format(total_latency * 1e6, total_throughput))

        print("Node1 -> suppose the PQ codebook is evenly split over 32 banks")
        # print("Node1 -> suppose the best case scenario that each channel can be evenly accessed for PQ codebook loading")
        self.print_node_bottleneck(self.node1, "node1")
        self.print_node_bottleneck(self.node2, "node2")
        print("Network Throughput: {:.2f}\n".format(network_throughput))


    def print_node_bottleneck(self, node, nodename):
        print(nodename)
        print("HBM Latency: {:.2f} us\tDDR Latency: {:.2f} us\tComputation Latency: {:.2f} us".format(
            node.HBM_latency * 1e6, node.DDR_latency * 1e6, node.computation_latency * 1e6))
        print("throughput: {:.2f}".format(node.throughput))
        max_latency = np.max([node.HBM_latency, node.DDR_latency, node.computation_latency])
        if max_latency == node.HBM_latency:
            print("Bound by HBM")
        elif max_latency == node.DDR_latency:
            print("Bound by DDR")
        elif max_latency == node.computation_latency:
            print("Bound by Computation")


    def network_latency_throughput(self):
        # throughput bound by node1, which needs to distribute distance tables
        # to node 2 and 3
        distance_table_size_per_query = self.nprobe * self.m * self.k * self.float_size
        network_throughput = self.network_bandwidth / (distance_table_size_per_query * 2)

        # query -> node 1; node 1 -> 2 3 in parallel; 2 3 to server in parallel
        network_latency = 3 * self.network_latency_single_hop 

        return network_latency, network_throughput

    #####   2 Main Stages   #####
    def node1_distance_table_construction(self, node):

        # Storage
        node.on_chip_consumption += self.nlist * self.D * self.float_size # Coarse codebook 
        node.HBM_consumption += self.nlist * self.D * self.k * self.float_size# PQ codebook
        node.on_chip_consumption += self.nprobe * self.m * self.k * self.float_size # distance table

        # Scan + Random Access
        node.HBM_scan += self.nprobe * self.D * self.k * self.float_size # PQ codebook in HBM
        node.HBM_access_count += self.nprobe * self.HBM_channels

        # Computation
        node.flops_needed += self.nprobe * self.D * self.k * 2 

    def node23_PQ_scan(self, node):
        # Storage
        node.on_chip_consumption += self.nprobe * self.m * self.k * self.float_size # distance table
        node.HBM_consumption += self.m * self.vector_num / 2

        # Scan + Random Access
        node.HBM_scan += self.nprobe * self.vector_num / self.nlist * self.float_size / 2
        node.HBM_access_count += self.HBM_channels * self.nprobe

        # Computation
        node.flops_needed += self.nprobe * self.vector_num / self.nlist * self.m / 2


class IMI:

    def __init__(self):
        raise("Not Implemented Error")

if __name__ == "__main__":
    ivf_test = IVF(args.nlist, args.nprobe, args.D, args.m, args.k, args.FLOPS, 
        args.onchip_storage_limit, args.HBM_bandwidth, args.DDR_bandwidth, args.network_bandwidth)
    ivf_test.estimate_performance()