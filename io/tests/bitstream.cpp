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

#include "../common_bitstream.hpp"
#include "../../math/tt800.hpp"
#include "../../support/cstdint.hpp"
#include "../../support/macros.hpp"
#include <functional>
#include <vector>
#include <utility>

template<typename T, std::size_t N>
std::size_t array_size(T(&)[N])
{
	return N;
}

GVLTEST_SUITE(gvl, bitstream)

GVLTEST(gvl, bitstream, put_get_ignore)
{
	typedef std::vector<std::pair<uint32_t, uint32_t> > SampleVec;
	SampleVec samples;

	gvl::tt800 rand(0);

	for(int i = 0; i < 1000; ++i)
	{
		uint32_t bits = rand.range(1, 33);
		uint32_t v;
		if(bits == 32)
			v = rand();
		else
			v = rand.range(0u, 1u<<bits);

		samples.push_back(std::make_pair(bits, v));
	}

	gvl::vector_bitstream bs;

	{
		bs.put_uint(0xabcde, 20);
		bs.put_uint(0xf012, 16);

		bs.finish();

		uint32_t a = bs.get_uint(20);
		uint32_t b = bs.get_uint(16);

		ASSERTEQM("a is correct", a, 0xabcde);
		ASSERTEQM("b is correct", b, 0xf012);

		bs.rewindg();

		bs.ignore(20);
		b = bs.get_uint(16);
		ASSERTEQM("b is correct", b, 0xf012);

		bs.clear();
	}

	FOREACH(SampleVec, i, samples)
	{
		bs.put_uint(i->second, i->first);
	}

	bs.finish();

	FOREACH(SampleVec, i, samples)
	{
		uint32_t v = bs.get_uint(i->first);
		ASSERTEQM("integers encoded right", v, i->second);
	}

	bs.rewindg();

	// Get with interjected ignores
	int ignoreLen = 0;
	FOREACH(SampleVec, i, samples)
	{
		if(rand() & 1)
			ignoreLen += i->first;
		else
		{
			if(ignoreLen > 0)
			{
				bs.ignore(ignoreLen);
				ignoreLen = 0;
			}
			uint32_t v = bs.get_uint(i->first);
			ASSERTEQM("integers encoded right", v, i->second);

		}
	}

	bs.clear();

	FOREACH(SampleVec, i, samples)
	{
		bs.put(i->second & 1);
	}

	bs.finish();

	FOREACH(SampleVec, i, samples)
	{
		uint32_t v = bs.get();
		ASSERTEQM("bits encoded right", v, (i->second & 1));
	}

}
