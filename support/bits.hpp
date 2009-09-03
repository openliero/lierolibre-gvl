#ifndef UUID_D006EF6EB7A24020D1926ABC53D805D6
#define UUID_D006EF6EB7A24020D1926ABC53D805D6

#include "cstdint.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)
#endif

namespace gvl
{

int trailing_zeroes(uint32_t v);
int bottom_bit(uint32_t v);

#if defined(_MSC_VER)
inline int log2(uint32_t v)
{
	unsigned long r;
	if(!_BitScanReverse(&r, v))
		r = 0;
	return r;
}

inline int top_bit(uint32_t v)
{
	unsigned long r;
	if(!_BitScanReverse(&r, v))
		return -1;
	return r;
}

inline int log2(uint64_t v)
{
	unsigned long r = 0;
	if(_BitScanReverse(&r, (uint32_t)(v >> 32)))
		return 32 + r;
	if(!_BitScanReverse(&r, (uint32_t)v))
		return 0;
	return r;
}
#else
int log2(uint32_t v);
int log2(uint64_t v);
int top_bit(uint32_t v);
#endif

inline int ceil_log2(uint32_t v)
{
	return v == 0 ? 0 : log2(v - 1) + 1;
}

inline int even_log2(uint32_t v)
{
	// TODO: Special look-up table for this
	return ((log2(v) + 1) & ~1);
}

inline int odd_log2(uint32_t v)
{
	return (log2(v) | 1);
}

inline int odd_log2(uint64_t v)
{
	return (log2(v) | 1);
}

/* Returns v if v >= 0, otherwise 0 */
inline int32_t saturate0(int32_t v)
{
	/* NOTE! This depends on:
	(-1>>31) == -1 &&
	~(-1) == 0 &&
	(v & -1) == v
	*/
	return (v & ~(v >> 31));
}

inline int32_t udiff(uint32_t x, uint32_t y)
{
	x -= y;
	return x < 0x80000000ul ? int32_t(x) : int32_t(x - 0x80000000ul) - 0x80000000;
}

/* lsb_mask(x) works for x in [1, 32] */
inline uint32_t lsb_mask(int x)
{
	return (~uint32_t(0)) >> uint32_t(32-x);
}

/* Left shift that works for o in [1, 32] */
inline uint32_t shl_1_32(uint32_t v, uint32_t o)
{
	return (v << (o - 1)) << 1;
}

inline bool all_set(uint32_t v, uint32_t f)
{
	return (v & f) == f;
}

inline bool is_power_of_two(uint32_t x)
{
	return (x & (x-1)) == 0 && x != 0;
}

void write_uint32(uint8_t* ptr, uint32_t v);
uint32_t read_uint32(uint8_t const* ptr);
void write_uint16(uint8_t* ptr, uint32_t v);
uint32_t read_uint16(uint8_t const* ptr);

} // namespace gvl

#endif // UUID_D006EF6EB7A24020D1926ABC53D805D6
