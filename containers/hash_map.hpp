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

#ifndef UUID_230800A056934D64F2EB04A436B203D7
#define UUID_230800A056934D64F2EB04A436B203D7

#include "generic_hash_set.hpp"
#include <functional>

namespace gvl
{

template<typename KeyT, typename ValueT>
struct hash_map_index
{
	hash_map_index(std::pair<KeyT, ValueT>* ptr = 0)
	: ptr(ptr)
	{
	}

	KeyT const& key() const
	{
		return ptr->first;
	}

	ValueT const& value() const
	{
		return ptr->second;
	}

	bool is_empty() const
	{
		return !ptr;
	}

	bool is_filled() const
	{
		return !is_null_or_dummy_ptr(ptr);
	}

	void make_empty()
	{
		if(is_filled())
			delete ptr;
		ptr = 0;
	}

	void make_deleted()
	{
		delete ptr;
		ptr = dummy_ptr<std::pair<KeyT, ValueT> >();
	}

	void assign_value(hash_map_index v)
	{
		// TODO: Either copy v.ptr->second and delete v.ptr,
		// or delete ptr and assign v.ptr to ptr. The latter
		// seems to be cheaper, so that has been chosen tentatively.
		delete ptr;
		ptr = v.ptr;
	}

	std::pair<KeyT, ValueT>* ptr;
};

template<typename KeyT, typename ValueT, typename Hash = hash_functor, typename Compare = std::equal_to<KeyT> >
struct hash_map : generic_hash_set<hash_map_index<KeyT, ValueT>, KeyT, ValueT, Hash, Compare>
{
	typedef generic_hash_set<hash_map_index<KeyT, ValueT>, KeyT, ValueT, Hash, Compare> base;

	typedef std::pair<KeyT, ValueT> index_type;

	hash_map(Hash const& hash = Hash(), Compare const& compare = Compare())
	: base(hash, compare)
	{
	}

	void insert(index_type* v)
	{
		base::insert(hash_map_index<KeyT, ValueT>(v));
	}

	template<typename SpecKeyT>
	ValueT* get(SpecKeyT const& v)
	{
		hash_map_index<KeyT, ValueT>* index = base::lookup(v);

		return index ? &index->ptr->second : 0;
	}
};

}

#endif // UUID_230800A056934D64F2EB04A436B203D7
