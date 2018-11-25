#pragma once

#include <iostream>
#include <chrono>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <getopt.h>

#include <atomic>

#include "measure_latencies.hh"


// #define BLOOM_SIZE 1 << 13;
//#define BLOOM_SIZE 8192           // 2 ^ 13
//#define BLOOM_SIZE 131072     // 2 ^ 17
//#define BLOOM_SIZE 1048576  // 2 ^ 20
//#define BLOOM_SIZE 2097152   // 2 ^ 21
//#define BLOOM_SIZE 4194304      // 2 ^ 22
// ** our standard is 2^23
#define BLOOM_SIZE 8388608    // 2 ^ 23
//#define BLOOM_SIZE 67108864   // 2 ^ 26

#define HASH_NUM 3

#define HASH_METHOD 2   // 1 for MurmurHash, 2 for HighwayHash
#define VALIDATE 1      // Whether to perform bloom filter validation when an element is absent from the BF

#if HASH_METHOD == 1
#include "MurmurHash3.h"
#elif HASH_METHOD == 2
#include "highwayhash/highwayhash.h"
using namespace highwayhash;
constexpr HHKey highway_key HH_ALIGNAS(32) = {1, 2, 3, 4}; 
#endif


string keyToStr(const char* key, unsigned len) {
    string res="";
    for(unsigned i=0; i<len; i++){
        res += (char)key[i];
    }
    return res;
}


