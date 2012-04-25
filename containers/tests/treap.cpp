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

#include "../treap2.hpp"
#include "../../math/tt800.hpp"
#include "../../support/algorithm.hpp"
#include "../../resman/allocator.hpp"
#include <functional>
#include <cmath>
#include <set>
#include <ctime>
#include <cstdio>

struct treap_integer2 : gvl::treap_node2<>
{
	treap_integer2(int v)
	: v(v)
	{
	}
	
	bool operator<(treap_integer2 const& b) const
	{
		return v < b.v;
	}
	
	int v;
};

template<typename T>
struct greater : std::binary_function<T const&, T const&, bool>
{
	bool operator()(T const& a, T const& b) const
	{
		return b < a;
	}
};

double log2(double x)
{
	return std::log(x) / std::log(2.0);
}

struct timer
{
	timer()
	: start(std::clock())
	{
	}
	
	~timer()
	{
		std::clock_t stop = std::clock();
		std::printf("Time: %f\n", (stop - start) / (double)(CLOCKS_PER_SEC));
	}
	
	std::clock_t start;
};

GVLTEST_SUITE(gvl, treap)

GVLTEST(gvl, treap, push_erase_find)
{
	//typedef gvl::treap<treap_integer> l1_t;
	typedef std::set<int> l2_t;
	typedef gvl::treap2<treap_integer2> l3_t;
	
	
	//l1_t l1;
	/*l2_t l2;*/
	l3_t l3;
	
	gvl::tt800 r(1234);
	
	int const iter = 1000;
	int const limit = iter;
	

	{
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			l3.insert(new treap_integer2(/*repeat*/r.range(0, limit)));
		}
	}
	
	
	int sum = 0;
	
	{
		for(int repeat = 0; repeat < 30; ++repeat)
		{
			int v = r.range(0, limit);
			
			for(l3_t::range iter = l3.all(); !iter.empty(); iter.pop_front())
			{
				sum += iter.front().v;
			}
		}
	}
	
	{
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			int v = r.range(0, limit);
			
			l3_t::range iter = l3.find(v);
			if(!iter.empty())
				l3.erase_front(iter);
				
			//l3.integrity_check();
		}
	}
		
	{
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			int v = r.range(0, limit);
			
			l3.find(v);
		}
	}
	
}

GVLTEST(gvl, treap, useless)
{
	typedef gvl::treap2<treap_integer2> l1_t;
	l1_t l1;
	
	gvl::tt800 r(1234);
	
	int const iter = 1500000;
	
	
}
