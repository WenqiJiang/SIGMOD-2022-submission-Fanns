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
    size_t HBM_embedding0_size =  HBM_BANK0_SIZE;
    size_t HBM_embedding1_size =  HBM_BANK1_SIZE;
    size_t HBM_embedding2_size =  HBM_BANK2_SIZE;
    size_t HBM_embedding3_size =  HBM_BANK3_SIZE;
    size_t HBM_embedding4_size =  HBM_BANK4_SIZE;
    size_t HBM_embedding5_size =  HBM_BANK5_SIZE;
    size_t HBM_embedding6_size =  HBM_BANK6_SIZE;
    size_t HBM_embedding7_size =  HBM_BANK7_SIZE;
    size_t HBM_embedding8_size =  HBM_BANK8_SIZE;
    size_t HBM_embedding9_size =  HBM_BANK9_SIZE;
    size_t HBM_embedding10_size =  HBM_BANK10_SIZE;
    size_t HBM_embedding11_size =  HBM_BANK11_SIZE;
    size_t HBM_embedding12_size =  HBM_BANK12_SIZE;
    size_t HBM_embedding13_size =  HBM_BANK13_SIZE;
    size_t HBM_embedding14_size =  HBM_BANK14_SIZE;
    size_t HBM_embedding15_size =  HBM_BANK15_SIZE;
    size_t HBM_embedding16_size =  HBM_BANK16_SIZE;
    size_t HBM_embedding17_size =  HBM_BANK17_SIZE;
    size_t HBM_embedding18_size =  HBM_BANK18_SIZE;
    size_t HBM_embedding19_size =  HBM_BANK19_SIZE;
    size_t HBM_embedding20_size =  HBM_BANK20_SIZE;
    size_t HBM_embedding21_size =  HBM_BANK21_SIZE;
    size_t HBM_embedding22_size =  HBM_BANK22_SIZE;
    size_t HBM_embedding23_size =  HBM_BANK23_SIZE;
    size_t HBM_embedding24_size =  HBM_BANK24_SIZE;
    size_t HBM_embedding25_size =  HBM_BANK25_SIZE;
    size_t HBM_embedding26_size =  HBM_BANK26_SIZE;
    size_t HBM_embedding27_size =  HBM_BANK27_SIZE;
    size_t HBM_embedding28_size =  HBM_BANK28_SIZE;
    size_t HBM_embedding29_size =  HBM_BANK29_SIZE;
    size_t HBM_embedding30_size =  HBM_BANK30_SIZE;
    size_t HBM_embedding31_size =  HBM_BANK31_SIZE;

    size_t DDR_embedding0_size =  DDR_BANK0_SIZE;
    size_t DDR_embedding1_size =  DDR_BANK1_SIZE;

    size_t PLRAM_embedding0_size =  PLRAM_BANK0_SIZE;
    size_t PLRAM_embedding1_size =  PLRAM_BANK1_SIZE;
    size_t PLRAM_embedding2_size =  PLRAM_BANK2_SIZE;
    size_t PLRAM_embedding3_size =  PLRAM_BANK3_SIZE;
//////////////////////////////   TEMPLATE END  //////////////////////////////

    cl_int err;
    unsigned fileBufSize;

    // allocate aligned 2D vectors
//////////////////////////////   TEMPLATE START  //////////////////////////////
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding0(HBM_embedding0_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding1(HBM_embedding1_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding2(HBM_embedding2_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding3(HBM_embedding3_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding4(HBM_embedding4_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding5(HBM_embedding5_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding6(HBM_embedding6_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding7(HBM_embedding7_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding8(HBM_embedding8_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding9(HBM_embedding9_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding10(HBM_embedding10_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding11(HBM_embedding11_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding12(HBM_embedding12_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding13(HBM_embedding13_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding14(HBM_embedding14_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding15(HBM_embedding15_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding16(HBM_embedding16_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding17(HBM_embedding17_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding18(HBM_embedding18_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding19(HBM_embedding19_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding20(HBM_embedding20_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding21(HBM_embedding21_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding22(HBM_embedding22_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding23(HBM_embedding23_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding24(HBM_embedding24_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding25(HBM_embedding25_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding26(HBM_embedding26_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding27(HBM_embedding27_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding28(HBM_embedding28_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding29(HBM_embedding29_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding30(HBM_embedding30_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> HBM_embedding31(HBM_embedding31_size, 0);


    std::vector<D_TYPE, aligned_allocator<D_TYPE>> PLRAM_embedding0(PLRAM_embedding0_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> PLRAM_embedding1(PLRAM_embedding1_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> PLRAM_embedding2(PLRAM_embedding2_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> PLRAM_embedding3(PLRAM_embedding3_size, 0);

    std::vector<D_TYPE, aligned_allocator<D_TYPE>> DDR_embedding0(DDR_embedding0_size, 0);
    std::vector<D_TYPE, aligned_allocator<D_TYPE>> DDR_embedding1(DDR_embedding1_size, 0);

//////////////////////////////   TEMPLATE END  //////////////////////////////

    int size_results_out = 1024; 
    std::vector<ap_uint<64>,aligned_allocator<ap_uint<64>>> source_hw_results(size_results_out);
    std::vector<float,aligned_allocator<float>> source_sw_results(size_results_out);

//// INIT SORT array ////
    float input_array[1024] = {
        5256, -1494, -8828,  4251,  2305,  1793,  1830,  8083,  9249,
       -6040,  3349,  2380,  -130,  8798, -1263, -3422,  7605, -8434,
       -4720,  2938,  3222,  6589, -4546,  4860,  7626,  4678, -7294,
       -5041, -1096, -8786,  5444, -6519,  1572, -2383, -5454, -4508,
        7713,   972,  1464,   992, -9367, -7912,  2275, -1215, -2461,
       -9107,  6335, -4032,  8585,  1179, -5751,   224,  5573, -3108,
       -7120,  5960,  7803,  7760, -3963,  4767, -3604,  6238, -9997,
        8998,  6499,  5265, -1601,  5377,  3523, -4487, -8662, -1346,
         979, -2919, -5950, -3111, -1360,  9303,  1990, -2518, -1129,
       -7067, -5260,  1732, -6553,  7192,   879,  2357, -5798,  5235,
        4595,   146, -5791,  8801, -6225, -9302,  2682,  8233, -4885,
        6725,  4390,  3931, -6332,  5863, -9875, -2278,  9367,  3312,
        6992,  -683, -7296,  -154,  3713,  3132,    54, -5469,  4595,
        4916,  2034,  6203,  9077, -4029,  7877, -9918, -1189, -6393,
        8414,  9655,  5625,  4994, -3952, -1690, -5622,  -731,  1224,
        1770,  6688, -5562,  9014,  -213,   941, -7466,  5985,  -194,
       -2042,  8883,  8027, -7499,  9071, -7204, -3366, -9184,  4455,
       -5381,  3020, -2310, -5776,  1352, -4843,  9944,  7091, -1425,
       -1204,   196,  2987,  1814,  9055,  4571,  9738, -3375,  -417,
        6459,  7245, -6927,  2229, -6861, -2766, -1748,   353,  5320,
       -2300, -3417, -9150,  9661,  8998, -4048,  4850,  5646, -9777,
        8332,  5716,  8294, -5353,  -407,  8484, -9837,  2890, -5380,
       -8126,  -693,   659, -4836,  6847, -1238, -1007,  -767, -5515,
       -5122,  5746, -4654,  4251, -6268,  1789, -2320,   887, -9694,
       -9803, -2650, -2222,  7565,  2280, -1911, -6525,  9826, -5629,
        4120,  5013,  4236,  3470,  9073,  2309,  -557, -8134,  4121,
        3056, -8237, -2942, -7390, -6121,   871, -3200, -3025,  1636,
        6637,  3208,   564, -6816, -9650, -4054, -4583,  7914, -8761,
         462, -7422,  1668, -8969, -9983,  1136, -4245, -6564, -8012,
        -667, -5043,  4526, -5256,  6466,  3149,  9234, -8004,  4880,
        4346,  1870,  7028,  -969,  2447, -1404,  2175,  2809,  7136,
        9557, -9216, -8399,   -73, -3585,  8567,  -381, -4411,  8148,
       -7005,  9501, -4649,  8104,  3695, -9824,   903, -8933, -4712,
       -2648,  9055,  6175,  4988, -2035,  3844,  7938, -1240,  7088,
        8338,  -391, -2376, -4847,  5700,  6784, -8951,  2160,  9621,
       -8308,  4138,  4163,  1764,  3916,  2204, -2210, -7711,  5473,
        5276, -2655,  5018,   837, -2212,  6784,  7018, -6369,  1780,
       -7110,  1014,  2293,  6338,  -870, -6226,  1992,   308, -2450,
       -1820,  8828, -9127, -1901,  9311, -6265, -1474,  5859, -6747,
        6685, -3012, -2302, -8485,  9276, -8006,  1345,  7847, -7463,
         -96, -9560, -7632,  3974, -3928,  6582, -5067,  9684, -6274,
        3583, -2736,  1643,  2614,   598,  6256, -7335,  1245,  3489,
         456, -4516,  2113, -5480,  -368,  7274,  8814,  3681,  9350,
       -6986,  9557,  3744, -8861, -3142, -3364,  3869, -4648, -8126,
       -8251, -4591,  2912,  -723, -1355,   584, -6888, -7470, -7153,
         999,  8308, -3241,  1516,  7405, -5173, -4333, -7744, -7210,
       -8660, -4653, -1101, -9664,  7120,  8802, -3131, -8745,  3482,
        5682,  1363,  4210, -3902, -8697, -7600,  7263,  5142, -5061,
       -1406,  -218, -9637, -4705, -9813,  2473,  2132,   -93,  5819,
        8784, -4654,  9412, -2679,  8158, -8887,  3365,  -500, -3385,
        7061, -1784, -2768,  8946, -2660, -2459,   -14,  1116, -2413,
       -4600, -9489, -6246, -4750,  8392,  -693, -1642,  9910,  9840,
        5904, -5856,  9453, -8308, -6369, -1945,  1000,  6852, -9590,
         476,  9744, -8960,  8497,  6658,  5010,  6719,  -346,  8916,
       -9413,   441,    25, -8188,  2326,  7622,  5596,  7978,  4881,
        6866,   593,  9249, -5615,  1121, -8158, -1286, -5182,  9442,
        -856,  4395,  6646,  8563, -6180,  3103, -4406, -3968,  2060,
        1961,  1202, -9000,  6923, -6141, -8872, -2390, -7610,  5719,
       -1545, -1340,  1990, -1921, -8489, -2442,  8457, -6537, -7235,
        -180,  3071, -3911,  -289,  7370, -5962,  7815,  7597,  7342,
       -3588,  2485,  7917,  3765, -2161, -5113, -5493,  9727, -6802,
       -1668,  3330,  1960,  -189,   483, -6008, -8937,   -84,   351,
        2473, -6207,  3426,  7627, -5729,  5609,  5500, -1388,  7528,
        8842, -8403,  -770, -4874, -4438,   -92, -2408,  1096, -5512,
        9923,  3093,  5840,  6287, -4007,  3605,  -408, -9594, -3010,
       -1965,  4348, -8793, -3883,  2850, -7450,  -695,  2248, -1916,
        1963,  6029, -8874, -2468,  -421, -3111, -2997,  5880, -2112,
        8275,  9549,  3100,  -647, -6968, -3990,  4136, -3222, -7090,
        8819,  -630,  4049,  4141,  8399, -6071,  3741, -1830,  2649,
         397, -3867,  2981,  7454, -2065, -5176, -5802,   729,  4099,
         616, -9571, -7616,  3786, -4021,  7264,  9832, -1565,  8792,
       -8689, -4828,  4107, -2336, -1373,  7815, -8899, -1729,  -575,
       -3648,  9231,  6879,  1627, -8926,   934,  5079,  2610, -6757,
        3224,   461, -2893, -9780, -2453, -7484, -2984, -7270, -8565,
       -5542,   432,   878, -7209,  2414,  3162,  4403, -3760,  5706,
       -4111, -9214, -8296,  8311,   865, -7027, -6298, -9260, -5459,
       -5575, -8630,  5886, -6806,  8331,   -55, -9400,  4648,  2517,
       -9949, -1827,  1307,  6593,  9280,  4618, -8987,  3827, -9819,
       -2349,  6393,  -792, -7989,  -613, -3273, -6269,  2805,  7281,
         541,  9823, -6750, -9117,  3947, -4663,  3007,  9070, -1048,
       -3406, -5665, -7783, -2119,  2867,  5325, -2639, -4348, -4662,
       -6279,  1581, -8062, -4476, -4381, -9958,  2378, -9407,  -989,
       -5682,  3571,  5907,    53, -3623, -3471, -2982,  -935,  3736,
        2183, -5677,  8221,  5008, -5168,  9170,   456, -5815, -6730,
        8935,  9962, -3894,  6405, -9475,  7858, -2411, -2542, -2310,
       -6624,   357, -3398, -8890,  5115, -9627, -4454,  7120,  5487,
       -1061, -6699, -3019,  3821,  3174,  2575, -9658, -9345,   -78,
         858,  -641, -4737,  9240, -2254, -1137, -8987, -9164,  5618,
        -883, -9384, -8022,  7294,  7543,  1635, -2526, -5789,  6702,
       -9253,  2349,  7430,  4764,  6153, -4168,  4498,  8534, -1786,
       -9926,  4843,  8753,  8544,  9640,  3891, -3482,   861,  -381,
       -9039, -8006,  -404, -3476, -1917, -6236, -1101,  -395,  8531,
       -7282,  1116, -3742, -7389, -6297,  9756,    42,   491,  5476,
       -4500,  2953,  5456,  -950, -5430, -6488,   665, -3515, -1915,
        7214, -5726, -4098,  4754,  7820,  6775,  1896,   433, -1441,
        2611,  5254,  5093,  5861,  8073, -5462,  2936,  8457,  -365,
        5958,  5871,  9090, -6486,  8807,  -385,  8408,  9590, -9730,
        1681,  8560, -4894,  8216, -6564,   345, -8400,   887,  2330,
       -7282,  2566,  2734,  2110,  4396, -4359, -8043,  8410, -1843,
        4591, -7657,  7941, -1392,   562, -4050,  5643,  5096,  6482,
       -6236,  6019,  7785, -4817,  6472, -2501, -1459,  9651, -3414,
       -3589,   813, -1787, -7388, -2490,   900,  8933,  -950, -3508,
        6631,  9369, -4701,  9098, -4787,   847,  -431,  4840, -3817,
       -2118,  2166,  7147,  7153, -4891,  3604, -5155,  2392, -7365,
       -7247,  2248,  2960, -5343, -1888,  5238,  3087, -6093, -8898,
       -5998, -8702,  9023, -4295, -2075, -7630,  2244, -8795, -7121,
        8715, -5756,  4779, -3751,  8394, -8137,  8165,  5100, -8723,
       -2043,  1222,  7436,  2673, -2323,  -942,  6183, -3026, -1954,
       -2996,  4739,  1842, -8815,  4571,  1475, -6799,  4145,  3049,
         442,  8318,  9296,  7409,  5460,  -404, -1067,  7370,  5325,
       -7154,  1648, -4560, -7729,  3823,  5616,  9167,  3878, -3898,
         586,  7090,  1100,  9835,  4849, -2890, -5659, -4558,  9663,
        1167, -5682,   905, -2321,  2194,  8753, -6307,  6347,  6634,
       -1420, -9400, -2260, -9070,  7616,  5229,  9138
    };
        // Correct result: 
//       [-9997, -9983, -9958, -9949, -9926, -9918, -9875, -9837, -9824,
//        -9819, -9813, -9803, -9780, -9777, -9730, -9694, -9664, -9658,
//        -9650, -9637, -9627, -9594, -9590, -9571, -9560, -9489, -9475,
//        -9413, -9407, -9400, -9400, -9384, -9367, -9345, -9302, -9260,
//        -9253, -9216, -9214, -9184, -9164, -9150, -9127, -9117, -9107,
//        -9070, -9039, -9000, -8987, -8987, -8969, -8960, -8951, -8937,
//        -8933, -8926, -8899, -8898, -8890, -8887, -8874, -8872, -8861,
//        -8828, -8815, -8795, -8793, -8786, -8761, -8745, -8723, -8702,
//        -8697, -8689, -8662, -8660, -8630, -8565, -8489, -8485, -8434,
//        -8403, -8400, -8399, -8308, -8308, -8296, -8251, -8237, -8188,
//        -8158, -8137, -8134, -8126, -8126, -8062, -8043, -8022, -8012,
//        -8006, -8006, -8004, -7989, -7912, -7783, -7744, -7729, -7711,
//        -7657, -7632, -7630, -7616, -7610, -7600, -7499, -7484, -7470,
//        -7466, -7463, -7450, -7422, -7390, -7389, -7388, -7365, -7335,
//        -7296, -7294, -7282, -7282, -7270, -7247, -7235, -7210, -7209,
//        -7204, -7154, -7153, -7121, -7120, -7110, -7090, -7067, -7027,
//        -7005, -6986, -6968, -6927, -6888, -6861, -6816, -6806, -6802,
//        -6799, -6757, -6750, -6747, -6730, -6699, -6624, -6564, -6564,
//        -6553, -6537, -6525, -6519, -6488, -6486, -6393, -6369, -6369,
//        -6332, -6307, -6298, -6297, -6279, -6274, -6269, -6268, -6265,
//        -6246, -6236, -6236, -6226, -6225, -6207, -6180, -6141, -6121,
//        -6093, -6071, -6040, -6008, -5998, -5962, -5950, -5856, -5815,
//        -5802, -5798, -5791, -5789, -5776, -5756, -5751, -5729, -5726,
//        -5682, -5682, -5677, -5665, -5659, -5629, -5622, -5615, -5575,
//        -5562, -5542, -5515, -5512, -5493, -5480, -5469, -5462, -5459,
//        -5454, -5430, -5381, -5380, -5353, -5343, -5260, -5256, -5182,
//        -5176, -5173, -5168, -5155, -5122, -5113, -5067, -5061, -5043,
//        -5041, -4894, -4891, -4885, -4874, -4847, -4843, -4836, -4828,
//        -4817, -4787, -4750, -4737, -4720, -4712, -4705, -4701, -4663,
//        -4662, -4654, -4654, -4653, -4649, -4648, -4600, -4591, -4583,
//        -4560, -4558, -4546, -4516, -4508, -4500, -4487, -4476, -4454,
//        -4438, -4411, -4406, -4381, -4359, -4348, -4333, -4295, -4245,
//        -4168, -4111, -4098, -4054, -4050, -4048, -4032, -4029, -4021,
//        -4007, -3990, -3968, -3963, -3952, -3928, -3911, -3902, -3898,
//        -3894, -3883, -3867, -3817, -3760, -3751, -3742, -3648, -3623,
//        -3604, -3589, -3588, -3585, -3515, -3508, -3482, -3476, -3471,
//        -3422, -3417, -3414, -3406, -3398, -3385, -3375, -3366, -3364,
//        -3273, -3241, -3222, -3200, -3142, -3131, -3111, -3111, -3108,
//        -3026, -3025, -3019, -3012, -3010, -2997, -2996, -2984, -2982,
//        -2942, -2919, -2893, -2890, -2768, -2766, -2736, -2679, -2660,
//        -2655, -2650, -2648, -2639, -2542, -2526, -2518, -2501, -2490,
//        -2468, -2461, -2459, -2453, -2450, -2442, -2413, -2411, -2408,
//        -2390, -2383, -2376, -2349, -2336, -2323, -2321, -2320, -2310,
//        -2310, -2302, -2300, -2278, -2260, -2254, -2222, -2212, -2210,
//        -2161, -2119, -2118, -2112, -2075, -2065, -2043, -2042, -2035,
//        -1965, -1954, -1945, -1921, -1917, -1916, -1915, -1911, -1901,
//        -1888, -1843, -1830, -1827, -1820, -1787, -1786, -1784, -1748,
//        -1729, -1690, -1668, -1642, -1601, -1565, -1545, -1494, -1474,
//        -1459, -1441, -1425, -1420, -1406, -1404, -1392, -1388, -1373,
//        -1360, -1355, -1346, -1340, -1286, -1263, -1240, -1238, -1215,
//        -1204, -1189, -1137, -1129, -1101, -1101, -1096, -1067, -1061,
//        -1048, -1007,  -989,  -969,  -950,  -950,  -942,  -935,  -883,
//         -870,  -856,  -792,  -770,  -767,  -731,  -723,  -695,  -693,
//         -693,  -683,  -667,  -647,  -641,  -630,  -613,  -575,  -557,
//         -500,  -431,  -421,  -417,  -408,  -407,  -404,  -404,  -395,
//         -391,  -385,  -381,  -381,  -368,  -365,  -346,  -289,  -218,
//         -213,  -194,  -189,  -180,  -154,  -130,   -96,   -93,   -92,
//          -84,   -78,   -73,   -55,   -14,    25,    42,    53,    54,
//          146,   196,   224,   308,   345,   351,   353,   357,   397,
//          432,   433,   441,   442,   456,   456,   461,   462,   476,
//          483,   491,   541,   562,   564,   584,   586,   593,   598,
//          616,   659,   665,   729,   813,   837,   847,   858,   861,
//          865,   871,   878,   879,   887,   887,   900,   903,   905,
//          934,   941,   972,   979,   992,   999,  1000,  1014,  1096,
//         1100,  1116,  1116,  1121,  1136,  1167,  1179,  1202,  1222,
//         1224,  1245,  1307,  1345,  1352,  1363,  1464,  1475,  1516,
//         1572,  1581,  1627,  1635,  1636,  1643,  1648,  1668,  1681,
//         1732,  1764,  1770,  1780,  1789,  1793,  1814,  1830,  1842,
//         1870,  1896,  1960,  1961,  1963,  1990,  1990,  1992,  2034,
//         2060,  2110,  2113,  2132,  2160,  2166,  2175,  2183,  2194,
//         2204,  2229,  2244,  2248,  2248,  2275,  2280,  2293,  2305,
//         2309,  2326,  2330,  2349,  2357,  2378,  2380,  2392,  2414,
//         2447,  2473,  2473,  2485,  2517,  2566,  2575,  2610,  2611,
//         2614,  2649,  2673,  2682,  2734,  2805,  2809,  2850,  2867,
//         2890,  2912,  2936,  2938,  2953,  2960,  2981,  2987,  3007,
//         3020,  3049,  3056,  3071,  3087,  3093,  3100,  3103,  3132,
//         3149,  3162,  3174,  3208,  3222,  3224,  3312,  3330,  3349,
//         3365,  3426,  3470,  3482,  3489,  3523,  3571,  3583,  3604,
//         3605,  3681,  3695,  3713,  3736,  3741,  3744,  3765,  3786,
//         3821,  3823,  3827,  3844,  3869,  3878,  3891,  3916,  3931,
//         3947,  3974,  4049,  4099,  4107,  4120,  4121,  4136,  4138,
//         4141,  4145,  4163,  4210,  4236,  4251,  4251,  4346,  4348,
//         4390,  4395,  4396,  4403,  4455,  4498,  4526,  4571,  4571,
//         4591,  4595,  4595,  4618,  4648,  4678,  4739,  4754,  4764,
//         4767,  4779,  4840,  4843,  4849,  4850,  4860,  4880,  4881,
//         4916,  4988,  4994,  5008,  5010,  5013,  5018,  5079,  5093,
//         5096,  5100,  5115,  5142,  5229,  5235,  5238,  5254,  5256,
//         5265,  5276,  5320,  5325,  5325,  5377,  5444,  5456,  5460,
//         5473,  5476,  5487,  5500,  5573,  5596,  5609,  5616,  5618,
//         5625,  5643,  5646,  5682,  5700,  5706,  5716,  5719,  5746,
//         5819,  5840,  5859,  5861,  5863,  5871,  5880,  5886,  5904,
//         5907,  5958,  5960,  5985,  6019,  6029,  6153,  6175,  6183,
//         6203,  6238,  6256,  6287,  6335,  6338,  6347,  6393,  6405,
//         6459,  6466,  6472,  6482,  6499,  6582,  6589,  6593,  6631,
//         6634,  6637,  6646,  6658,  6685,  6688,  6702,  6719,  6725,
//         6775,  6784,  6784,  6847,  6852,  6866,  6879,  6923,  6992,
//         7018,  7028,  7061,  7088,  7090,  7091,  7120,  7120,  7136,
//         7147,  7153,  7192,  7214,  7245,  7263,  7264,  7274,  7281,
//         7294,  7342,  7370,  7370,  7405,  7409,  7430,  7436,  7454,
//         7528,  7543,  7565,  7597,  7605,  7616,  7622,  7626,  7627,
//         7713,  7760,  7785,  7803,  7815,  7815,  7820,  7847,  7858,
//         7877,  7914,  7917,  7938,  7941,  7978,  8027,  8073,  8083,
//         8104,  8148,  8158,  8165,  8216,  8221,  8233,  8275,  8294,
//         8308,  8311,  8318,  8331,  8332,  8338,  8392,  8394,  8399,
//         8408,  8410,  8414,  8457,  8457,  8484,  8497,  8531,  8534,
//         8544,  8560,  8563,  8567,  8585,  8715,  8753,  8753,  8784,
//         8792,  8798,  8801,  8802,  8807,  8814,  8819,  8828,  8842,
//         8883,  8916,  8933,  8935,  8946,  8998,  8998,  9014,  9023,
//         9055,  9055,  9070,  9071,  9073,  9077,  9090,  9098,  9138,
//         9167,  9170,  9231,  9234,  9240,  9249,  9249,  9276,  9280,
//         9296,  9303,  9311,  9350,  9367,  9369,  9412,  9442,  9453,
//         9501,  9549,  9557,  9557,  9590,  9621,  9640,  9651,  9655,
//         9661,  9663,  9684,  9727,  9738,  9744,  9756,  9823,  9826,
//         9832,  9835,  9840,  9910,  9923,  9944,  9962]
        
    for (int i = 0; i < 1024; i++) {
        HBM_embedding0[i] = input_array[i];
    }
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
        HBM_embedding20Ext, HBM_embedding21Ext, HBM_embedding22Ext, HBM_embedding23Ext, 
        HBM_embedding24Ext, HBM_embedding25Ext, HBM_embedding26Ext, HBM_embedding27Ext, 
        HBM_embedding28Ext, HBM_embedding29Ext, HBM_embedding30Ext, HBM_embedding31Ext, 
        DDR_embedding0Ext, DDR_embedding1Ext,
        // PLRAM_embedding0Ext, PLRAM_embedding1Ext, PLRAM_embedding2Ext, PLRAM_embedding3Ext, 
        sourcce_hw_resultsExt;
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
    HBM_embedding21Ext.obj = HBM_embedding21.data();
    HBM_embedding21Ext.param = 0;
    HBM_embedding21Ext.flags = bank[21];
    HBM_embedding22Ext.obj = HBM_embedding22.data();
    HBM_embedding22Ext.param = 0;
    HBM_embedding22Ext.flags = bank[22];
    HBM_embedding23Ext.obj = HBM_embedding23.data();
    HBM_embedding23Ext.param = 0;
    HBM_embedding23Ext.flags = bank[23];
    HBM_embedding24Ext.obj = HBM_embedding24.data();
    HBM_embedding24Ext.param = 0;
    HBM_embedding24Ext.flags = bank[24];
    HBM_embedding25Ext.obj = HBM_embedding25.data();
    HBM_embedding25Ext.param = 0;
    HBM_embedding25Ext.flags = bank[25];
    HBM_embedding26Ext.obj = HBM_embedding26.data();
    HBM_embedding26Ext.param = 0;
    HBM_embedding26Ext.flags = bank[26];
    HBM_embedding27Ext.obj = HBM_embedding27.data();
    HBM_embedding27Ext.param = 0;
    HBM_embedding27Ext.flags = bank[27];
    HBM_embedding28Ext.obj = HBM_embedding28.data();
    HBM_embedding28Ext.param = 0;
    HBM_embedding28Ext.flags = bank[28];
    HBM_embedding29Ext.obj = HBM_embedding29.data();
    HBM_embedding29Ext.param = 0;
    HBM_embedding29Ext.flags = bank[29];
    HBM_embedding30Ext.obj = HBM_embedding30.data();
    HBM_embedding30Ext.param = 0;
    HBM_embedding30Ext.flags = bank[30];
    HBM_embedding31Ext.obj = HBM_embedding31.data();
    HBM_embedding31Ext.param = 0;
    HBM_embedding31Ext.flags = bank[31];

    DDR_embedding0Ext.obj = DDR_embedding0.data();
    DDR_embedding0Ext.param = 0;
    DDR_embedding0Ext.flags = bank[0 + 32];
    DDR_embedding1Ext.obj = DDR_embedding1.data();
    DDR_embedding1Ext.param = 0;
    DDR_embedding1Ext.flags = bank[1 + 32];

    // PLRAM
    sourcce_hw_resultsExt.obj = source_hw_results.data();
    sourcce_hw_resultsExt.param = 0;
    sourcce_hw_resultsExt.flags = bank[34];

    // CL_MEM_EXT_PTR_XILINX
//////////////////////////////   TEMPLATE START  //////////////////////////////
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding0(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding0_size *sizeof(D_TYPE), &HBM_embedding0Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding1_size *sizeof(D_TYPE), &HBM_embedding1Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding2_size *sizeof(D_TYPE), &HBM_embedding2Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding3(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding3_size *sizeof(D_TYPE), &HBM_embedding3Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding4(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding4_size *sizeof(D_TYPE), &HBM_embedding4Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding5(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding5_size *sizeof(D_TYPE), &HBM_embedding5Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding6(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding6_size *sizeof(D_TYPE), &HBM_embedding6Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding7(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding7_size *sizeof(D_TYPE), &HBM_embedding7Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding8(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding8_size *sizeof(D_TYPE), &HBM_embedding8Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding9(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding9_size *sizeof(D_TYPE), &HBM_embedding9Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding10(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding10_size *sizeof(D_TYPE), &HBM_embedding10Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding11(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding11_size *sizeof(D_TYPE), &HBM_embedding11Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding12(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding12_size *sizeof(D_TYPE), &HBM_embedding12Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding13(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding13_size *sizeof(D_TYPE), &HBM_embedding13Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding14(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding14_size *sizeof(D_TYPE), &HBM_embedding14Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding15(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding15_size *sizeof(D_TYPE), &HBM_embedding15Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding16(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding16_size *sizeof(D_TYPE), &HBM_embedding16Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding17(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding17_size *sizeof(D_TYPE), &HBM_embedding17Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding18(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding18_size *sizeof(D_TYPE), &HBM_embedding18Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding19(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding19_size *sizeof(D_TYPE), &HBM_embedding19Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding20(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding20_size *sizeof(D_TYPE), &HBM_embedding20Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding21(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding21_size *sizeof(D_TYPE), &HBM_embedding21Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding22(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding22_size *sizeof(D_TYPE), &HBM_embedding22Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding23(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding23_size *sizeof(D_TYPE), &HBM_embedding23Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding24(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding24_size *sizeof(D_TYPE), &HBM_embedding24Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding25(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding25_size *sizeof(D_TYPE), &HBM_embedding25Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding26(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding26_size *sizeof(D_TYPE), &HBM_embedding26Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding27(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding27_size *sizeof(D_TYPE), &HBM_embedding27Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding28(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding28_size *sizeof(D_TYPE), &HBM_embedding28Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding29(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding29_size *sizeof(D_TYPE), &HBM_embedding29Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding30(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding30_size *sizeof(D_TYPE), &HBM_embedding30Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_HBM_embedding31(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            HBM_embedding31_size *sizeof(D_TYPE), &HBM_embedding31Ext, &err));

    OCL_CHECK(err, cl::Buffer buffer_DDR_embedding0(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            DDR_embedding0_size *sizeof(D_TYPE), &DDR_embedding0Ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_DDR_embedding1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
            DDR_embedding1_size *sizeof(D_TYPE), &DDR_embedding1Ext, &err));

// .......................................................
// Allocate Global Memory for sourcce_hw_results
// .......................................................
    OCL_CHECK(err, cl::Buffer buffer_output(
        context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX, 
        size_results_out *sizeof(ap_uint<64>), &sourcce_hw_resultsExt, &err));

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
    OCL_CHECK(err, err = krnl_vector_add.setArg(21, buffer_HBM_embedding21));
    OCL_CHECK(err, err = krnl_vector_add.setArg(22, buffer_HBM_embedding22));
    OCL_CHECK(err, err = krnl_vector_add.setArg(23, buffer_HBM_embedding23));
    OCL_CHECK(err, err = krnl_vector_add.setArg(24, buffer_HBM_embedding24));
    OCL_CHECK(err, err = krnl_vector_add.setArg(25, buffer_HBM_embedding25));
    OCL_CHECK(err, err = krnl_vector_add.setArg(26, buffer_HBM_embedding26));
    OCL_CHECK(err, err = krnl_vector_add.setArg(27, buffer_HBM_embedding27));
    OCL_CHECK(err, err = krnl_vector_add.setArg(28, buffer_HBM_embedding28));
    OCL_CHECK(err, err = krnl_vector_add.setArg(29, buffer_HBM_embedding29));
    OCL_CHECK(err, err = krnl_vector_add.setArg(30, buffer_HBM_embedding30));
    OCL_CHECK(err, err = krnl_vector_add.setArg(31, buffer_HBM_embedding31));

    OCL_CHECK(err, err = krnl_vector_add.setArg(0 + 32, buffer_DDR_embedding0));
    OCL_CHECK(err, err = krnl_vector_add.setArg(1 + 32, buffer_DDR_embedding1));

    OCL_CHECK(err, err = krnl_vector_add.setArg(34, buffer_output));
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
        buffer_HBM_embedding20, buffer_HBM_embedding21, buffer_HBM_embedding22, buffer_HBM_embedding23, 
        buffer_HBM_embedding24, buffer_HBM_embedding25, buffer_HBM_embedding26, buffer_HBM_embedding27, 
        buffer_HBM_embedding28, buffer_HBM_embedding29, buffer_HBM_embedding30, buffer_HBM_embedding31, 
        buffer_DDR_embedding0, buffer_DDR_embedding1}, 0/* 0 means from host*/));	
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

    for (int i = 0 ; i < PRIORITY_QUEUE_LEN; i++) {
        //     single_PQ_result reg; 
            ap_uint<32> raw_vec_ID = source_hw_results[i].range(31, 0);
            ap_uint<32> raw_dist = source_hw_results[i].range(63, 32);
            int vec_ID = *((int*) (&raw_vec_ID));
            float dist = *((float*) (&raw_dist));
            std::cout << "i = " << i << " vec_ID = " << vec_ID << " distance = " << dist << std::endl;
    }

// ============================================================================
// Step 3: Release Allocated Resources
// ============================================================================
    delete[] fileBuf;

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}

