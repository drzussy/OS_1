// OS 2025 EX1

#include <iostream>
#include "memory_latency.h"
#include "measure.h"

#define GALOIS_POLYNOMIAL ((1ULL << 63) | (1ULL << 62) | (1ULL << 60) | (1ULL << 59))

/**
 * Converts the struct timespec to time in nano-seconds.
 * @param t - the struct timespec to convert.
 * @return - the value of time in nano-seconds.
 */
uint64_t nanosectime(struct timespec t)
{
    return (uint64_t)(t.tv_sec * 1000000000ULL + t.tv_nsec);
}

/**
* Measures the average latency of accessing a given array in a sequential order.
* @param repeat - the number of times to repeat the measurement for and average on.
* @param arr - an allocated (not empty) array to preform measurement on.
* @param arr_size - the length of the array arr.
* @param zero - a variable containing zero in a way that the compiler doesn't "know" it in compilation time.
* @return struct measurement containing the measurement with the following fields:
*      double baseline - the average time (ns) taken to preform the measured operation without memory access.
*      double access_time - the average time (ns) taken to preform the measured operation with memory access.
*      uint64_t rnd - the variable used to randomly access the array, returned to prevent compiler optimizations.
*/
struct measurement measure_sequential_latency(uint64_t repeat, array_element_t* arr, uint64_t arr_size, uint64_t zero)
{
    repeat = arr_size > repeat ? arr_size:repeat; // Make sure repeat >= arr_size

    // Baseline measurement:
    struct timespec t0;
    timespec_get(&t0, TIME_UTC);
    register uint64_t rnd=12345;
    for (register uint64_t i = 0; i < repeat; i++)
    {
        register uint64_t index = i%arr_size;//this was changed!
        rnd ^= index & zero; //??????
        rnd = (rnd >> 1) ^ ((0-(rnd & 1)) & GALOIS_POLYNOMIAL);  // Advance rnd pseudo-randomly (using Galois LFSR)
    }
    struct timespec t1;
    timespec_get(&t1, TIME_UTC);

    // Memory access measurement:
    struct timespec t2;
    timespec_get(&t2, TIME_UTC);
    rnd=(rnd & zero) ^ 12345;
    for (register uint64_t i = 0; i < repeat; i++)
    {
        register uint64_t index = i%arr_size; //this was changed!
        rnd ^= arr[index] & zero;
        rnd = (rnd >> 1) ^ ((0-(rnd & 1)) & GALOIS_POLYNOMIAL);  // Advance rnd pseudo-randomly (using Galois LFSR)
    }
    struct timespec t3;
    timespec_get(&t3, TIME_UTC);

    // Calculate baseline and memory access times:
    double baseline_per_cycle=(double)(nanosectime(t1)- nanosectime(t0))/(repeat);
    double memory_per_cycle=(double)(nanosectime(t3)- nanosectime(t2))/(repeat);
    struct measurement result;

    result.baseline = baseline_per_cycle;
    result.access_time = memory_per_cycle;
    result.rnd = rnd;
    return result;
}

/**
 * Runs the logic of the memory_latency program. Measures the access latency for random and sequential memory access
 * patterns.
 * Usage: './memory_latency max_size factor repeat' where:
 *      - max_size - the maximum size in bytes of the array to measure access latency for.
 *      - factor - the factor in the geometric series representing the array sizes to check.
 *      - repeat - the number of times each measurement should be repeated for and averaged on.
 * The program will print output to stdout in the following format:
 *      mem_size_1,offset_1,offset_sequential_1
 *      mem_size_2,offset_2,offset_sequential_2
 *              ...
 *              ...
 *              ...
 */
int main(int argc, char* argv[]) {
    // zero==0, but the compiler doesn't know it. Use as the zero arg of measure_latency and measure_sequential_latency.
    struct timespec t_dummy;
    timespec_get(&t_dummy, TIME_UTC);
    const uint64_t zero = nanosectime(t_dummy) > 1000000000ull ? 0 : nanosectime(t_dummy);

    // Check if 4 arguments where given:
    if (argc != 4) {
        std::cerr << "Usage: ./memory_latency max_size factor repeat" << std::endl;
        return EXIT_FAILURE;
    }

    // check max_size >= 100
    char* endptr; 
    uint64_t max_size = strtoull(argv[1], &endptr, 10);
    if (*endptr != '\0' || max_size < 100) {
        std::cerr << "Invalid max_size: should be max_size >= 10." << std::endl;
        return EXIT_FAILURE;
    }

    // check factor > 1
    double factor = strtod(argv[2], &endptr);
    if (*endptr != '\0' || factor <= 1.0) {
        std::cerr << "Invalid factor: should be factor > 1." << std::endl;
        return EXIT_FAILURE;
    }

    // check repeat > 0
    uint64_t repeat = strtoull(argv[3], &endptr, 10);
    if (*endptr != '\0' || repeat == 0) {
        std::cerr << "Invalid repeat: should be repeat > 0." << std::endl;
        return EXIT_FAILURE;
    }

    for (uint64_t size = 100; size < max_size; size *= factor) {
        auto *arr = (array_element_t *) malloc(size * sizeof(array_element_t));
        if (arr == nullptr) {
            std::cout << "Memory allocation failed for size " << size << std::endl;
            return EXIT_FAILURE;
        }
        uint64_t times[4] = {0};
        for (uint64_t j = 0; j < repeat; j++) {
            //random latency check
            struct measurement random_measurement = measure_latency(repeat, arr, size, zero);
            //sequential latency check
            struct measurement sequential_measurement = measure_sequential_latency(repeat, arr, size, zero);
            //add to sums
            times[0] += random_measurement.baseline;
            times[1] += random_measurement.access_time;
            times[2] += sequential_measurement.baseline;
            times[3] += sequential_measurement.access_time;
        }
        //calculate averages
        for(int i=0;i<4;i++){
            times[i] /= repeat;
        }
        //print out mem_size1(bytes),offset1(random access latency, ns),offset1(sequential access latency, ns)
        std::cout << size << ","
                  << times[1]-times[0] << "," // Random access latency
                  << times[3]-times[2] << std::endl; // Sequential access latency
        free(arr);
}
