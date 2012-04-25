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

#ifndef UUID_E882406B58BE4B04808FB79776E9882A
#define UUID_E882406B58BE4B04808FB79776E9882A

#include "debug.hpp"

namespace gvl
{

// Bit set with very cheap clearing

template<typename Tag>
struct cheap_flags_item
{
	cheap_flags_item()
	: last_iter_set(-1)
	{
	}

	int last_iter_set;
};

template<typename Tag>
struct cheap_flags
{
	cheap_flags()
	: iter_count(0)
	{
	}

	bool operator[](cheap_flags_item<Tag> const& item)
	{
		return item.last_iter_set >= iter_count;
	}

	void set(cheap_flags_item<Tag>& item)
	{
		item.last_iter_set = iter_count;
	}

	void reset(cheap_flags_item<Tag>& item)
	{
		item.last_iter_set = -1;
	}

	void clear()
	{
		passert((iter_count + 1) > iter_count, "Clear limit reached");
		++iter_count;
	}

	int iter_count;
};

struct cheap_flags_dummy_tag;

struct cheap_flags_vector_item : cheap_flags_item<cheap_flags_dummy_tag>
{
};

struct cheap_flags_vector : cheap_flags<cheap_flags_dummy_tag>
{
	// TODO
	std::vector<cheap_flags_vector_item> items;
};

} // namespace gvl

#endif // UUID_E882406B58BE4B04808FB79776E9882A
