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

#include "../../test/test.hpp"

#include "../../math/cmwc.hpp"
#include "../bits.hpp"

int reference_trailing_zeroes(uint32_t v)
{
	int c = 0;
	if(!v) return 0;
	while(!((v >> c) & 1))
	{
		++c;
	}
	return c;
}

int reference_bottom_bit(uint32_t v)
{
	if(!v) return -1;
	return reference_trailing_zeroes(v);
}

int reference_log2(uint32_t v)
{
	int c = 0;
	while((v >>= 1))
		++c;
	return c;
}

int reference_top_bit(uint32_t v)
{
	if(!v) return -1;
	return reference_log2(v);
}

int32_t reference_saturate0(int32_t x)
{
	return x < 0 ? 0 : x;
}

uint32_t reference_bswap(uint32_t x)
{
	return (x >> 24)
	| ((x >> 8) & 0xff00)
	| ((x << 8) & 0xff0000)
	| ((x << 24) & 0xff000000);
}

uint64_t reference_bswap(uint64_t x)
{
	return reference_bswap(uint32_t(x >> 32)) | (uint64_t(reference_bswap(uint32_t(x))) << 32);
}

GVLTEST_SUITE(gvl, bits)

GVLTEST(gvl, bits, compare_against_reference)
{
	gvl::mwc rand;
	for(int i = 0; i < 1000000; ++i)
	{
		uint32_t x = rand();
		uint64_t x64 = (uint64_t(rand()) << 32) | x;
		ASSERTEQM("trailing_zeroes", reference_trailing_zeroes(x), gvl::trailing_zeroes(x));
		ASSERTEQM("bottom_bit", reference_bottom_bit(x), gvl::bottom_bit(x));
		ASSERTEQM("log2", reference_log2(x), gvl::log2(x));
		ASSERTEQM("top_bit", reference_top_bit(x), gvl::top_bit(x));
		ASSERTEQM("saturate0", reference_saturate0(int32_t(x)), gvl::saturate0(int32_t(x)));
		ASSERTEQM("bswap", reference_bswap(x), gvl::bswap(x));
		ASSERTEQM("bswap 64-bit", reference_bswap(x64), gvl::bswap(x64));
	}
}
