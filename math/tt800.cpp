/*
 * Copyright (c) 2010, Erik Lindroos <gliptic@gmail.com>
 * This software is released under the The BSD-2-Clause License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
