#include<cstdint>
#include<iostream>
using namespace std;

//https://crypto.stackexchange.com/questions/15864/pseudocode-for-constant-time-modular-exponentiation
//https://en.wikipedia.org/wiki/Exponentiation_by_squaring#Montomerys_ladder_technique


typedef uintM uint64_t;
typedef uintMhalf uint32_t;

#if sizeof(size_t) < sizeof(uintM)
//if the underlying machine word size is 32, you can't safely use
//uint64 as the higher level size because the 32 bit implementation of 64 code could have branches.
#error "This code must be compiled for an architecture which supports a word size of exactly uintM size"
#endif

uintM powm_unsafe(uintM x,uintM n,uintMhalf m_in)
{
	//in order to deal with x1*x2 correctly producing the right result that can be stored in one word size,
	//m has to be half the size
	uintM m=m_in;
	uintM x1=x % m;
	uintM x2=(x*x) % m;
	for(unsigned int i=0;i<(sizeof(uintM)*8);i++)
	{
		if((n >> i) & 1)
		{
			x1=(x1*x1) % m;
			x2=(x1*x2) % m;
		}
		else
		{
			x1=(x1*x2) % m;
			x2=(x2*x2) % m;
		}
	}
	return x1;
}

uintM powm(uintM x,uintM n,uintMhalf m_in)
{
	//in order to deal with x1*x2 correctly producing the right result that can be stored in one word size,
	//m has to be half the size
	uintM m=m_in;
	register uintM x1=x % m;
	register uintM x2=(x*x) % m;
	for(unsigned int i=0;i<(sizeof(uintM)*8);i++)
	{
		uintM selectormask=uintM(0)-static_cast<uintM>((n >> i) & 1);//create a boolean all-ones mask based on the result of the test.
		register uintM lerp=x1 & selectormask | x2 & ~selectormask; //lerp is b ? x1 : x2, but branchless
		x1=(x1*lerp) % m;		
		x2=(x2*lerp) % m;
	}
	return x1;
}
