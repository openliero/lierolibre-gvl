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

#include <tut/tut.hpp>

#include <gvl/support/opt.hpp>
#include <gvl/support/profile.hpp>
#include <gvl/math/cmwc.hpp>
#include <gvl/io/encoding.hpp>
#include <gvl/io/fstream.hpp>
#include <gvl/io/stream.hpp>

#include <numeric>

namespace tut
{

struct opt_data
{

};

typedef test_group<opt_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("opt.hpp");
} // namespace

namespace tut
{


template<>
template<>
void object::test<1>()
{
#if !GVL_PROFILE
	gvl::mwc rand;
	for(int i = 0; i < 1000; ++i)
	{
		uint32_t y = rand(2, 0xffffffff);

		gvl::prepared_division d(y);

		for(uint32_t x = 3; x < 10000000; x += 1327)
		{
			uint32_t expected = x / y;
			uint32_t actual = d.quot(x);

			ensure("division was correct", expected == actual);
		}

		//std::cout << y << ',';
	}
#endif
}

template<>
template<>
void object::test<2>()
{

#if GVL_PROFILE
	uint32_t divisors[512];
	gvl::mwc rand(1);

	for(int i = 0; i < 512; ++i)
	{
		divisors[i] = rand(3, 1337);
	}

	{
		uint32_t sum = 0;
		GVL_PROF_TIMER("prepared_division");

		for(int i = 0; i < 512; ++i)
		{
			gvl::prepared_division d(divisors[i]);

			uint32_t x = 0x13371337;

			for(int j = 0; j < 65536/4; ++j)
			{
				std::pair<uint32_t, uint32_t> p;
			#define DO \
				p = d.quot_rem(x); \
				x = p.first + p.second; \
				sum += p.second

				DO;
				DO;
				DO;
				DO;
			#undef DO
			}
		}

		ensure("sum is correct", sum == 2405768521);
	}

	{
		uint32_t sum = 0;
		GVL_PROF_TIMER("normal division");

		for(int i = 0; i < 512; ++i)
		{
			uint32_t d = divisors[i];

			uint32_t x = 0x13371337;

			for(int j = 0; j < 65536/4; ++j)
			{
				uint32_t rem;
			#define DO \
				rem = (x % d); \
				x = x / d + rem; \
				sum += rem

				DO;
				DO;
				DO;
				DO;
			#undef DO
			}
		}

		ensure("sum is correct", sum == 2405768521);
	}

	{
		uint32_t sum = 0;
		GVL_PROF_TIMER("static division");

		for(int i = 0; i < 512; ++i)
		{
			uint32_t x = 0x13371337;

			for(int j = 0; j < 65536/4; ++j)
			{
				uint32_t rem;
			#define DO \
				rem = (x % 1337); \
				x = x / 1337 + rem; \
				sum += rem

				DO;
				DO;
				DO;
				DO;
			#undef DO
			}
		}

		ensure("sum is correct", sum == 502897152);
	}
#endif
}


} // namespace tut
