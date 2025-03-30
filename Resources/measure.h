// OS 2025 EX1

#ifndef MEASURE_H
#define MEASURE_H

#include "memory_latency.h"


/**
 * Measures the average latency of accessing a given array.
 * @param repeat - the number of times to repeat the measurement for and average on.
 * @param arr - an allocated (not empty) array to preform measurement on.
 * @param arr_size - the length of the array arr.
 * @param zero - a variable containing zero in a way that the compiler doesn't "know" it in compilation time.
 * @return struct measurement containing the measurement with the following fields:
 *      double baseline - the average time (ns) taken to preform the measured operation without memory access.
 *      double access_time - the average time (ns) taken to preform the measured operation with memory access.
 *      uint64_t rnd - the variable used to randomly access the array, returned to prevent compiler optimizations.
 */
struct measurement measure_latency(uint64_t repeat, array_element_t* arr, uint64_t arr_size, uint64_t zero);

#endif
