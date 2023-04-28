//https://probablydance.com/2023/04/27/beautiful-branchless-binary-search/

#include<cstdlib>
#include<cstdint>
#include<bit>
#include<algorithm>
#include<functional>

template<typename It, typename T, typename Cmp>
It branchless_lower_bound(It begin, It end, const T & value, Cmp && compare)
{
    size_t length = end - begin;
    if (length == 0)
        return end;
    size_t step =std::bit_floor(length);
    if (step != length && compare(begin[step], value))
    {
        length -= step + 1;
        if (length == 0)
            return end;
        step = std::bit_ceil(length);
        begin = end - step;
    }
    for (step /= 2; step != 0; step /= 2)
    {
        if (compare(begin[step], value))
            begin += step;
    }
    return begin + compare(*begin, value);
}


#define CMAC(x) case (x): {if(compare(begin[1ULL << (x)], value)) begin+= 1ULL << (x);}

#define CMC8(y) \
    CMAC(8*y+7) \
    CMAC(8*y+6) \
    CMAC(8*y+5) \
    CMAC(8*y+4) \
    CMAC(8*y+3) \
    CMAC(8*y+2) \
    CMAC(8*y+1) \
    CMAC(8*y+0) 

template<typename It, typename T, typename Cmp>
It branchless_lower_bound2(It begin, It end, const T & value, Cmp && compare)
{
    size_t length = end - begin;
    if (length == 0)
        return end;
    size_t step =std::bit_floor(length);
    if (step != length && compare(begin[step], value))
    {
        length -= step + 1;
        if (length == 0)
            return end;
        step = std::bit_ceil(length);
        begin = end - step;
    }
    unsigned int istep=std::countr_zero(step)-1;
    
    switch(istep){
        CMC8(7)
        CMC8(6)
        CMC8(5)
        CMC8(4)
        CMC8(3)
        CMC8(2)
        CMC8(1)
        CMC8(0)
    };

    return begin + compare(*begin, value);
}


template<typename It, typename T>
It branchless_lower_bound(It begin, It end, const T & value)
{
    return branchless_lower_bound2(begin, end, value, std::less<>{});
}

int* lb_test(int* a,int N,int b){
    return branchless_lower_bound(a,a+N,b);
}
