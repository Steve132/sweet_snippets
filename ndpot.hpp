//CACHE Oblivious ND-Array.  Side must be a power-of-two

#ifndef NDPOT_HPP
#define NDPOT_HPP

#include<array>
#include<vector>

template<class MASKTYPE,unsigned DIMS>
struct pdepextmask
{
	static const MASKTYPE value;
};

static uint32_t _pdep(uint32_t v,uint32_t mask);
static uint64_t _pdep(uint64_t v,uint64_t mask);
static uint32_t _pext(uint32_t v,uint32_t mask);
static uint64_t _pext(uint64_t v,uint64_t mask);

template<class T,unsigned D>
class ndpot
{
protected:
	size_t side_length;
	size_t log_side_length;
	static inline size_t prod_l(size_t len)
	{
		size_t p=1;
		for(unsigned d=0;d<D;d++) p*=len;
		return p;
	}
public:
	typedef std::array<size_t,D> index;
	typedef size_t linear_index;
	
	std::vector<T> data;
	
	ndpot(size_t len=0,const T& init=T()):
		side_length(len),
		log_side_length(0),
		data(prod_l(len),init)
	{
		for(log_side_length=0;(size_t(1) << log_side_length) <side_length; log_side_length++);
	}
	size_t log_length() const
	{
		return log_side_length;
	}
	size_t length() const
	{
		return side_length;
	}

	linear_index linearize(const index& i)
	{
		size_t msk=side_length-1;
		size_t lcoord=0;
		for(unsigned d=0;d<D;d++)
		{
			lcoord |= (i[d] & msk) << (d*log_side_length);
		}
		return lcoord;
	}
	index unlinearize(const linear_index& s)
	{
		size_t msk=side_length-1;
		index ulcoord;
		for(unsigned d=0;d<D;d++)
		{
			ulcoord[d]=(s >> (d*log_side_length)) & msk;
		}
		return ulcoord;
	}
	static linear_index zorderize(const index& i)
	{
		linear_index out=0;
		for(unsigned d=0;d<D;d++)
		{
			out|=_pdep(i,pdepextmask<size_t,D>::value) << d;
		}
		return out;
	}
	static index unzorderize(const linear_index& li)
	{
		index out;
		for(unsigned d=0;d<D;d++)
		{
			out[d]=_pext(li >> d,pdepextmask<size_t,D>::value);
		}
		return out;
	}
	
	T& operator[](const size_t& s)
	{
		return data[s];
	}
	const T& operator[](const size_t& s) const
	{
		return data[s];
	}

	ndpot<T,D> from_linear_to_zorder() const
	{
		ndpot<T,D> out(side_length);
		#pragma omp parallel for
		for(size_t i=0;i<data.size();i++)
		{
			auto mi=unlinearize(i);
			auto zi=zorderize(mi);
			out[zi]=data[i];
		}
		return out;
	}
	ndpot<T,D> from_zorder_to_linear() const
	{
		ndpot<T,D> out(side_length);
		#pragma omp parallel for
		for(size_t i=0;i<data.size();i++)
		{
			auto mi=unzorderize(i);
			auto li=linearize(mi);
			out[li]=data[i];
		}
		return out;
	}
};

template<unsigned DIMS>
struct pdepextmask<uint32_t,DIMS>
{
	static const uint32_t value=static_cast<uint32_t>(pdepextmask<uint64_t,DIMS>::value & 0xFFFFFFFF);
};

template<> const uint64_t pdepextmask<uint64_t,1>::value=0xffffffffffffffffULL;
template<> const uint64_t pdepextmask<uint64_t,2>::value=0x5555555555555555ULL;
template<> const uint64_t pdepextmask<uint64_t,3>::value=0x9249249249249249ULL;
template<> const uint64_t pdepextmask<uint64_t,4>::value=0x1111111111111111ULL;
template<> const uint64_t pdepextmask<uint64_t,5>::value=0x1084210842108421ULL;
template<> const uint64_t pdepextmask<uint64_t,6>::value=0x1041041041041041ULL;
template<> const uint64_t pdepextmask<uint64_t,7>::value=0x8102040810204081ULL;
template<> const uint64_t pdepextmask<uint64_t,8>::value=0x101010101010101ULL;
template<> const uint64_t pdepextmask<uint64_t,9>::value=0x8040201008040201ULL;
template<> const uint64_t pdepextmask<uint64_t,10>::value=0x1004010040100401ULL;
template<> const uint64_t pdepextmask<uint64_t,11>::value=0x80100200400801ULL;
template<> const uint64_t pdepextmask<uint64_t,12>::value=0x1001001001001001ULL;
template<> const uint64_t pdepextmask<uint64_t,13>::value=0x10008004002001ULL;
template<> const uint64_t pdepextmask<uint64_t,14>::value=0x100040010004001ULL;
template<> const uint64_t pdepextmask<uint64_t,15>::value=0x1000200040008001ULL;
template<> const uint64_t pdepextmask<uint64_t,16>::value=0x1000100010001ULL;
template<> const uint64_t pdepextmask<uint64_t,17>::value=0x8000400020001ULL;
template<> const uint64_t pdepextmask<uint64_t,18>::value=0x40001000040001ULL;
template<> const uint64_t pdepextmask<uint64_t,19>::value=0x200004000080001ULL;
template<> const uint64_t pdepextmask<uint64_t,20>::value=0x1000010000100001ULL;
template<> const uint64_t pdepextmask<uint64_t,21>::value=0x8000040000200001ULL;
template<> const uint64_t pdepextmask<uint64_t,22>::value=0x100000400001ULL;
template<> const uint64_t pdepextmask<uint64_t,23>::value=0x400000800001ULL;
template<> const uint64_t pdepextmask<uint64_t,24>::value=0x1000001000001ULL;
template<> const uint64_t pdepextmask<uint64_t,25>::value=0x4000002000001ULL;
template<> const uint64_t pdepextmask<uint64_t,26>::value=0x10000004000001ULL;
template<> const uint64_t pdepextmask<uint64_t,27>::value=0x40000008000001ULL;
template<> const uint64_t pdepextmask<uint64_t,28>::value=0x100000010000001ULL;
template<> const uint64_t pdepextmask<uint64_t,29>::value=0x400000020000001ULL;
template<> const uint64_t pdepextmask<uint64_t,30>::value=0x1000000040000001ULL;
template<> const uint64_t pdepextmask<uint64_t,31>::value=0x4000000080000001ULL;
template<> const uint64_t pdepextmask<uint64_t,32>::value=0x100000001ULL;
template<> const uint64_t pdepextmask<uint64_t,33>::value=0x200000001ULL;
template<> const uint64_t pdepextmask<uint64_t,34>::value=0x400000001ULL;
template<> const uint64_t pdepextmask<uint64_t,35>::value=0x800000001ULL;
template<> const uint64_t pdepextmask<uint64_t,36>::value=0x1000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,37>::value=0x2000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,38>::value=0x4000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,39>::value=0x8000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,40>::value=0x10000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,41>::value=0x20000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,42>::value=0x40000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,43>::value=0x80000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,44>::value=0x100000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,45>::value=0x200000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,46>::value=0x400000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,47>::value=0x800000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,48>::value=0x1000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,49>::value=0x2000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,50>::value=0x4000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,51>::value=0x8000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,52>::value=0x10000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,53>::value=0x20000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,54>::value=0x40000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,55>::value=0x80000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,56>::value=0x100000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,57>::value=0x200000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,58>::value=0x400000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,59>::value=0x800000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,60>::value=0x1000000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,61>::value=0x2000000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,62>::value=0x4000000000000001ULL;
template<> const uint64_t pdepextmask<uint64_t,63>::value=0x8000000000000001ULL;

#if __BMI2__
#include<x86intrin.h>

static inline uint32_t _pdep(uint32_t v,uint32_t mask)
{
	return _pdep_u32(v,mask);
}
static inline uint64_t _pdep(uint64_t v,uint64_t mask)
{
	return _pdep_u64(v,mask);
}
static inline uint32_t _pext(uint32_t v,uint32_t mask)
{
	return _pext_u32(v,mask);
}
static inline uint64_t _pext(uint64_t v,uint64_t mask)
{
	return _pext_u64(v,mask);
}

#else

static inline size_t _pdep(size_t v,size_t mask)
{
	unsigned cur=0;
	size_t out=0;
	for(unsigned int i=0;i<sizeof(size_t)*8;i++)
	{
		if((mask >> i) & 0x1)
		{
			out|=((v >> cur++) & 0x1) << i;
		}
	}
	return out;
}
static inline size_t _pext(size_t v,size_t mask)
{
	unsigned cur=0;
	size_t out=0;
	for(unsigned int i=0;i<sizeof(size_t)*8;i++)
	{
		if((mask >> i) & 0x1)
		{
			out|=((v >> i) & 0x1) << (cur++);
		}
	}
	return out;
}

#endif

#endif
