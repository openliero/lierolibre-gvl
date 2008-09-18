#ifndef UUID_BBDA02831ADD413B1A3552A8997B8324
#define UUID_BBDA02831ADD413B1A3552A8997B8324

#include "../support/cstdint.hpp"

namespace gvl
{

template<int R, uint32_t A>
struct cmwc
{
	
	cmwc()
	: p(R - 1)
	{
		
	}
	
	void seed(uint32_t const* v, std::size_t count)
	{
		// Seeding based on Mersenne twister's
		
		if(count > R)
			count = R;
			
		std::size_t i = 0;
		for(; i < count; ++i)
			Q[i] = v[i];
			
		for(; i < R; ++i)
		{
			uint32_t prev = Q[i - count];
			Q[i] = (1812433253UL * (prev ^ (prev >> 30)) + i);
		}
	}
	
#if 0
	void fill()
	{
		uint32_t local_c = c;
		
		for(std::size_t i = 0; i < R; ++i)
		{
			uint64_t t = uint64_t(Q[i])*A + local_c;
			
			local_c = uint32_t(t >> 32);
			uint64_t x = (t & 0xffffffff) + local_c;
					
			uint32_t overflow = uint32_t(x >> 32);
			
			local_c += overflow;
			
			Q[i] = 0xfffffffe - x - overflow;
		}
		
		c = local_c;
	}
#endif
	
	uint32_t operator()()
	{
		p = (p+1) & (R - 1);
		uint64_t t = uint64_t(Q[p])*A + c;
		
		c = uint32_t(t >> 32);
		uint64_t x = (t & 0xffffffff) + c;
				
		uint32_t overflow = uint32_t(x >> 32);
		
		c += overflow;
		
		return (Q[p] = 0xfffffffe - uint32_t(x & 0xffffffff) - overflow);
	}
	
	uint32_t Q[R];
	uint32_t c;
	uint32_t p;
	
};

struct mwc
{
	mwc() : x(time(0))
	{
	}

	uint32_t x, c;

	uint32_t operator()()
	{
		uint64_t t = uint64_t(698769069)*x + c;
		c = uint64_t(t>>32);
		x = uint64_t(t&0xffffffff);
		return x;
	}
};

typedef cmwc<4096, 131086> cmwc131086;

} // namespace gvl

#endif // UUID_BBDA02831ADD413B1A3552A8997B8324
