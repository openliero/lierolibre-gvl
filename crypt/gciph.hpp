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

#ifndef UUID_EF37EB8B8BAB4601A60DCDBA8B3B9F33
#define UUID_EF37EB8B8BAB4601A60DCDBA8B3B9F33

#include "../math/cmwc.hpp"

// NOTE: Highly experimental
namespace gvl
{



struct gciph
{
	cmwc131086 base_gen;

	static inline uint32_t rot(uint32_t x, int count)
	{
		return (x << count) | (x >> (32-count));
	}

	static inline void mix(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d)
	{
		a = rot(a + d, 17);
		b = rot(b + a, 16);
		c = rot(c + b, 8);
		d = rot(d + c, 7);

		a ^= d;
		b ^= a;
		c ^= b;
		d ^= c;
	}

	void gen()
	{
		uint32_t k[8], o[8];

		for(int i = 0; i < 8; ++i)
			o[i] = k[i] = base_gen();

		// 0 1 _ _ 4 5 _ _
		// _ 1 2 _ _ 5 6 _
		// _ _ 2 3 _ _ 6 7
		// 0 _ _ 3 4 _ _ 7

		mix(k[0], k[1], k[4], k[5]);
		mix(k[1], k[2], k[5], k[6]);
		mix(k[2], k[3], k[6], k[7]);
		mix(k[3], k[4], k[7], k[0]);

		for(int i = 0; i < 8; ++i)
			k[i] += o[i];
	}
};

}

#endif // UUID_EF37EB8B8BAB4601A60DCDBA8B3B9F33
