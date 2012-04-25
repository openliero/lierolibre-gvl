#include "tt800.hpp"
#include "../system/system.hpp"

namespace gvl
{

void tt800::seed(uint32_t new_seed)
{
	// Initialization taken from mt19937ar.c
	x[0] = new_seed;
	for(unsigned int i = 1; i < N; ++i)
	{
		x[i] = (1812433253UL * (x[i - 1] ^ (x[i - 1] >> 30)) + i); 
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
	}
	
	k = N;
}

void tt800::update()
{
	static uint32_t mag01[2] =
	{ 
		0x0, 0x8ebfd028 /* this is magic vector `a', don't change */
	};
	
	unsigned int kk = 0;
	for(; kk < N - M; ++kk)
	{
		x[kk] = x[kk + M] ^ (x[kk] >> 1) ^ mag01[x[kk] & 1];
	}
	
	for(; kk < N; ++kk)
	{
		x[kk] = x[kk + (M - N)] ^ (x[kk] >> 1) ^ mag01[x[kk] & 1];
	}
	
	k = 0;
}

tt800& global_rand()
{
	static tt800 inst(get_ticks());
	return inst;
}

}
