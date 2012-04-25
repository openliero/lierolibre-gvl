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

#ifndef UUID_6D485E50691B483866E152A1029D5226
#define UUID_6D485E50691B483866E152A1029D5226

#include "../support/cstdint.hpp"

namespace gvl
{

struct tt800
{
	static unsigned int const N = 25;
	static unsigned int const M = 7;

	typedef uint32_t value_type;

	tt800(uint32_t new_seed)
	: k(0)
	{
		seed(new_seed);
	}

	void seed(uint32_t new_seed);

	uint32_t operator()()
	{
		if(k == N)
			update();

		uint32_t y = x[k++];
		y ^= (y << 7) & 0x2b5b2500; /* s and b, magic vectors */
		y ^= (y << 15) & 0xdb8b0000; /* t and c, magic vectors */

		return y;
	}

	/*
	int32_t range(int32_t a, int32_t b)
	{
		return a + int32_t((b - a) * uint64_t(operator()()) >> 32);
	}*/

	uint32_t range(uint32_t a, uint32_t b)
	{
		return a + uint32_t((b - a) * uint64_t(operator()()) >> 32);
	}

	template<typename Archive>
	void serialize(Archive& arch);

private:
	void update();

	uint32_t x[N];
	uint32_t k;
	uint32_t front_;
};

template<typename Archive>
void tt800::serialize(Archive& arch)
{
	for(int i = 0; i < N; ++i)
		arch.uint(32, x[i]);
	arch.uint(32, k);
}

tt800& global_rand();

} // namespace gvl

#endif // UUID_6D485E50691B483866E152A1029D5226
