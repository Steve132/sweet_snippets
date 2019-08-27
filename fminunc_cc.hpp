/*MIT License

Copyright (c) 2019 Steven Braeger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/


#ifndef FMINUNC_CC_HPP
#define FMINUNC_CC_HPP

#include<array>
#include<cstdint>
#include<type_traits>

template<size_t N,class Comparable>
struct result
{
	std::array<size_t,N> index;
	Comparable value;
};


template<size_t N,class Func,class Comparable,size_t dindex>
struct impl_t
{
    static inline void implfunc(Func f,const std::array<size_t,N>& maxbounds,result<N,Comparable>& minresult)
    {
        size_t M=maxbounds[dindex];
        minresult.index[dindex]=0;
        impl_t<N,Func,Comparable,dindex+1>::implfunc(f,maxbounds,minresult);
        size_t mindex=0;
        Comparable cv=minresult.value;
        
        for(size_t i=1;i<M;i++)
        {
            minresult.index[dindex]=i;
            impl_t<N,Func,Comparable,dindex+1>::implfunc(f,maxbounds,minresult);
            
            if(minresult.value >= cv)
            {
                minresult.index[dindex]=mindex;
                minresult.value=cv;
            }
            else
            {
                mindex=i;
                cv=minresult.value;
            }
        }
        minresult.value=cv;
        minresult.index[dindex]=mindex;
    }
};

template<size_t N,class Func,class Comparable>
struct impl_t<N,Func,Comparable,N>
{
    static inline void implfunc(Func f,const std::array<size_t,N>& maxbounds,result<N,Comparable>& minresult)
    {
        minresult.value=f(minresult.index);
    }
};


template<size_t N,class Func>
inline result<N,std::invoke_result_t<Func,const std::array<size_t,N>&>> fargmini(Func f,const std::array<size_t,N>& maxbounds)
{
    result<N,std::invoke_result_t<Func,const std::array<size_t,N>&>> minresult;
    impl_t<N,Func,std::invoke_result_t<Func,const std::array<size_t,N>&>,0>::implfunc(f,maxbounds,minresult);
    return minresult;
}

#endif
