#ifndef ENDIAN_COMPILE_TIME_HPP
#define ENDIAN_COMPILE_TIME_HPP

#include <cstdint>
#include <type_traits>
#include <cstdlib>
#include <algorithm>

#if defined(_MSC_VER)
	#define ESWAP16 _byteswap_ushort
	#define ESWAP32 _byteswap_ulong
	#define ESWAP64 _byteswap_uint64
#elif defined(__GNUC__) || defined(__clang__)
	#define ESWAP16 __builtin_bswap16
	#define ESWAP32 __builtin_bswap32
	#define ESWAP64 __builtin_bswap64
#else
	#error "No compiler detected for bswap"
#endif

namespace endian
{

namespace {

template<class T,unsigned int N>
struct swap_bytes { static T call(const T& x) {
	union {T tx; uint8_t bits[N]; };
	tx=x;
	std::reverse(bits,bits+N);
	return tx;
}};
	
template<class T>
struct swap_bytes<T,1> { static T call(const T& x) {
	return x;
}};

template<class T>
struct swap_bytes<T,2> { static T call(const T& x) {
	union { T tx; uint16_t bits; };
	tx=x;
	bits=ESWAP16(bits);
	return tx;
}};
template<class T>
struct swap_bytes<T,4> { static T call(const T& x) {
	union { T tx; uint32_t bits; };
	tx=x;
	bits=ESWAP32(bits);
	return tx;
}};
template<class T>
struct swap_bytes<T,8> { static T call(const T& x) {
	union { T tx; uint64_t bits; };
	tx=x;
	bits=ESWAP64(bits);
	return tx;
}};

static constexpr const bool host_be=(__BYTE_ORDER == __BIG_ENDIAN);
	
template<class T,bool swapper>
struct swapper_if_needed {
	static T call(const T& x);
};
template<class T>
struct swapper_if_needed<T,false> {
    static T call(const T& x) { return x; }
};

template<class T>
struct swapper_if_needed<T,true> {
    static T call(const T& x) { return swap_bytes<T,sizeof(T)>::call(x); }
};
}
	
template<class T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value,T>::type
host2be(const T& x) {
	return swapper_if_needed<T,host_be ^ true>::call(x);
}
template<class T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value,T>::type
be2host(const T& x) {
	return swapper_if_needed<T,host_be ^ true>::call(x);
}
template<class T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value,T>::type
host2le(const T& x) {
	return swapper_if_needed<T,host_be ^ false>::call(x);
}
template<class T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value,T>::type
le2host(const T& x) {
	return swapper_if_needed<T,host_be ^ false>::call(x);
}
template<class T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value,T>::type
be2le(const T& x) {
	return swapper_if_needed<T,true>::call(x);
}
template<class T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value,T>::type
le2be(const T& x) {
	return swapper_if_needed<T,true>::call(x);
}
}

#endif
