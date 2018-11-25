#pragma once

#include "bloom_common.hh"

class BloomNoPacking {

uint64_t bloom [BLOOM_SIZE];

public:

void bloom_insert(const void* key, size_t key_len){
    uint64_t hashValue[2]; // equivalent to HHResult128 in Highway hash
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
    for (unsigned i=1; i<=HASH_NUM; i++){
        unsigned ind = (hashValue[0] + i * hashValue[1]) % BLOOM_SIZE;
        bloom[ind] = 1;
    }   
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
    for (unsigned i=1; i<=HASH_NUM; i++){
        unsigned ind = (hashValue[0] + i * hashValue[1]) % BLOOM_SIZE;
        if (bloom[ind] == 0){
            #if VALIDATE
                *hashVal = new uint64_t[2];
                memcpy(*hashVal, hashValue, 2 * sizeof(uint64_t));
            #endif
            return false;
        }
    }
    STOP_COUNTING_BLOOM("bloom contains, get bits")
    return true;
}

bool bloom_contains_hash(uint64_t* hashVal){
    INIT_COUNTING_BLOOM
    START_COUNTING_BLOOM
    for (unsigned i=1; i<=HASH_NUM; i++){
        unsigned ind = (hashVal[0] + i * hashVal[1]) % BLOOM_SIZE;
        if (bloom[ind] == 0){ 
            return false;
        }   
    }   
    STOP_COUNTING_BLOOM("bloom contains, get bits")
    return true;
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

