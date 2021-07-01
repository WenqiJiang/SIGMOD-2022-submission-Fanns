import numpy as np
import argparse 

"""
TO BE IMPROVED: 
1. not considered sort network yet, e.g., topK=100
2. ####  IN THE COMPUTATION STAGE, analyze memory vs computation which is bottleneck, e.g., distance computation performance decided by HBM or computation
"""

"""
Supposed storage of IVF
(Storage on chip) Coarse codebook 
(Storage in HBM) PQ codebook
(Storage on chip) Computed distance table
(Storage in DDR) PQ code
"""

"""
IVF PQ16 reach 95% recall:
--nlist=1024 --nprobe=16
--nlist=2048 --nprobe=32
--nlist=4096 --nprobe=32
--nlist=8192 --nprobe=32
--nlist=16384 --nprobe=64
--nlist=32768 --nprobe=64
--nlist=65536 --nprobe=64
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


# two ways
# 1st -> decode one by one
args = parser.parse_args()


class IVF:

    def __init__(self, nlist, nprobe, D, m, k, FLOPS, onchip_storage_limit, HBM_bandwidth, DDR_bandwidth):

        """ Parameters """
        self.nlist = nlist
        self.nprobe = nprobe
        self.D = D
        self.m = m
        self.k = k

        self.FLOPS = FLOPS
        self.onchip_storage_limit = onchip_storage_limit
        self.HBM_bandwidth = HBM_bandwidth
        self.DDR_bandwidth = DDR_bandwidth

        """ Fixed paramters """
        self.vector_num = 1e9
        self.HBM_storage_limit = 8 * 1e9
        self.DDR_storage_limit = 32 * 1e9
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

    def estimate_performance(self):
        self.distance_table_construction()
        self.PQ_scan()
        self.check_memory_fit()

        print("Total consumption:")
        print("On-chip: {:.2f} MB\tHBM: {:.2f} MB\tDDR: {:.2f} GB".format(
            self.on_chip_consumption / (1024 ** 2), 
            self.HBM_consumption / (1024 ** 2),
            self.DDR_consumption / (1024 ** 3)))
        print("Scan in HBM / query: {:.2f} MB\nScan in DDR / query: {:.2f} MB".format(
            self.HBM_scan / (1024 ** 2), self.DDR_scan / (1024 ** 2)))
        print("Flops / query: {} M".format(self.flops_needed / 1e6))


        # NOTE! This is average latency, not the e2e latency
        latency_HBM_scan = self.HBM_scan / self.HBM_bandwidth + \
            self.HBM_access_count / self.HBM_channels * self.DRAM_access_latency
        latency_DDR_scan = self.DDR_scan / self.DDR_bandwidth + \
            self.DDR_access_count / self.DDR_channels * self.DRAM_access_latency 
        latency_computation = self.flops_needed / self.FLOPS
        latency = np.max([latency_HBM_scan, latency_DDR_scan, latency_computation])
        throughput = 1 / latency
        # print("\nWithout considering pipeline, Latency = sum of latency (scan + computation) in all stages.")
        print("\nConsidering overlapping: Estimated Latency = Max(latency_HBM_scan, latency_DDR_scan, latency_computation).")
        print("Latency:{:.2f} us\nThroughput:{:.2f} QPS".format(latency * 1e6, throughput))
        print("Distance table construction -> suppose the best case scenario that each channel can be evenly accessed for PQ codebook loading")
        print("Latency break down in us:\nlatency_HBM_scan:{:.2f}\nlatency_DDR_scan:{:.2f}\nlatency_computation:{:.2f}".format(
            latency_HBM_scan * 1e6, latency_DDR_scan * 1e6, latency_computation * 1e6))


    #####   2 Main Stages   #####
    def distance_table_construction(self):
        # Storage 
        self.on_chip_consumption += self.nlist * self.D * self.float_size # Coarse codebook 
        self.HBM_consumption += self.nlist * self.D * self.k * self.float_size# PQ codebook
        self.on_chip_consumption += self.nprobe * self.m * self.k * self.float_size # distance table

        # Scan + Random Access
        self.HBM_scan += self.nprobe * self.D * self.k * self.float_size # PQ codebook in HBM
        self.HBM_access_count += self.HBM_channels

        # Computation
        self.flops_needed += self.nprobe * self.D * self.k * 2 

    def PQ_scan(self):
        # Storage
        self.DDR_consumption += self.m * 1e9 # PQ code

        # Scan + Random Access
        self.DDR_scan += self.nprobe * 1e9 / self.nlist * self.float_size # PQ code
        self.DDR_access_count += self.DDR_channels * self.nprobe

        # Computation
        self.flops_needed += self.nprobe * 1e9 / self.nlist * self.m # Lookup

    #####   Check Memory Fit Status   #####
    def check_memory_fit(self):
        assert self.on_chip_consumption < self.onchip_storage_limit
        assert self.HBM_consumption < self.HBM_storage_limit
        assert self.DDR_consumption < self.DDR_storage_limit

class IMI:

    def __init__(self):
        raise("Not Implemented Error")

if __name__ == "__main__":
    ivf_test = IVF(args.nlist, args.nprobe, args.D, args.m, args.k, args.FLOPS, 
        args.onchip_storage_limit, args.HBM_bandwidth, args.DDR_bandwidth)
    ivf_test.estimate_performance()