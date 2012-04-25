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

#include "../../containers/deque.hpp"
#include "../../math/tt800.hpp"
#include "../../support/algorithm.hpp"
#include "../../resman/allocator.hpp"
#include "../../support/platform.hpp"
#include <functional>
#include <cmath>
#include <set>
#include <ctime>
#include <cstdio>

struct track_obj;

std::set<track_obj*> track_obj_live;
int track_obj_copies = 0;

#define MOVABLE CPP0X

struct track_obj
{
	track_obj(int v)
	: v(v)
	{
		track_obj_live.insert(this);
	}

#if !MOVABLE
	track_obj(track_obj const& other)
	: v(other.v)
	{
		++track_obj_copies;
		track_obj_live.insert(this);
	}

	track_obj& operator=(track_obj const& other)
	{
		++track_obj_copies;
		v = other.v;
		return *this;
	}
#endif

	~track_obj()
	{
		if(track_obj_live.count(this) > 0)
			track_obj_live.erase(this);
	}

	int v;

#if MOVABLE
	track_obj(track_obj&& other)
	: v(other.v)
	{
		track_obj_live.insert(this);
	}

	track_obj& operator=(track_obj&& other)
	{
		v = other.v;
		return *this;
	}

private:
	track_obj(track_obj const& other);
	track_obj& operator=(track_obj const& other);
#endif

};

GVLTEST_SUITE(gvl, deque)

GVLTEST(gvl, deque, push_pop)
{
	typedef gvl::deque<track_obj> l_t;

	l_t l;

	gvl::tt800 r(1234);

	int const iter = 1000;
	int const limit = iter;

	for(int i = 0; i < iter; ++i)
	{
		l.push_back(track_obj(i));
	}

	for(int i = 0; i < iter; ++i)
	{
		track_obj& v = l.front();
		GVLTEST_ASSERT("popped in the right order", v.v == i);
		l.pop_front();
	}

	GVLTEST_ASSERT("empty after pops", l.empty());

	GVLTEST_ASSERT("all track_obj were destroyed", track_obj_live.empty());

#if MOVABLE
	GVLTEST_ASSERT("no copied were made in C++0x", track_obj_copies == 0);
#endif
}
