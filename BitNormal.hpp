
#include<cstdlib>
#include<cstdint>
#include<array>
#include<cmath>

#include<iostream>
#include<iomanip>
#include<bit>
#include<random>
#include<cstdint>
#include<map>


uint32_t trial_singlebits(size_t N){
    uint32_t S=0;
    for(size_t i=0;i<N;i++){
        uint32_t x=rand();
        if(x & 0x1){
            S++;
        }
    }
    return S;
}

static const inline uint64_t magic2(uint64_t seed){
    static constexpr uint64_t p1=0xc4ceb9fe1a85ec53;
    
}

static const inline uint64_t murmur64(uint64_t h){
    h ^= std::rotr(h,33);
    h *= 0xff51afd7ed558ccdL;
    h ^= std::rotr(h,33);
    h *= 0xc4ceb9fe1a85ec53L;
    //h ^= std::rotr(h >>> 33;
    return h;
}


uint32_t trial_mulhash(size_t N){
    size_t iTers=N/(sizeof(uint64_t)*8);
    uint64_t seed=0;
    for(int i=0;i<5;i++){
        seed<<=14;
        seed^=rand();
    }

    auto next=murmur64;
    uint32_t count=0;

    for(size_t i=0;i<iTers;i++){
        count+=std::popcount(seed);    
        seed=next(seed);
    }
    return count;
}

int main(){

    uint32_t x=0;
    uint32_t S=0;

    std::map<uint32_t,size_t> results;
    uint32_t T=10000;
    uint32_t N=64*4;
    for(size_t i=0;i<T;i++){
        uint32_t r=trial_mulhash(N);
        results.emplace(r,0).first->second++;
    }

    for(const auto& f : results){
        std::cout << f.first << ":" << f.second << std::endl;
    }
    
    //run_stretch_test(0xF320419);
    return 0;
}
