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

#ifndef UUID_115639EF65E5430E91CFED97BFA7B850
#define UUID_115639EF65E5430E91CFED97BFA7B850

#include <cstddef>
#include <vector>
#include <utility>
#include <functional>
#include "../support/debug.hpp"
#include "../support/hash.hpp"
#include "../support/platform.hpp"
#include "../resman/scoped_array.hpp"

namespace gvl
{

struct equal_to
{
	template<typename A, typename B>
	bool operator()(A const& a, B const& b)
	{ return a == b; }
};

template<
	typename T,
	typename KeyT,
	typename ValueT,
	typename Hash = hash_functor,
	typename Compare = equal_to,
	bool SafeEmptyKey = false>
struct generic_hash_set_new : Hash, Compare
{
	typedef Compare compare_type;

	struct empty_tag_ {};

	struct range
	{
		range(T* begin_init, T* end_init)
		: cur(begin_init)
		, end(end_init)
		{
			while(cur != end && cur->is_empty())
				++cur;
		}

		bool empty()
		{ return cur == end; }

		void pop_front()
		{
			do
				++cur;
			while(cur != end && cur->is_empty());
		}

		ValueT& front()
		{ return cur->value(); }

		T* cur;
		T* end;
	};

	generic_hash_set_new(
		Hash const& hash = Hash(),
		Compare const& compare = Compare())
	: Hash(hash), Compare(compare)
	, table_(0)
	, second_(0)
	, h1p(0x883C1BEE)
	, h2p(0x672833C6)
	, owning_elements_(true)
	{
		reallocate_table_(4);
	}

	~generic_hash_set_new()
	{
		if(owning_elements_)
		{
			int size = table_size_();

			for(int i = 0; i < size; ++i)
				table_[i].make_empty();
		}

		delete[] table_;
	}

	// NOTE: This does not touch the elements currently in the table.
	// They are assumed to not be owned by this instance anymore.
	void reallocate_table_(int h1log2_new)
	{
		int new_size = (1<<h1log2_new) + (1<<(h1log2_new-1));
		scoped_array<T> new_table(new T[new_size]);
		delete[] table_;

		table_ = new_table.release();
		second_ = table_ + unsigned(1<<h1log2_new);
		h1log2 = h1log2_new;
		h2log2 = h1log2_new - 1;
		h1mask = (1<<h1log2)-1;
		h2mask = (1<<h2log2)-1;
		h1shift = hash_bits - h1log2;
		h2shift = hash_bits - h2log2;
	}

	template<typename SpecKeyT>
	T* lookup(SpecKeyT const& k)
	{
		hash_t h1v = h1(k) & h1mask;
		hash_t h2v = h2(k) & h2mask;

		T& slot1 = table_[h1v];
		T& slot2 = second_[h2v];

		if(!slot1.is_empty() && Compare::operator()(slot1.key(), k))
			return &slot1;
		if(!slot2.is_empty() && Compare::operator()(slot2.key(), k))
			return &slot2;
		return 0;
	}

	void insert(T const& v)
	{
		KeyT const& k = v.key();

		if(T* existing_slot = lookup(k))
		{
			existing_slot->assign_value(v);
			return;
		}

		hash_t h1v = h1(k) & h1mask;
		T& slot = table_[h1v];
		if(slot.is_empty())
			slot = v;
		else
			cuckoo_(v, &slot);
	}

	bool try_insert(T& v)
	{
		KeyT const& k = v.key();

		hash_t h1v = h1(k) & h1mask;
		T& slot = table_[h1v];
		if(slot.is_empty())
			slot = v;
		else
			return try_cuckoo_(v, &slot);
		return true;
	}

	void cuckoo_(T old1, T* slot)
	{
		if(!try_cuckoo_(old1, slot))
		{
			resize_();
			insert(old1);
		}
	}

	bool try_cuckoo_(T& old1, T* slot)
	{
		for(int i = 0; i < 16; ++i)
		{
			T old2 = GVL_MOVE(*slot);
			*slot = GVL_MOVE(old1);

			hash_t h2v = h2(old2.key()) & h2mask;
			slot = &second_[h2v];

			if(slot->is_empty())
			{
				*slot = GVL_MOVE(old2);
				return true;
			}

			old1 = GVL_MOVE(*slot);
			*slot = GVL_MOVE(old2);

			hash_t h1v = h1(old1.key()) & h1mask;
			slot = &table_[h1v];

			if(slot->is_empty())
			{
				*slot = GVL_MOVE(old1);
				return true;
			}
		}

		return false;
	}

	void resize_()
	{
		int old_size = table_size_();

		generic_hash_set_new new_set((empty_tag_()));
		new_set.owning_elements_ = false; // new_set does not own the elements until it's final
		new_set.new_hashes_(h1p, h2p);

		int new_h1log2 = h1log2;

	retry:
		++new_h1log2;
		new_set.new_hashes_(new_set.h1p, new_set.h2p);
		new_set.reallocate_table_(new_h1log2);

		for(int i = 0; i < old_size; ++i)
		{
			T& slot = table_[i];
			if(!slot.is_empty())
			{
				// try_insert may change the parameter passed to it if it fails, so we
				// copy the slot to a temporary variable.
				T temp_slot(slot);
				if(!new_set.try_insert(temp_slot))
					goto retry;
			}
		}

		// new_set owns the elements now
		new_set.owning_elements_ = true;
		owning_elements_ = false;
		swap(new_set);
	}

	void new_hashes_(unsigned h1p_old, unsigned h2p_old)
	{
		h1p = (h1p_old + 1) * 2654435761;
		h2p = (h2p_old - 1) * 2654435761;
	}

	template<typename SpecKeyT>
	hash_t h1(SpecKeyT const& k)
	{
		hash_t h = Hash::operator()(k, h1p);
		h ^= h >> h1shift;
		return h;
	}

	template<typename SpecKeyT>
	hash_t h2(SpecKeyT const& k)
	{
		hash_t h = Hash::operator()(k, h2p);
		h ^= h >> h2shift;
		return h;
	}

	int table_size_()
	{ return (1<<h1log2) + (1<<h2log2); }

	void swap(generic_hash_set_new& other)
	{
		std::swap(table_, other.table_);
		std::swap(second_, other.second_);
		std::swap(h1p, other.h1p);
		std::swap(h2p, other.h2p);
		std::swap(h1mask, other.h1mask);
		std::swap(h2mask, other.h2mask);
		std::swap(h1log2, other.h1log2);
		std::swap(h2log2, other.h2log2);
		std::swap(h1shift, other.h1shift);
		std::swap(h2shift, other.h2shift);
		std::swap(owning_elements_, other.owning_elements_);
	}

	range all()
	{ return range(table_, table_ + table_size()); }

private:
	generic_hash_set_new(empty_tag_)
	: table_(0)
	, second_(0)
	, h1p(0x883C1BEE)
	, h2p(0x672833C6)
	, owning_elements_(false)
	{
	}

	T* table_;
	T* second_;

	unsigned h1p, h2p;
	hash_t h1mask, h2mask;
	int h1log2, h2log2;
	int h1shift, h2shift;
	bool owning_elements_;
};

} // namespace gvl

#endif // UUID_115639EF65E5430E91CFED97BFA7B850
