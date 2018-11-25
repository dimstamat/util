#pragma once
#include <iostream>
#include <sstream>


#define MEASURE_LATENCIES 0
#define MEASURE_LATENCIES_BLOOM 0

#if MEASURE_LATENCIES
    #define INIT_COUNTING struct timespec start_time, end_time;
    #define START_COUNTING clock_gettime(CLOCK_MONOTONIC, &start_time);
    #define STOP_COUNTING_PRINT(msg) { clock_gettime(CLOCK_MONOTONIC, &end_time); \
                                double duration_ns = (end_time.tv_sec > start_time.tv_sec ? (1e9-start_time.tv_nsec + end_time.tv_nsec /*+ (end_time.tv_sec - start_time.tv_sec -1)*1e9*/ ) : end_time.tv_nsec - start_time.tv_nsec) ;\
                                /*double duration_us = (end_time.tv_nsec - start_time.tv_nsec)/1000 + (end_time.tv_sec - start_time.tv_sec)*1e6;*/\
                                stringstream ss;\
                                ss << msg << ": "<< duration_ns <<endl; \
                            cout <<ss.str(); }
#else
    #define INIT_COUNTING {}
    #define START_COUNTING {}
    #define STOP_COUNTING_PRINT(msg) {}
#endif




#if MEASURE_LATENCIES_BLOOM
    #define INIT_COUNTING_BLOOM struct timespec start_time, end_time;
    #define START_COUNTING_BLOOM clock_gettime(CLOCK_MONOTONIC, &start_time);
    #define STOP_COUNTING_BLOOM(msg) { clock_gettime(CLOCK_MONOTONIC, &end_time); \
                                double duration_ns = (end_time.tv_sec > start_time.tv_sec ? (1e9-start_time.tv_nsec + end_time.tv_nsec /*+ (end_time.tv_sec - start_time.tv_sec -1)*1e9*/ ) : end_time.tv_nsec - start_time.tv_nsec) ;\
                                /*double duration_us = (end_time.tv_nsec - start_time.tv_nsec)/1000 + (end_time.tv_sec - start_time.tv_sec)*1e6;*/\
                                stringstream ss;\
                                ss << msg << ": "<< duration_ns <<endl; \
                            cout <<ss.str(); }
#else
    #define INIT_COUNTING_BLOOM {}
    #define START_COUNTING_BLOOM {}
    #define STOP_COUNTING_BLOOM(msg) {}
#endif
