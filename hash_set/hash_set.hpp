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

#ifndef UUID_E98E1BBBCFA240F5603AFC8F48E7B59F
#define UUID_E98E1BBBCFA240F5603AFC8F48E7B59F

#include "generic_hash_set.hpp"
#include "../support/debug.hpp"
#include <functional>

namespace gvl
{

template<typename KeyT>
struct hash_set_index_new
{
	hash_set_index_new(KeyT* ptr = 0)
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

	void make_empty()
	{
		if(!is_empty())
			delete ptr;
		ptr = 0;
	}

	void assign_value(hash_set_index_new v)
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
	typename Compare = equal_to>
struct hash_set_new : generic_hash_set_new<hash_set_index_new<KeyT>, KeyT, KeyT, Hash, Compare>
{
	typedef generic_hash_set_new<hash_set_index_new<KeyT>, KeyT, KeyT, Hash, Compare> base;

	hash_set_new(Hash const& hash = Hash(), Compare const& compare = Compare())
	: base(hash, compare)
	{
	}

	void insert(KeyT const& v)
	{
		base::insert(hash_set_index_new<KeyT>(new KeyT(v)));
	}

	void insert(KeyT* v)
	{
		base::insert(hash_set_index_new<KeyT>(v));
	}

	template<typename SpecKeyT>
	bool has(SpecKeyT const& v)
	{
		return base::lookup(v) != 0;
	}

	template<typename SpecKeyT>
	KeyT* get(SpecKeyT const& v)
	{
		hash_set_index_new<KeyT>* index = base::lookup(v);

		return index ? index->ptr : 0;
	}

	template<typename SpecKeyT>
	KeyT& operator[](SpecKeyT const& v)
	{
		hash_set_index_new<KeyT>* index = base::lookup(v);
		if(index)
			return *index->ptr;
		KeyT* k = new KeyT(v); // !exp
		insert(k);
		return *k;
	}

	template<typename SpecKeyT>
	KeyT* release(SpecKeyT const& v)
	{
		hash_set_index_new<KeyT>* index = base::lookup(v);

		return index ? index->release() : 0;
	}

	template<typename SpecKeyT>
	void erase(SpecKeyT const& v)
	{
		if(KeyT* k = release(v))
			delete k;
	}
};

template<typename KeyT>
struct hash_set_index_unboxed
{
	hash_set_index_unboxed()
	: k()
	{
	}

	hash_set_index_unboxed(KeyT const& k_init)
	: k(k_init)
	{
	}

#if GVL_CPP0X
	hash_set_index_unboxed(KeyT&& k_init)
	: k(k_init)
	{
	}
#endif

	KeyT const& key() const
	{
		return k;
	}

	KeyT const& value() const
	{
		return k;
	}

	bool is_empty() const
	{
		return !k;
	}

	void make_empty()
	{
		k = KeyT();
	}

	void assign_value(hash_set_index_unboxed const& v)
	{
		k = v.k;
	}

#if GVL_CPP0X
	void assign_value(hash_set_index_unboxed&& v)
	{
		k = GVL_MOVE(v.k);
	}
#endif

	KeyT k;
};

template<typename KeyT,
	typename Hash = hash_functor,
	typename Compare = equal_to>
struct hash_set_unboxed : generic_hash_set_new<hash_set_index_unboxed<KeyT>, KeyT, KeyT, Hash, Compare>
{
	typedef generic_hash_set_new<hash_set_index_unboxed<KeyT>, KeyT, KeyT, Hash, Compare> base;

	hash_set_unboxed(Hash const& hash = Hash(), Compare const& compare = Compare())
	: base(hash, compare)
	{
	}

#if GVL_CPP0X
	void insert(KeyT&& v)
	{
		base::insert(hash_set_index_unboxed<KeyT>(v));
	}
#endif

	void insert(KeyT const& v)
	{
		base::insert(hash_set_index_unboxed<KeyT>(v));
	}

	template<typename SpecKeyT>
	bool has(SpecKeyT const& v)
	{
		return base::lookup(v) != 0;
	}

	template<typename SpecKeyT>
	KeyT* get(SpecKeyT const& v)
	{
		hash_set_index_unboxed<KeyT>* index = base::lookup(v);

		return index ? &index->k : 0;
	}

	template<typename SpecKeyT>
	KeyT& operator[](SpecKeyT const& v)
	{
		hash_set_index_unboxed<KeyT>* index = base::lookup(v);
		if(index)
			return index->k;
		insert(KeyT(v));
		return *k;
	}

	template<typename SpecKeyT>
	KeyT release(SpecKeyT const& v)
	{
		hash_set_index_unboxed<KeyT>* index = base::lookup(v);

		if(index)
		{
			KeyT k(GVL_MOVE(index->k));
			index->k = KeyT();
			return GVL_MOVE(k);
		}
		return index ? index->release() : 0;
	}

	template<typename SpecKeyT>
	void erase(SpecKeyT const& v)
	{
		if(hash_set_index_unboxed<KeyT>* index = base::lookup(v))
			index->k = KeyT();
	}
};

}

#endif // UUID_E98E1BBBCFA240F5603AFC8F48E7B59F
