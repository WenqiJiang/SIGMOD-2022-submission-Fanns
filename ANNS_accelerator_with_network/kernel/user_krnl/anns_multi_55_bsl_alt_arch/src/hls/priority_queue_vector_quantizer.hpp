#pragma once 

#include "constants.hpp"
#include "types.hpp"

template<typename T, const int queue_size, Order order> 
class Priority_queue;

template<const int queue_size> 
class Priority_queue<dist_cell_ID_t, queue_size, Collect_smallest> {

    public: 

        Priority_queue() {
#pragma HLS inline
        }

        // For vector quantizer, each query outputs a certain number of distances
        //  = NLIST
        template<const int query_num, const int read_iter_per_query>
        void insert_wrapper(
            hls::stream<dist_cell_ID_t> &s_input, 
            hls::stream<dist_cell_ID_t> &s_output) {
            
            dist_cell_ID_t queue[queue_size];
#pragma HLS array_partition variable=queue complete

            for (int query_id = 0; query_id < query_num; query_id++) {

                // init
                for (int i = 0; i < queue_size; i++) {
#pragma HLS UNROLL
                    queue[i].cell_ID = -1;
                    queue[i].dist = LARGE_NUM;
                }

                // insert: 
                for (int i = 0; i < read_iter_per_query; i++) {
#pragma HLS pipeline II=1
                    dist_cell_ID_t reg = s_input.read();
                    queue[0] = queue[0].dist < reg.dist? queue[0] : reg;

                    compare_swap_array_step_A(queue);

                    compare_swap_array_step_B(queue);
                }

                // write
                for (int i = 0; i < queue_size; i++) {
#pragma HLS pipeline II=1
                    s_output.write(queue[i]);
                }
            }
        }

    private:
    
        void compare_swap(dist_cell_ID_t* array, int idxA, int idxB) {
            // if smaller -> swap to right
            // note: idxA must < idxB
#pragma HLS inline
            if (array[idxA].dist < array[idxB].dist) {
                dist_cell_ID_t regA = array[idxA];
                dist_cell_ID_t regB = array[idxB];
                array[idxA] = regB;
                array[idxB] = regA;
            }
        }

        void compare_swap_array_step_A(dist_cell_ID_t* array) {
            // start from idx 0, odd-even swap
#pragma HLS inline
            for (int j = 0; j < queue_size / 2; j++) {
#pragma HLS UNROLL
                compare_swap(array, 2 * j, 2 * j + 1);
            }
        }
                    
        void compare_swap_array_step_B(dist_cell_ID_t* array) {
            // start from idx 1, odd-even swap
#pragma HLS inline
            for (int j = 0; j < (queue_size - 1) / 2; j++) {
#pragma HLS UNROLL
                compare_swap(array, 2 * j + 1, 2 * j + 2);
            }
        }
};