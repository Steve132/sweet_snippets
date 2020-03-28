#ifndef REG_HPP
#define REG_HPP

#include<cstdint>

#ifdef __BIG_ENDIAN__
#define REG_BE
#else /* NO __BIG_ENDIAN__ */
#ifdef __LITTLE_ENDIAN__
#define REG_LE
#else /* NO __LITTLE_ENDIAN__ */
#ifdef BSD
#include <sys/endian.h>
#else // BSD
#include <endian.h>
#endif // BSD
#if __BYTE_ORDER == __BIG_ENDIAN
#define REG_BE
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define REG_LE
#else
#error "Unsupported Endianness!"
#endif /* __BYTE_ORDER */
#endif /* __LITTLE_ENDIAN__ */
#endif /* __BIG_ENDIAN__ */

namespace {

template<class TYPE_FULL,class TYPE_HI,class TYPE_LO> 
struct reg
{
public:
	union
	{
		TYPE_FULL full;
#ifdef REG_BE
		struct{ TYPE_HI hi;TYPE_LO lo;};
#else
		struct{ TYPE_LO lo;TYPE_HI hi;};
#endif
	};
	reg() {}
	reg(const TYPE_FULL& f):full(f) {}
	reg& operator=(const TYPE_FULL& f) { full=f; return *this; }
	operator TYPE_FULL&() { return full; }
	operator TYPE_FULL const&() const { return full; }
	
};
}

using reg8_t=uint8_t;
using reg16_t=::reg<uint16_t,reg8_t,reg8_t>;
using reg32_t=::reg<uint32_t,reg16_t,reg16_t>;
using reg64_t=::reg<uint64_t,reg32_t,reg32_t>;

#endif
