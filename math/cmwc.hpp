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

#ifndef UUID_BBDA02831ADD413B1A3552A8997B8324
#define UUID_BBDA02831ADD413B1A3552A8997B8324

#include "../support/cstdint.hpp"
#include "random.hpp"
#include <cstddef>

namespace gvl
{

// NOTE: This generator only generates
// numbers in [0, 2^32-1)
template<int R, uint32_t A>
struct cmwc // : prng_common<cmwc<R, A>, uint32_t>
{
	//using prng_common<cmwc<R, A>, uint32_t>::operator();

	template<typename Archive, int R2, uint32_t A2>
	friend void archive(Archive ar, cmwc<R2, A2>& x);

	cmwc()
	: p(R - 1)
	, c(0x1337)
	{

	}

	void seed(uint32_t v)
	{
		seed(&v, 1);
	}

	void seed(uint32_t const* v, std::size_t count)
	{
		// Seeding based on Mersenne twister's

		c = 0x1337;

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

			Q[i] = 0xfffffffe - uint32_t(x & 0xffffffff) - overflow;
		}

#if 0
		uint64_t t1 = uint64_t(Q[i])*A + local_c1;
		uint64_t t2 = uint64_t(Q[i+1])*A;

		uint32_t local_c2 = uint32_t(t >> 32);

		uint64_t x1 = (t1 & 0xffffffff) + local_c2;

		uint32_t overflow1 = uint32_t(x1 >> 32);

		local_c2 += overflow1;

		t2 += local_c2;

		local_c2 = uint32_t(t2 >> 32);

		uint64_t x2 = (t2 & 0xffffffff) + local_c2;

		uint32_t overflow2 = uint32_t(x2 >> 32);

		local_c2 += overflow2;

		Q[i] = 0xfffffffe - uint32_t(x1 & 0xffffffff) - overflow1;
		Q[i+1] = 0xfffffffe - uint32_t(x2 & 0xffffffff) - overflow2;
#endif

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

private:
	uint32_t Q[R];
	uint32_t c;
	uint32_t p;
};

template<typename Archive, int R, uint32_t A>
void archive(Archive ar, cmwc<R, A>& x)
{
	ar.ui32(x.c);
	for(int i = 0; i < R; ++i)
		ar.ui32(x.Q[i]);
}

struct mwc : prng_common<mwc, uint32_t>
{
	using prng_common<mwc, uint32_t>::operator();

	mwc(uint32_t seed_new = 0x1337)
	{
		seed(seed_new);
	}

	uint32_t x, c;

	bool operator==(mwc const& b)
	{
		return x == b.x && c == b.c;
	}

	bool operator!=(mwc const& b)
	{
		return !operator==(b);
	}

	void seed(uint32_t seed_new)
	{
		x = seed_new;
		c = 9413207;
	}

	uint32_t operator()()
	{
		uint64_t t = uint64_t(2083801278)*x + c;
		c = uint32_t(t>>32);
		x = uint32_t(t&0xffffffff);
		return x;
	}
};

template<typename Archive>
void archive(Archive ar, mwc& x)
{
	ar.ui32(x.c);
	ar.ui32(x.x);
}

template<int A, int B, int C>
struct xorshift
{
	uint32_t x;

	xorshift(uint32_t seed)
	: x(seed)
	{

	}

	uint32_t operator()()
	{
		uint32_t v = x;

		v ^= v << A;
		v ^= v >> B;
		v ^= v << C;
		return (x = v);
	}
};

typedef cmwc<4096, 18782> cmwc18782;
typedef cmwc<4, 987654978> cmwc987654978;
typedef xorshift<2, 9, 15> default_xorshift;

} // namespace gvl

#endif // UUID_BBDA02831ADD413B1A3552A8997B8324
