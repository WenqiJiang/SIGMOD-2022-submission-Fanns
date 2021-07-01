#include "host.hpp"

#define BANK_NAME(n) n | XCL_MEM_TOPOLOGY
/* for U280 specifically */
const int bank[40] = {
    /* 0 ~ 31 HBM */
    BANK_NAME(0),  BANK_NAME(1),  BANK_NAME(2),  BANK_NAME(3),  BANK_NAME(4),
    BANK_NAME(5),  BANK_NAME(6),  BANK_NAME(7),  BANK_NAME(8),  BANK_NAME(9),
    BANK_NAME(10), BANK_NAME(11), BANK_NAME(12), BANK_NAME(13), BANK_NAME(14),
    BANK_NAME(15), BANK_NAME(16), BANK_NAME(17), BANK_NAME(18), BANK_NAME(19),
    BANK_NAME(20), BANK_NAME(21), BANK_NAME(22), BANK_NAME(23), BANK_NAME(24),
    BANK_NAME(25), BANK_NAME(26), BANK_NAME(27), BANK_NAME(28), BANK_NAME(29),
    BANK_NAME(30), BANK_NAME(31), 
    /* 32, 33 DDR */ 
    BANK_NAME(32), BANK_NAME(33), 
    /* 34 ~ 39 PLRAM */ 
    BANK_NAME(34), BANK_NAME(35), BANK_NAME(36), BANK_NAME(37), 
    BANK_NAME(38), BANK_NAME(39)};

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
		return EXIT_FAILURE;
	}

    std::string binaryFile = argv[1];
//////////////////////////////   TEMPLATE START  //////////////////////////////
    
    int query_num = 10000;
    // len = 1591280
    size_t HBM_embedding0_len = 1591280;
    size_t HBM_embedding1_len = 1591280;
    size_t HBM_embedding2_len = 1591280;
    size_t HBM_embedding3_len = 1591280;
    size_t HBM_embedding4_len = 1591280;
    size_t HBM_embedding5_len = 1591280;
    size_t HBM_embedding6_len = 1591280;
    size_t HBM_embedding7_len = 1591280;
    size_t HBM_embedding8_len = 1591280;
    size_t HBM_embedding9_len = 1591280;
    size_t HBM_embedding10_len = 1591280;
    size_t HBM_embedding11_len = 1591280;
    size_t HBM_embedding12_len = 1591280;
    size_t HBM_embedding13_len = 1591280;
    size_t HBM_embedding14_len = 1591280;
    size_t HBM_embedding15_len = 1591280;
    size_t HBM_embedding16_len = 1591280;
    size_t HBM_embedding17_len = 1591280;
    size_t HBM_embedding18_len = 1591280;
    size_t HBM_embedding19_len = 1591280;
    size_t HBM_embedding20_len = 1591280;

    size_t HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_len = NLIST * 3;
    size_t HBM_query_vector_len = query_num * 128 < 10000 * 128? query_num * 128: 10000 * 128;
    size_t HBM_vector_quantizer_len = 8192 * 128;
    size_t HBM_product_quantizer_len = 16 * 256 * 8;
    size_t HBM_out_len = PRIORITY_QUEUE_LEN * query_num; 

    size_t sw_result_vec_ID_len = 10000 * 10;
    size_t sw_result_dist_len = 10000 * 10;

    // size = 101841920
    size_t HBM_embedding0_size =  HBM_embedding0_len * sizeof(ap_uint512_t);
    size_t HBM_embedding1_size =  HBM_embedding1_len * sizeof(ap_uint512_t);
    size_t HBM_embedding2_size =  HBM_embedding2_len * sizeof(ap_uint512_t);
    size_t HBM_embedding3_size =  HBM_embedding3_len * sizeof(ap_uint512_t);
    size_t HBM_embedding4_size =  HBM_embedding4_len * sizeof(ap_uint512_t);
    size_t HBM_embedding5_size =  HBM_embedding5_len * sizeof(ap_uint512_t);
    size_t HBM_embedding6_size =  HBM_embedding6_len * sizeof(ap_uint512_t);
    size_t HBM_embedding7_size =  HBM_embedding7_len * sizeof(ap_uint512_t);
    size_t HBM_embedding8_size =  HBM_embedding8_len * sizeof(ap_uint512_t);
    size_t HBM_embedding9_size =  HBM_embedding9_len * sizeof(ap_uint512_t);
    size_t HBM_embedding10_size =  HBM_embedding10_len * sizeof(ap_uint512_t);
    size_t HBM_embedding11_size =  HBM_embedding11_len * sizeof(ap_uint512_t);
    size_t HBM_embedding12_size =  HBM_embedding12_len * sizeof(ap_uint512_t);
    size_t HBM_embedding13_size =  HBM_embedding13_len * sizeof(ap_uint512_t);
    size_t HBM_embedding14_size =  HBM_embedding14_len * sizeof(ap_uint512_t);
    size_t HBM_embedding15_size =  HBM_embedding15_len * sizeof(ap_uint512_t);
    size_t HBM_embedding16_size =  HBM_embedding16_len * sizeof(ap_uint512_t);
    size_t HBM_embedding17_size =  HBM_embedding17_len * sizeof(ap_uint512_t);
    size_t HBM_embedding18_size =  HBM_embedding18_len * sizeof(ap_uint512_t);
    size_t HBM_embedding19_size =  HBM_embedding19_len * sizeof(ap_uint512_t);
    size_t HBM_embedding20_size =  HBM_embedding20_len * sizeof(ap_uint512_t);

    size_t HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_size = 
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_len * sizeof(int);
    size_t HBM_query_vector_size = HBM_query_vector_len * sizeof(float);
    size_t HBM_vector_quantizer_size = HBM_vector_quantizer_len * sizeof(float);
    size_t HBM_product_quantizer_size = HBM_product_quantizer_len * sizeof(float);
    size_t HBM_out_size = HBM_out_len * sizeof(ap_uint64_t); 

    size_t sw_result_vec_ID_size = sw_result_vec_ID_len * sizeof(int);
    size_t sw_result_dist_size = sw_result_dist_len * sizeof(float);

//////////////////////////////   TEMPLATE END  //////////////////////////////

    cl_int err;
    unsigned fileBufSize;

    // allocate aligned 2D vectors
//////////////////////////////   TEMPLATE START  //////////////////////////////
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding0(HBM_embedding0_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding1(HBM_embedding1_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding2(HBM_embedding2_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding3(HBM_embedding3_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding4(HBM_embedding4_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding5(HBM_embedding5_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding6(HBM_embedding6_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding7(HBM_embedding7_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding8(HBM_embedding8_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding9(HBM_embedding9_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding10(HBM_embedding10_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding11(HBM_embedding11_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding12(HBM_embedding12_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding13(HBM_embedding13_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding14(HBM_embedding14_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding15(HBM_embedding15_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding16(HBM_embedding16_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding17(HBM_embedding17_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding18(HBM_embedding18_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding19(HBM_embedding19_len, 0);
    std::vector<ap_uint512_t, aligned_allocator<ap_uint512_t>> HBM_embedding20(HBM_embedding20_len, 0);

    std::vector<int, aligned_allocator<int>> HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid(
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_len, 0);
    std::vector<float, aligned_allocator<float>> HBM_query_vectors(HBM_query_vector_len, 0);
    std::vector<float, aligned_allocator<float>> HBM_vector_quantizer(HBM_vector_quantizer_len, 0);
    std::vector<float, aligned_allocator<float>> HBM_product_quantizer(HBM_product_quantizer_len, 0);
    std::vector<ap_uint64_t, aligned_allocator<ap_uint64_t>> HBM_out(HBM_out_len, 0);
    
    std::vector<int, aligned_allocator<int>> sw_result_vec_ID(sw_result_vec_ID_len, 0);
    std::vector<float, aligned_allocator<float>> sw_result_dist(sw_result_dist_len, 0);

//////////////////////////////   TEMPLATE END  //////////////////////////////

    char* HBM_embedding0_char = (char*) malloc(HBM_embedding0_size);
    char* HBM_embedding1_char = (char*) malloc(HBM_embedding1_size);
    char* HBM_embedding2_char = (char*) malloc(HBM_embedding2_size);
    char* HBM_embedding3_char = (char*) malloc(HBM_embedding3_size);
    char* HBM_embedding4_char = (char*) malloc(HBM_embedding4_size);
    char* HBM_embedding5_char = (char*) malloc(HBM_embedding5_size);
    char* HBM_embedding6_char = (char*) malloc(HBM_embedding6_size);
    char* HBM_embedding7_char = (char*) malloc(HBM_embedding7_size);
    char* HBM_embedding8_char = (char*) malloc(HBM_embedding8_size);
    char* HBM_embedding9_char = (char*) malloc(HBM_embedding9_size);
    char* HBM_embedding10_char = (char*) malloc(HBM_embedding10_size);
    char* HBM_embedding11_char = (char*) malloc(HBM_embedding11_size);
    char* HBM_embedding12_char = (char*) malloc(HBM_embedding12_size);
    char* HBM_embedding13_char = (char*) malloc(HBM_embedding13_size);
    char* HBM_embedding14_char = (char*) malloc(HBM_embedding14_size);
    char* HBM_embedding15_char = (char*) malloc(HBM_embedding15_size);
    char* HBM_embedding16_char = (char*) malloc(HBM_embedding16_size);
    char* HBM_embedding17_char = (char*) malloc(HBM_embedding17_size);
    char* HBM_embedding18_char = (char*) malloc(HBM_embedding18_size);
    char* HBM_embedding19_char = (char*) malloc(HBM_embedding19_size);
    char* HBM_embedding20_char = (char*) malloc(HBM_embedding20_size);

    char* HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_char = 
        (char*) malloc(HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_size);
    char* HBM_query_vector_char = (char*) malloc(HBM_query_vector_size);
    char* HBM_vector_quantizer_char = (char*) malloc(HBM_vector_quantizer_size);
    char* HBM_product_quantizer_char = (char*) malloc(HBM_product_quantizer_size);

    char* sw_result_vec_ID_char = (char*) malloc(sw_result_vec_ID_size);
    char* sw_result_dist_char = (char*) malloc(sw_result_dist_size);

    std::ifstream HBM_embedding0_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_0_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding1_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_1_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding2_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_2_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding3_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_3_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding4_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_4_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding5_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_5_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding6_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_6_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding7_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_7_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding8_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_8_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding9_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_9_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding10_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_10_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding11_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_11_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding12_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_12_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding13_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_13_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding14_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_14_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding15_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_15_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding16_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_16_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding17_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_17_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding18_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_18_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding19_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_19_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_embedding20_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_bank_20_raw", 
        std::ios::in | std::ios::binary);

    std::ifstream HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_3_by_8192_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_query_vector_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/query_vectors_float32_10000_128_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_vector_quantizer_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/vector_quantizer_float32_8192_128_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream HBM_product_quantizer_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/product_quantizer_float32_16_256_8_raw", 
        std::ios::in | std::ios::binary);

    std::ifstream sw_result_vec_ID_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/result_index_int32_10000_10_raw", 
        std::ios::in | std::ios::binary);
    std::ifstream sw_result_dist_fstream(
        "/mnt/scratch/wenqi/FPGA-ANNS/saved_npy_data/FPGA_data_SIFT100M_IVF8192,PQ16/result_distance_float32_10000_10_raw", 
        std::ios::in | std::ios::binary);

    HBM_embedding0_fstream.read(HBM_embedding0_char, HBM_embedding0_size);
    HBM_embedding1_fstream.read(HBM_embedding1_char, HBM_embedding1_size);
    HBM_embedding2_fstream.read(HBM_embedding2_char, HBM_embedding2_size);
    HBM_embedding3_fstream.read(HBM_embedding3_char, HBM_embedding3_size);
    HBM_embedding4_fstream.read(HBM_embedding4_char, HBM_embedding4_size);
    HBM_embedding5_fstream.read(HBM_embedding5_char, HBM_embedding5_size);
    HBM_embedding6_fstream.read(HBM_embedding6_char, HBM_embedding6_size);
    HBM_embedding7_fstream.read(HBM_embedding7_char, HBM_embedding7_size);
    HBM_embedding8_fstream.read(HBM_embedding8_char, HBM_embedding8_size);
    HBM_embedding9_fstream.read(HBM_embedding9_char, HBM_embedding9_size);
    HBM_embedding10_fstream.read(HBM_embedding10_char, HBM_embedding10_size);
    HBM_embedding11_fstream.read(HBM_embedding11_char, HBM_embedding11_size);
    HBM_embedding12_fstream.read(HBM_embedding12_char, HBM_embedding12_size);
    HBM_embedding13_fstream.read(HBM_embedding13_char, HBM_embedding13_size);
    HBM_embedding14_fstream.read(HBM_embedding14_char, HBM_embedding14_size);
    HBM_embedding15_fstream.read(HBM_embedding15_char, HBM_embedding15_size);
    HBM_embedding16_fstream.read(HBM_embedding16_char, HBM_embedding16_size);
    HBM_embedding17_fstream.read(HBM_embedding17_char, HBM_embedding17_size);
    HBM_embedding18_fstream.read(HBM_embedding18_char, HBM_embedding18_size);
    HBM_embedding19_fstream.read(HBM_embedding19_char, HBM_embedding19_size);
    HBM_embedding20_fstream.read(HBM_embedding20_char, HBM_embedding20_size);

    HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_fstream.read(
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_char,
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_size);
    HBM_query_vector_fstream.read(HBM_query_vector_char, HBM_query_vector_size);
    HBM_vector_quantizer_fstream.read(HBM_vector_quantizer_char, HBM_vector_quantizer_size);
    HBM_product_quantizer_fstream.read(HBM_product_quantizer_char, HBM_product_quantizer_size);

    sw_result_vec_ID_fstream.read(sw_result_vec_ID_char, sw_result_vec_ID_size);
    sw_result_dist_fstream.read(sw_result_dist_char, sw_result_dist_size);

    // std::cout << "HBM_query_vector_fstream read bytes: " << HBM_query_vector_fstream.gcount() << std::endl;
    // std::cout << "HBM_vector_quantizer_fstream read bytes: " << HBM_vector_quantizer_fstream.gcount() << std::endl;
    // std::cout << "HBM_product_quantizer_fstream read bytes: " << HBM_product_quantizer_fstream.gcount() << std::endl;
 
    // HBM_embedding0
   
    memcpy(&HBM_embedding0[0], HBM_embedding0_char, HBM_embedding0_size); 
    memcpy(&HBM_embedding1[0], HBM_embedding1_char, HBM_embedding1_size); 
    memcpy(&HBM_embedding2[0], HBM_embedding2_char, HBM_embedding2_size); 
    memcpy(&HBM_embedding3[0], HBM_embedding3_char, HBM_embedding3_size); 
    memcpy(&HBM_embedding4[0], HBM_embedding4_char, HBM_embedding4_size); 
    memcpy(&HBM_embedding5[0], HBM_embedding5_char, HBM_embedding5_size); 
    memcpy(&HBM_embedding6[0], HBM_embedding6_char, HBM_embedding6_size); 
    memcpy(&HBM_embedding7[0], HBM_embedding7_char, HBM_embedding7_size); 
    memcpy(&HBM_embedding8[0], HBM_embedding8_char, HBM_embedding8_size); 
    memcpy(&HBM_embedding9[0], HBM_embedding9_char, HBM_embedding9_size); 
    memcpy(&HBM_embedding10[0], HBM_embedding10_char, HBM_embedding10_size); 
    memcpy(&HBM_embedding11[0], HBM_embedding11_char, HBM_embedding11_size); 
    memcpy(&HBM_embedding12[0], HBM_embedding12_char, HBM_embedding12_size); 
    memcpy(&HBM_embedding13[0], HBM_embedding13_char, HBM_embedding13_size); 
    memcpy(&HBM_embedding14[0], HBM_embedding14_char, HBM_embedding14_size); 
    memcpy(&HBM_embedding15[0], HBM_embedding15_char, HBM_embedding15_size); 
    memcpy(&HBM_embedding16[0], HBM_embedding16_char, HBM_embedding16_size); 
    memcpy(&HBM_embedding17[0], HBM_embedding17_char, HBM_embedding17_size); 
    memcpy(&HBM_embedding18[0], HBM_embedding18_char, HBM_embedding18_size); 
    memcpy(&HBM_embedding19[0], HBM_embedding19_char, HBM_embedding19_size); 
    memcpy(&HBM_embedding20[0], HBM_embedding20_char, HBM_embedding20_size); 

    memcpy(&HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid[0], 
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_char, 
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_size);
    memcpy(&HBM_query_vectors[0], HBM_query_vector_char, HBM_query_vector_size);
    memcpy(&HBM_vector_quantizer[0], HBM_vector_quantizer_char, HBM_vector_quantizer_size);
    memcpy(&HBM_product_quantizer[0], HBM_product_quantizer_char, HBM_product_quantizer_size);

    memcpy(&sw_result_vec_ID[0], sw_result_vec_ID_char, sw_result_vec_ID_size);
    memcpy(&sw_result_dist[0], sw_result_dist_char, sw_result_dist_size);

    free(HBM_embedding0_char);
    free(HBM_embedding1_char);
    free(HBM_embedding2_char);
    free(HBM_embedding3_char);
    free(HBM_embedding4_char);
    free(HBM_embedding5_char);
    free(HBM_embedding6_char);
    free(HBM_embedding7_char);
    free(HBM_embedding8_char);
    free(HBM_embedding9_char);
    free(HBM_embedding10_char);
    free(HBM_embedding11_char);
    free(HBM_embedding12_char);
    free(HBM_embedding13_char);
    free(HBM_embedding14_char);
    free(HBM_embedding15_char);
    free(HBM_embedding16_char);
    free(HBM_embedding17_char);
    free(HBM_embedding18_char);
    free(HBM_embedding19_char);
    free(HBM_embedding20_char);

    free(HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_char);
    free(HBM_query_vector_char);
    free(HBM_vector_quantizer_char);
    free(HBM_product_quantizer_char);

    free(sw_result_vec_ID_char);
    free(sw_result_dist_char);

// OPENCL HOST CODE AREA START
	
// ------------------------------------------------------------------------------------
// Step 1: Get All PLATFORMS, then search for Target_Platform_Vendor (CL_PLATFORM_VENDOR)
//	   Search for Platform: Xilinx 
// Check if the current platform matches Target_Platform_Vendor
// ------------------------------------------------------------------------------------	
    std::vector<cl::Device> devices = get_devices("Xilinx");
    devices.resize(1);
    cl::Device device = devices[0];
    std::cout << "Finished getting device..." << std::endl;
// ------------------------------------------------------------------------------------
// Step 1: Create Context
// ------------------------------------------------------------------------------------
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
	std::cout << "Finished creating context..." << std::endl;
// ------------------------------------------------------------------------------------
// Step 1: Create Command Queue
// ------------------------------------------------------------------------------------
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
	std::cout << "Finished creating command queue..." << std::endl;
// ------------------------------------------------------------------
// Step 1: Load Binary File from disk
// ------------------------------------------------------------------		
    char* fileBuf = read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    	std::cout << "Finished loading binary..." << std::endl;
	
// -------------------------------------------------------------
// Step 1: Create the program object from the binary and program the FPGA device with it
// -------------------------------------------------------------	
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
	std::cout << "Finished creating program..." << std::endl;
// -------------------------------------------------------------
// Step 1: Create Kernels
// -------------------------------------------------------------
    OCL_CHECK(err, cl::Kernel krnl_vector_add(program,"vadd", &err));
    std::cout << "Finished creating kernel..." << std::endl;

// ================================================================
// Step 2: Setup Buffers and run Kernels
// ================================================================
//   o) Allocate Memory to store the results 
//   o) Create Buffers in Global Memory to store data
// ================================================================

// ------------------------------------------------------------------
// Step 2: Create Buffers in Global Memory to store data
//             o) buffer_in1 - stores source_in1
//             o) buffer_in2 - stores source_in2
//             o) buffer_ouput - stores Results
// ------------------------------------------------------------------	

// .......................................................
// Allocate Global Memory for source_in1
// .......................................................	
//////////////////////////////   TEMPLATE START  //////////////////////////////
    std::cout << "Start to allocate device memory..." << std::endl;
    cl_mem_ext_ptr_t HBM_embedding0Ext, HBM_embedding1Ext, HBM_embedding2Ext, HBM_embedding3Ext, 
        HBM_embedding4Ext, HBM_embedding5Ext, HBM_embedding6Ext, HBM_embedding7Ext, 
        HBM_embedding8Ext, HBM_embedding9Ext, HBM_embedding10Ext, HBM_embedding11Ext, 
        HBM_embedding12Ext, HBM_embedding13Ext, HBM_embedding14Ext, HBM_embedding15Ext, 
        HBM_embedding16Ext, HBM_embedding17Ext, HBM_embedding18Ext, HBM_embedding19Ext, 
        HBM_embedding20Ext, 
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_validExt, // HBM 21
        HBM_query_vectorExt, 
        HBM_vector_quantizerExt, 
        HBM_product_quantizerExt, 
        HBM_outExt;
        /* HBM_embedding25Ext, HBM_embedding26Ext, HBM_embedding27Ext,
        HBM_embedding28Ext, HBM_embedding29Ext, HBM_embedding30Ext, HBM_embedding31Ext, 
        DDR_embedding0Ext, DDR_embedding1Ext, */
        // PLRAM_embedding0Ext, PLRAM_embedding1Ext, PLRAM_embedding2Ext, PLRAM_embedding3Ext, 
        // sourcce_hw_resultsExt;
//////////////////////////////   TEMPLATE END  //////////////////////////////

//////////////////////////////   TEMPLATE START  //////////////////////////////
    HBM_embedding0Ext.obj = HBM_embedding0.data();
    HBM_embedding0Ext.param = 0;
    HBM_embedding0Ext.flags = bank[0];
    HBM_embedding1Ext.obj = HBM_embedding1.data();
    HBM_embedding1Ext.param = 0;
    HBM_embedding1Ext.flags = bank[1];
    HBM_embedding2Ext.obj = HBM_embedding2.data();
    HBM_embedding2Ext.param = 0;
    HBM_embedding2Ext.flags = bank[2];
    HBM_embedding3Ext.obj = HBM_embedding3.data();
    HBM_embedding3Ext.param = 0;
    HBM_embedding3Ext.flags = bank[3];
    HBM_embedding4Ext.obj = HBM_embedding4.data();
    HBM_embedding4Ext.param = 0;
    HBM_embedding4Ext.flags = bank[4];
    HBM_embedding5Ext.obj = HBM_embedding5.data();
    HBM_embedding5Ext.param = 0;
    HBM_embedding5Ext.flags = bank[5];
    HBM_embedding6Ext.obj = HBM_embedding6.data();
    HBM_embedding6Ext.param = 0;
    HBM_embedding6Ext.flags = bank[6];
    HBM_embedding7Ext.obj = HBM_embedding7.data();
    HBM_embedding7Ext.param = 0;
    HBM_embedding7Ext.flags = bank[7];
    HBM_embedding8Ext.obj = HBM_embedding8.data();
    HBM_embedding8Ext.param = 0;
    HBM_embedding8Ext.flags = bank[8];
    HBM_embedding9Ext.obj = HBM_embedding9.data();
    HBM_embedding9Ext.param = 0;
    HBM_embedding9Ext.flags = bank[9];
    HBM_embedding10Ext.obj = HBM_embedding10.data();
    HBM_embedding10Ext.param = 0;
    HBM_embedding10Ext.flags = bank[10];
    HBM_embedding11Ext.obj = HBM_embedding11.data();
    HBM_embedding11Ext.param = 0;
    HBM_embedding11Ext.flags = bank[11];
    HBM_embedding12Ext.obj = HBM_embedding12.data();
    HBM_embedding12Ext.param = 0;
    HBM_embedding12Ext.flags = bank[12];
    HBM_embedding13Ext.obj = HBM_embedding13.data();
    HBM_embedding13Ext.param = 0;
    HBM_embedding13Ext.flags = bank[13];
    HBM_embedding14Ext.obj = HBM_embedding14.data();
    HBM_embedding14Ext.param = 0;
    HBM_embedding14Ext.flags = bank[14];
    HBM_embedding15Ext.obj = HBM_embedding15.data();
    HBM_embedding15Ext.param = 0;
    HBM_embedding15Ext.flags = bank[15];
    HBM_embedding16Ext.obj = HBM_embedding16.data();
    HBM_embedding16Ext.param = 0;
    HBM_embedding16Ext.flags = bank[16];
    HBM_embedding17Ext.obj = HBM_embedding17.data();
    HBM_embedding17Ext.param = 0;
    HBM_embedding17Ext.flags = bank[17];
    HBM_embedding18Ext.obj = HBM_embedding18.data();
    HBM_embedding18Ext.param = 0;
    HBM_embedding18Ext.flags = bank[18];
    HBM_embedding19Ext.obj = HBM_embedding19.data();
    HBM_embedding19Ext.param = 0;
    HBM_embedding19Ext.flags = bank[19];
    HBM_embedding20Ext.obj = HBM_embedding20.data();
    HBM_embedding20Ext.param = 0;
    HBM_embedding20Ext.flags = bank[20];

    HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_validExt.obj = 
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid.data();
    HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_validExt.param = 0;
    HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_validExt.flags = bank[21];


    HBM_query_vectorExt.obj = HBM_query_vectors.data();
    HBM_query_vectorExt.param = 0;
    HBM_query_vectorExt.flags = bank[22];

    HBM_vector_quantizerExt.obj = HBM_vector_quantizer.data();
    HBM_vector_quantizerExt.param = 0;
    HBM_vector_quantizerExt.flags = bank[23];

    HBM_product_quantizerExt.obj = HBM_product_quantizer.data();
    HBM_product_quantizerExt.param = 0;
    HBM_product_quantizerExt.flags = bank[24];

    HBM_outExt.obj = HBM_out.data();
    HBM_outExt.param = 0;
    HBM_outExt.flags = bank[25];

    // HBM_embedding21Ext.obj = HBM_embedding21.data();
    // HBM_embedding21Ext.param = 0;
    // HBM_embedding21Ext.flags = bank[21];
    // HBM_embedding22Ext.obj = HBM_embedding22.data();
    // HBM_embedding22Ext.param = 0;
    // HBM_embedding22Ext.flags = bank[22];
    // HBM_embedding23Ext.obj = HBM_embedding23.data();
    // HBM_embedding23Ext.param = 0;
    // HBM_embedding23Ext.flags = bank[23];
    // HBM_embedding24Ext.obj = HBM_embedding24.data();
    // HBM_embedding24Ext.param = 0;
    // HBM_embedding24Ext.flags = bank[24];
    // HBM_embedding25Ext.obj = HBM_embedding25.data();
    // HBM_embedding25Ext.param = 0;
    // HBM_embedding25Ext.flags = bank[25];
    // HBM_embedding26Ext.obj = HBM_embedding26.data();
    // HBM_embedding26Ext.param = 0;
    // HBM_embedding26Ext.flags = bank[26];
    // HBM_embedding27Ext.obj = HBM_embedding27.data();
    // HBM_embedding27Ext.param = 0;
    // HBM_embedding27Ext.flags = bank[27];
//     HBM_embedding28Ext.obj = HBM_embedding28.data();
//     HBM_embedding28Ext.param = 0;
//     HBM_embedding28Ext.flags = bank[28];
//     HBM_embedding29Ext.obj = HBM_embedding29.data();
//     HBM_embedding29Ext.param = 0;
//     HBM_embedding29Ext.flags = bank[29];
//     HBM_embedding30Ext.obj = HBM_embedding30.data();
//     HBM_embedding30Ext.param = 0;
//     HBM_embedding30Ext.flags = bank[30];
//     HBM_embedding31Ext.obj = HBM_embedding31.data();
//     HBM_embedding31Ext.param = 0;
//     HBM_embedding31Ext.flags = bank[31];

//     DDR_embedding0Ext.obj = DDR_embedding0.data();
//     DDR_embedding0Ext.param = 0;
//     DDR_embedding0Ext.flags = bank[0 + 32];
//     DDR_embedding1Ext.obj = DDR_embedding1.data();
//     DDR_embedding1Ext.param = 0;
//     DDR_embedding1Ext.flags = bank[1 + 32];

    // PLRAM
    // sourcce_hw_resultsExt.obj = source_hw_results.data();
    // sourcce_hw_resultsExt.param = 0;
    // sourcce_hw_resultsExt.flags = bank[34];

    // CL_MEM_EXT_PTR_XILINX
//////////////////////////////   TEMPLATE START  //////////////////////////////
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding0(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding0_size, &HBM_embedding0Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding1_size, &HBM_embedding1Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding2_size, &HBM_embedding2Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding3(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding3_size, &HBM_embedding3Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding4(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding4_size, &HBM_embedding4Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding5(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding5_size, &HBM_embedding5Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding6(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding6_size, &HBM_embedding6Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding7(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding7_size, &HBM_embedding7Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding8(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding8_size, &HBM_embedding8Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding9(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding9_size, &HBM_embedding9Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding10(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding10_size, &HBM_embedding10Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding11(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding11_size, &HBM_embedding11Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding12(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding12_size, &HBM_embedding12Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding13(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding13_size, &HBM_embedding13Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding14(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding14_size, &HBM_embedding14Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding15(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding15_size, &HBM_embedding15Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding16(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding16_size, &HBM_embedding16Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding17(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding17_size, &HBM_embedding17Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding18(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding18_size, &HBM_embedding18Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding19(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding19_size, &HBM_embedding19Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding20(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding20_size, &HBM_embedding20Ext, &err));

    OCL_CHECK(err, cl::Buffer buffer_HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid(
        context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
        HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid_size, 
        &HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_validExt, &err));

    OCL_CHECK(err, cl::Buffer buffer_HBM_query_vectors(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_query_vector_size, &HBM_query_vectorExt, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_vector_quantizer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_vector_quantizer_size, &HBM_vector_quantizerExt, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_product_quantizer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_product_quantizer_size, &HBM_product_quantizerExt, &err));
    
// .......................................................
// Allocate Global Memory for sourcce_hw_results
// .......................................................
    OCL_CHECK(err, cl::Buffer buffer_output(
        context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX, 
        HBM_out_size, &HBM_outExt, &err));

// ============================================================================
// Step 2: Set Kernel Arguments and Run the Application
//         o) Set Kernel Arguments
//         o) Copy Input Data from Host to Global Memory on the device
//         o) Submit Kernels for Execution
//         o) Copy Results from Global Memory, device to Host
// ============================================================================	
    
//////////////////////////////   TEMPLATE START  //////////////////////////////
    OCL_CHECK(err, err = krnl_vector_add.setArg(0, buffer_HBM_embedding0));
    OCL_CHECK(err, err = krnl_vector_add.setArg(1, buffer_HBM_embedding1));
    OCL_CHECK(err, err = krnl_vector_add.setArg(2, buffer_HBM_embedding2));
    OCL_CHECK(err, err = krnl_vector_add.setArg(3, buffer_HBM_embedding3));
    OCL_CHECK(err, err = krnl_vector_add.setArg(4, buffer_HBM_embedding4));
    OCL_CHECK(err, err = krnl_vector_add.setArg(5, buffer_HBM_embedding5));
    OCL_CHECK(err, err = krnl_vector_add.setArg(6, buffer_HBM_embedding6));
    OCL_CHECK(err, err = krnl_vector_add.setArg(7, buffer_HBM_embedding7));
    OCL_CHECK(err, err = krnl_vector_add.setArg(8, buffer_HBM_embedding8));
    OCL_CHECK(err, err = krnl_vector_add.setArg(9, buffer_HBM_embedding9));
    OCL_CHECK(err, err = krnl_vector_add.setArg(10, buffer_HBM_embedding10));
    OCL_CHECK(err, err = krnl_vector_add.setArg(11, buffer_HBM_embedding11));
    OCL_CHECK(err, err = krnl_vector_add.setArg(12, buffer_HBM_embedding12));
    OCL_CHECK(err, err = krnl_vector_add.setArg(13, buffer_HBM_embedding13));
    OCL_CHECK(err, err = krnl_vector_add.setArg(14, buffer_HBM_embedding14));
    OCL_CHECK(err, err = krnl_vector_add.setArg(15, buffer_HBM_embedding15));
    OCL_CHECK(err, err = krnl_vector_add.setArg(16, buffer_HBM_embedding16));
    OCL_CHECK(err, err = krnl_vector_add.setArg(17, buffer_HBM_embedding17));
    OCL_CHECK(err, err = krnl_vector_add.setArg(18, buffer_HBM_embedding18));
    OCL_CHECK(err, err = krnl_vector_add.setArg(19, buffer_HBM_embedding19));
    OCL_CHECK(err, err = krnl_vector_add.setArg(20, buffer_HBM_embedding20));
    OCL_CHECK(err, err = krnl_vector_add.setArg(21, buffer_HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid));
    OCL_CHECK(err, err = krnl_vector_add.setArg(22, buffer_HBM_query_vectors));
    OCL_CHECK(err, err = krnl_vector_add.setArg(23, buffer_HBM_vector_quantizer));
    OCL_CHECK(err, err = krnl_vector_add.setArg(24, buffer_HBM_product_quantizer));

    OCL_CHECK(err, err = krnl_vector_add.setArg(25, buffer_output));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(26, query_num));
//////////////////////////////   TEMPLATE END  //////////////////////////////
// ------------------------------------------------------
// Step 2: Copy Input data from Host to Global Memory on the device
// ------------------------------------------------------
//////////////////////////////   TEMPLATE START  //////////////////////////////
    std::cout << "Starting copy from Host to device..." << std::endl;
    OCL_CHECK(
        err, err = q.enqueueMigrateMemObjects({
        buffer_HBM_embedding0, buffer_HBM_embedding1, buffer_HBM_embedding2, buffer_HBM_embedding3, 
        buffer_HBM_embedding4, buffer_HBM_embedding5, buffer_HBM_embedding6, buffer_HBM_embedding7, 
        buffer_HBM_embedding8, buffer_HBM_embedding9, buffer_HBM_embedding10, buffer_HBM_embedding11, 
        buffer_HBM_embedding12, buffer_HBM_embedding13, buffer_HBM_embedding14, buffer_HBM_embedding15, 
        buffer_HBM_embedding16, buffer_HBM_embedding17, buffer_HBM_embedding18, buffer_HBM_embedding19, 
        buffer_HBM_embedding20, buffer_HBM_info_start_addr_and_scanned_entries_every_cell_and_last_element_valid,
        buffer_HBM_query_vectors,
        buffer_HBM_vector_quantizer,
        buffer_HBM_product_quantizer
        /*, buffer_HBM_embedding21, buffer_HBM_embedding22, buffer_HBM_embedding23, 
        buffer_HBM_embedding24, buffer_HBM_embedding25, buffer_HBM_embedding26, buffer_HBM_embedding27, 
        buffer_HBM_embedding28, buffer_HBM_embedding29, buffer_HBM_embedding30, buffer_HBM_embedding31, 
        buffer_DDR_embedding0, buffer_DDR_embedding1*/}, 0/* 0 means from host*/));	
    std::cout << "Host to device finished..." << std::endl;
//////////////////////////////   TEMPLATE END  //////////////////////////////
// ----------------------------------------
// Step 2: Submit Kernels for Execution
// ----------------------------------------
    OCL_CHECK(err, err = q.enqueueTask(krnl_vector_add));
// --------------------------------------------------
// Step 2: Copy Results from Device Global Memory to Host
// --------------------------------------------------
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST));

    q.finish();
// OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    // only check the last batch (since other are not transfered back)
    std::cout << "Comparing Results..." << std::endl;
    bool match = true;


    for (int query_id = 0; query_id < query_num; query_id++) {

        std::vector<int> hw_result_vec_ID_partial(PRIORITY_QUEUE_LEN, 0);
        std::vector<float> hw_result_dist_partial(PRIORITY_QUEUE_LEN, 0);

        std::vector<int> sw_result_vec_ID_partial(PRIORITY_QUEUE_LEN, 0);
        std::vector<float> sw_result_dist_partial(PRIORITY_QUEUE_LEN, 0);

        // Load data
        for (int k = 0; k < PRIORITY_QUEUE_LEN; k++) {

            ap_uint<64> reg = HBM_out[query_id * PRIORITY_QUEUE_LEN + k];
            ap_uint<32> raw_vec_ID = reg.range(31, 0); 
            ap_uint<32>  raw_dist = reg.range(63, 32);
            int vec_ID = *((int*) (&raw_vec_ID));
            float dist = *((float*) (&raw_dist));
            
            hw_result_vec_ID_partial[k] = vec_ID;
            hw_result_dist_partial[k] = dist;

            sw_result_vec_ID_partial[k] = sw_result_vec_ID[query_id * PRIORITY_QUEUE_LEN + k];
            sw_result_dist_partial[k] = sw_result_dist[query_id * PRIORITY_QUEUE_LEN + k];
        }

        std::sort(hw_result_vec_ID_partial.begin(), hw_result_vec_ID_partial.end());
        std::sort(hw_result_dist_partial.begin(), hw_result_dist_partial.end());

        std::sort(sw_result_vec_ID_partial.begin(), sw_result_vec_ID_partial.end());
        std::sort(sw_result_dist_partial.begin(), sw_result_dist_partial.end());

        // Check correctness
        for (int k = 0; k < PRIORITY_QUEUE_LEN; k++) {
            if (hw_result_vec_ID_partial[k] != sw_result_vec_ID_partial[k]) {
                printf("query_id: %d\tk: %d\thw vec_ID: %d\t sw vec_ID:%d\n",
                    query_id, k, hw_result_vec_ID_partial[k], sw_result_vec_ID_partial[k]);
            }
            if (hw_result_dist_partial[k] != sw_result_dist_partial[k]) {
                printf("query_id: %d\tk: %d\thw dist: %f\t sw dist:%f\n",
                    query_id, k, hw_result_vec_ID_partial[k], sw_result_vec_ID_partial[k]);
            }
        } 
    }

// ============================================================================
// Step 3: Release Allocated Resources
// ============================================================================
    delete[] fileBuf;

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}

