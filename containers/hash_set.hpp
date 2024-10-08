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

#ifndef UUID_D140C63FD82B4342E7DD71BD6C73E92D
#define UUID_D140C63FD82B4342E7DD71BD6C73E92D

#include "generic_hash_set.hpp"
#include <functional>

namespace gvl
{

template<typename KeyT>
struct hash_set_index
{
	hash_set_index(KeyT* ptr = 0)
	: ptr(ptr)
	{
	}

	KeyT& key() const
	{
		return *ptr;
	}

	KeyT& value() const
	{
		return *ptr;
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
		ptr = dummy_ptr<KeyT>();
	}

	void assign_value(hash_set_index v)
	{
		delete ptr;
		ptr = v.ptr;
	}

	// hash_set specific
	KeyT* release()
	{
		KeyT* ret = ptr;
		ptr = dummy_ptr<KeyT>();
		return ptr;
	}

	KeyT* ptr;
};

template<typename KeyT,
	typename Hash = hash_functor,
	typename Compare = std::equal_to<KeyT> >
struct hash_set : generic_hash_set<hash_set_index<KeyT>, KeyT, KeyT, Hash, Compare>
{
	typedef generic_hash_set<hash_set_index<KeyT>, KeyT, KeyT, Hash, Compare> base;

	hash_set(Hash const& hash = Hash(), Compare const& compare = Compare())
	: base(hash, compare)
	{
	}

	void insert(KeyT const& v)
	{
		base::insert(hash_set_index<KeyT>(new KeyT(v)));
	}

	void insert(KeyT* v)
	{
		base::insert(hash_set_index<KeyT>(v));
	}

	template<typename SpecKeyT>
	bool has(SpecKeyT const& v)
	{
		return base::lookup(v) != 0;
	}

	template<typename SpecKeyT>
	KeyT* get(SpecKeyT const& v)
	{
		hash_set_index<KeyT>* index = base::lookup(v);

		return index ? index->ptr : 0;
	}

	template<typename SpecKeyT>
	KeyT& operator[](SpecKeyT const& v)
	{
		hash_set_index<KeyT>* index = base::lookup(v);
		if(index)
			return *index->ptr;
		KeyT* k = new KeyT(v); // !exp
		insert(k);
		return *k;
	}

	template<typename SpecKeyT>
	KeyT* release(SpecKeyT const& v)
	{
		hash_set_index<KeyT>* index = base::lookup(v);

		return index ? index->release() : 0;
	}

	template<typename SpecKeyT>
	void erase(SpecKeyT const& v)
	{
		if(KeyT* k = release(v))
			delete k;
	}
};

}

#endif // UUID_D140C63FD82B4342E7DD71BD6C73E92D
