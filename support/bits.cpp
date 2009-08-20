#include "bits.hpp"

namespace gvl
{

/* Taken mostly from http://graphics.stanford.edu/~seander/bithacks.html */
int trailing_zeroes(uint32_t v)
{
#ifdef _MSC_VER
	unsigned long r;
	if(!_BitScanForward(&r, v))
		return 0;
	return r;
#else
	static const int MultiplyDeBruijnBitPosition[32] = 
	{
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	
	return MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531UL)) >> 27];
#endif
}

int bottom_bit(uint32_t v)
{
#ifdef _MSC_VER
	unsigned long r;
	if(!_BitScanForward(&r, v))
		return -1;
	return r;
#else
	if(!v)
		return -1;
		
	static const int MultiplyDeBruijnBitPosition[32] = 
	{
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	
	return MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531UL)) >> 27];
#endif
}

#if !defined(_MSC_VER)
int log2(uint32_t v)
{
	static const char LogTable256[] = 
	{
	  0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
	};
	
	unsigned int t, tt;
	if((tt = v >> 16))
		return (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
	else 
		return (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
}

int top_bit(uint32_t v)
{
	static const char LogTable256[] = 
	{
	  -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
	};
	
	unsigned int t, tt;
	if((tt = v >> 16))
		return (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
	else 
		return (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
}

int log2(uint64_t v)
{

	// TODO: For 64-bit archs, use intrinsics
	unsigned int ttt;
	
	if((ttt = (uint32_t)(v >> 32)))
		return 32 + log2(ttt);
	else
		return log2((uint32_t)v);
}
#endif
/*
int even_log2(uint32_t v)
{
	// TODO: Special look-up table for this
	return ((log2(v) + 1) & ~1);
}

int odd_log2(uint32_t v)
{
	return (log2(v) | 1);
}

int odd_log2(uint64_t v)
{
	return (log2(v) | 1);
}*/

void write_uint32(uint8_t* ptr, uint32_t v)
{
	ptr[0] = (uint8_t)(v >> 24);
	ptr[1] = (uint8_t)(v >> 16);
	ptr[2] = (uint8_t)(v >> 8);
	ptr[3] = (uint8_t)(v);
}

uint32_t read_uint32(uint8_t const* ptr)
{
	return (ptr[0] << 24) + (ptr[1] << 16) + (ptr[2] << 8) + ptr[3];
}

void write_uint16(uint8_t* ptr, uint32_t v)
{
	ptr[0] = (uint8_t)(v >> 8);
	ptr[1] = (uint8_t)(v);
}

uint32_t read_uint16(uint8_t const* ptr)
{
	return (ptr[0] << 8) + ptr[1];
}

} // namespace gvl
