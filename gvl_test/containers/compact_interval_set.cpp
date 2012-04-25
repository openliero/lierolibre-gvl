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

#include <gvl/containers/compact_interval_set.hpp>
#include <gvl/math/cmwc.hpp>
#include <functional>
#include <memory>
#include <algorithm>

// For bounded_rectangle_packer
#include <gvl/math/rect.hpp>
#include <gvl/containers/list.hpp>
#include <vector>

namespace tut
{

struct cis_data
{

};

typedef test_group<cis_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::compact_interval_set");
} // namespace

namespace tut
{

template<>
template<>
void object::test<1>()
{
	gvl::compact_interval_set<int> s;

	s.insert_no_overlap(0, 100);   // [0, 100)
	s.insert_no_overlap(150, 200); // [0, 100) [150, 200)
	s.insert_no_overlap(100, 150); // [0, 200)
	ensure("ranges merged after insert", s.end() - s.begin() == 1);
	s.erase_no_overlap(50, 150);  // [0, 50) [150, 200)
	ensure("range split in two", s.end() - s.begin() == 2);
	s.erase_no_overlap(0, 50);    // [150, 200)
	ensure("range erased", s.end() - s.begin() == 1);
	s.erase_no_overlap(150, 175); // [175, 200)
	ensure("range cut at front", (s.end() - s.begin() == 1) && s.begin()->begin == 175);
	s.erase_no_overlap(190, 200); // [175, 190)
	ensure("range cut at back", (s.end() - s.begin() == 1) && s.begin()->end == 190);
	s.insert_no_overlap(0, 100);   // [0, 100) [175, 190)
	s.insert_no_overlap(100, 175); // [0, 190)
	ensure("three ranges merged", (s.end() - s.begin() == 1) && s.begin()->begin == 0);
}


} // namespace tut
