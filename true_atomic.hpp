#ifndef TRUE_ATOMIC_HPP
#define TRUE_ATOMIC_HPP

#ifdef __linux__
#include<linux/cache.h>
#endif

#if defined(__x86_64__) || defined(_M_ARM64) || defined(_M_IX86) || defined(_M_AMD64)
	#define L1_CACHE_BYTES 64
#endif

#include<new>
#include<atomic>
#include<cstdint>
#include<array>

#ifdef __cpp_lib_hardware_interference_size
	using std::hardware_constructive_interference_size;
	using std::hardware_destructive_interference_size;
#elifdef L1_CACHE_BYTES
	constexpr std::size_t hardware_constructive_interference_size = L1_CACHE_BYTES;
	constexpr std::size_t hardware_destructive_interference_size = L1_CACHE_BYTES;
#else
	constexpr std::size_t hardware_constructive_interference_size = 256;
	constexpr std::size_t hardware_destructive_interference_size = 256;
#endif


template<class T,class Enable=void>
struct alignas(hardware_constructive_interference_size) true_atomic:
	public std::atomic<T>
{
public:
	using std::atomic<T>::atomic;
	using std::atomic<T>::operator T;
	using std::operator=;
private:
	std::uint8_t pad[INTERFERENCE-sizeof(T)];
};

#endif
