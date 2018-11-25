#pragma once

#include <atomic>

#include "bloom_common.hh"


class BloomPacking{
std::atomic<uint64_t> bloom [BLOOM_SIZE];// __attribute__((aligned(64)));

public:

void bloom_insert(const void* key, size_t key_len){
    uint64_t hashValue[2];
    INIT_COUNTING_BLOOM
    START_COUNTING_BLOOM
    #if HASH_METHOD == 1
    MurmurHash3_x64_128((uint8_t*)key, key_len, 0, hashValue);
    #elif HASH_METHOD == 2
    HHStateT<HH_TARGET> state(highway_key);
    HighwayHashT(&state, (const char*)key, key_len, &hashValue);
    #endif
    STOP_COUNTING_BLOOM("bloom insert, hash")
    START_COUNTING_BLOOM
    unsigned ind = (hashValue[0] + hashValue[1]) % BLOOM_SIZE;
    uint64_t tmp = 0;
    tmp = tmp | (1 << (ind%64));
    for(unsigned i=2; i<=HASH_NUM; i++){
        unsigned bit_ind = (hashValue[0] + i * hashValue[1]) % 64; 
        tmp = tmp | (1 << bit_ind);
    }   
    uint64_t to_set = bloom[ind] | tmp;
    uint64_t contents_got = bloom[ind];
    while(! bloom[ind].compare_exchange_weak(contents_got, to_set)){
        to_set = contents_got | tmp;
    }   
    //bloom[ind] = bloom[ind] | tmp;
    STOP_COUNTING_BLOOM("bloom insert, set bits")
}

bool bloom_contains(const void* key, size_t key_len, uint64_t** hashVal){
    uint64_t hashValue[2];
    INIT_COUNTING_BLOOM
    START_COUNTING_BLOOM
    #if HASH_METHOD == 1
    MurmurHash3_x64_128((uint8_t*)key, key_len, 0, hashValue);
    #elif HASH_METHOD == 2
    HHStateT<HH_TARGET> state(highway_key);
    HighwayHashT(&state, (const char*)key, key_len, &hashValue);
    #endif
    STOP_COUNTING_BLOOM("bloom contains, hash")
    START_COUNTING_BLOOM
    unsigned ind = (hashValue[0] + hashValue[1]) % BLOOM_SIZE;
    uint64_t tmp = 0;
    tmp = tmp | (1 << (ind%64));
    for (unsigned i=2; i<=HASH_NUM; i++){
        unsigned bit_ind = (hashValue[0] + i * hashValue[1]) % 64;
        tmp = tmp | (1 << bit_ind);
    }
    uint64_t existing = bloom[ind];
    if(tmp == (tmp & existing)){ // the bits set for that key are also set in the target bloom cell
        STOP_COUNTING_BLOOM("bloom contains, get bits")
        return true;
    }
    STOP_COUNTING_BLOOM("bloom contains, get bits")
    #if VALIDATE
        *hashVal = new uint64_t[2];
        memcpy(*hashVal, hashValue, 2 * sizeof(uint64_t));
    #endif
    return false;
}


bool bloom_contains_hash(uint64_t* hashVal){
    INIT_COUNTING_BLOOM
    START_COUNTING_BLOOM
    unsigned ind = (hashVal[0] + hashVal[1]) % BLOOM_SIZE;
    uint64_t tmp = 0;
    tmp = tmp | (1 << (ind%64));
    for (unsigned i=2; i<=HASH_NUM; i++){
        unsigned bit_ind = (hashVal[0] + i * hashVal[1]) % 64; 
        tmp = tmp | (1 << bit_ind);
    }   
    uint64_t existing = bloom[ind];
    if(tmp == (tmp & existing)){ // the bits set for that key are also set in the target bloom cell
        STOP_COUNTING_BLOOM("bloom contains, get bits")
        return true;
    }   
    STOP_COUNTING_BLOOM("bloom contains, get bits")
    return false;
}

void inspect_bloom(){
    uint64_t zeros=0, ones=0;
    for(uint64_t i=0; i<BLOOM_SIZE; i++){
        if(bloom[i]==1)
            ones++;
        else
            zeros++;
    }   
    cout <<"Bloom size: "<<BLOOM_SIZE<<endl;
    cout <<"ones: "<<ones<<endl;
    cout<<"zeros: "<<zeros<<endl;
}


};

