#include<cstdint>
#include<iostream>
using namespace std;

//WARNING DO NOT USE THIS CODE IT IS BROKEN!!!!

//https://crypto.stackexchange.com/questions/15864/pseudocode-for-constant-time-modular-exponentiation
//https://en.wikipedia.org/wiki/Exponentiation_by_squaring#Montomerys_ladder_technique


typedef uint64_t uintM;
typedef uint32_t uintMhalf;
static_assert(sizeof(size_t) == sizeof(uintM),"This code must be compiled for an architecture which supports a word size of exactly uintM size");

//if the underlying machine word size is 32, you can't safely use
//uint64 as the higher level size because the 32 bit implementation of 64 code could have branches.
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
		selectormask >>= sizeof(uintMhalf)*8;
		register uintM lerp=x1 & selectormask | x2 & (~selectormask); //lerp is b ? x1 : x2, but branchless
		x1=(x1*lerp) % m;		
		x2=(x2*lerp) % m;
	}
	return x1;
}

#include<random>


int main()
{
	std::mt19937_64 rnum(4); //seed random numbers
	
	for(int i=0;i<5000;i++)
	{
		uintM a=rnum();
		uintM b=rnum();
		uintMhalf m=rnum() >> (sizeof(uintMhalf)*8);
		cout << a << "^" << b << "%" << m << "=" << powm_unsafe(a,b,m) << ":" << powm(a,b,m) << "\n";
	}
	return 0;
}
