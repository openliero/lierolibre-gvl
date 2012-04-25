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

#ifndef UUID_080B6EF700D0430C6B7571952F9E9E99
#define UUID_080B6EF700D0430C6B7571952F9E9E99

#include <cstddef>
#include <vector>
#include <utility>
#include <functional>
#include "../support/debug.hpp"
#include "../support/hash.hpp"

namespace gvl
{

/*
Generic hash set

generic_hash_set<T, KeyT, Hash, Compare>

Let t : generic_hash_set<T, KeyT, Hash, Compare>
    a, b : T

Requirements:

Hash is a function of type (KeyT -> integer in 0..MaxInt).
Compare is a transitive, reflexive, symmetrical, binary relation on KeyT.

T::key : T -> KeyT
T::is_empty : T -> Boolean
T::is_filled : T -> Boolean
T::make_empty : T -> Void
T::make_deleted : T -> Void
T::assign_value : T T -> Void
T::value : T -> ValueT

a.is_empty() iff 'a' does not have a key, nor value.
a.is_filled() iff 'a' does have a key and/or value.
(T()).is_empty() == true
a.is_empty() => !a.is_filled()
a.is_filled() => !a.is_empty()

a.assign_value(b) assigns the value part of 'b' to 'a' (if any). A precondition
	is that Compare(a.key(), b.key()) == true, so the function can assign
	the key part as well without breaking the semantics.

a.make_empty():
	Precond:   a.is_filled() && !a.is_empty()
	Postcond: !a.is_filled() &&  a.is_empty()

a.make_deleted():
	Precond:   a.is_filled() && !a.is_empty()
	Postcond: !a.is_filled() && !a.is_empty()

Compare(a.key(), b.key()) == true iff the keys of 'a' and 'b' are equal

Compare(a.key(), b.key()) == true => Hash(a.key()) == Hash(b.key())

If SafeEmptyKey == true, then // NOTE: This is not made use of
	a.is_empty() && !b.is_empty() => Compare(a.key(), b.key()) == false



Operations:


*/

template<int ID, typename BaseT>
struct base_select : BaseT
{
	base_select(BaseT const& b)
	: BaseT(b) {}
};

template<typename T>
inline T* dummy_ptr()
{
	return reinterpret_cast<T*>(1);
}

template<typename T>
inline bool is_null_or_dummy_ptr(T* p)
{
	return p <= dummy_ptr<T>(); // Assumes dummy_ptr is 1
}

template<typename T, typename KeyT, typename ValueT, typename Hash = hash_functor, typename Compare = std::equal_to<KeyT>, bool SafeEmptyKey = false>
struct generic_hash_set : Hash, Compare
{
	typedef Compare compare_type;

	// NOTE: Compare can't be the same type as Hash. This is a (minor) semantics issue.

	//typedef T index_type;

	struct iterator
	{
		typedef std::ptrdiff_t difference_type;
		typedef std::forward_iterator_tag iterator_category;
		typedef ValueT* pointer;
		typedef ValueT& reference;
		typedef ValueT value_type;

		struct found_tag {};

		iterator(T* p, T* end)
		: p(p), end(end)
		{
			find();
		}

		iterator(T* p, T* end, found_tag)
		: p(p), end(end)
		{
			sassert(p == end || p->is_filled());
		}

		ValueT& operator*() const
		{
			return p->value();
		}

		ValueT* operator->()
		{
			return &p->value();
		}

		KeyT& key() const
		{
			return p->key();
		}

		iterator& operator++()
		{
			++p;
			find();
			return *this;
		}

		bool operator==(iterator b) const
		{
			return p == b.p && end == b.end;
		}

		bool operator!=(iterator b) const
		{
			return !operator==(b);
		}

	private:
		void find()
		{
			while(p != end && !p->is_filled())
			{
				++p;
			}
		}

		T* p;
		T* end;
	};

	generic_hash_set(
		Hash const& hash = Hash(),
		Compare const& compare = Compare())
	: Hash(hash), Compare(compare)
	, t(11), tsize(11)
	, elems(0)
	, deleted(0)
	, uload(0.666)
	, lload(0.20)
	, shload(0.45) // 2*lload < shload < uload
	{
	}

	~generic_hash_set()
	{
		clear();
	}

	void clear()
	{
		for(std::size_t i = 0; i < tsize; ++i)
		{
			if(t[i].is_filled())
				t[i].make_empty();
			else if(!t[i].is_empty())
				t[i] = T();
		}
		elems = 0;
	}

	template<typename SpecKeyT>
	T* lookup(SpecKeyT const& k)
	{
		std::size_t h = Hash::operator()(k);

		int step = 1;
		do
		{
			T& slot = t[h % tsize];

			if(slot.is_empty())
				return 0;
			if(slot.is_filled() && Compare::operator()(slot.key(), k))
				return &slot;

			h += step; step += 2;
		}
		while(step < 1024);

		return 0;
	}

	template<typename SpecKeyT>
	iterator find(SpecKeyT const& k)
	{
		T* p = lookup(k);
		T* end = &t[0] + tsize;
		if(p)
			return iterator(p, end, iterator::found_tag());
		else
			return iterator(end, end, iterator::found_tag());
	}

	bool try_insert(T const& v)
	{
		std::size_t h = Hash::operator()(v.key());

		std::size_t step = 1;
		do
		{
			T& slot = t[h % tsize];

			if(!slot.is_filled())
			{
				if(!slot.is_empty())
					--deleted;
				++elems;
				slot = v;
				sassert(slot.is_filled());
				return true;
			}

			h += step; step += 2;
		}
		while(step < 1024);

		return false;
	}

	void insert(T const& v)
	{
		maybe_enlarge();

		T* idx = lookup(v.key());
		if(idx)
			idx->assign_value(v);
		else
		{
			while(!try_insert(v))
				enlarge();
		}
	}

	template<typename SpecKeyT>
	void remove(SpecKeyT const& k)
	{
		T* idx = lookup(k);

		if(idx)
		{
			sassert(idx->is_filled());
			idx->make_deleted();
			sassert(!idx->is_filled());
			--elems;
			++deleted;
			maybe_shrink();
		}
	}

	void erase(iterator i)
	{
		passert(
			i.p >= &t[0]
			&& i.p < &t[0] + tsize
			&& i.p->is_filled(), "Invalid iterator");

		i.p->make_deleted();
		--elems;
		++deleted;
	}

	template<typename Pred>
	void erase_if(Pred pred = Pred())
	{
		for(std::size_t i = 0; i < t.size(); ++i)
		{
			if(t[i].is_filled()
			&& pred(t[i].value()))
			{
				t[i].make_deleted();
				--elems;
				++deleted;
			}
		}

		maybe_shrink();
	}

	std::size_t size() const
	{
		return elems;
	}

	iterator begin()
	{
		T* p = &t[0];
		return iterator(p, p + tsize);
	}

	iterator end()
	{
		T* p = &t[0];
		return iterator(p + tsize, p + tsize);
	}

protected:
	void rehash(std::vector<T>& e)
	{
		deleted = 0; // There will be no deleted elements afterwards
try_insert:
		elems = 0;
		for(std::size_t i = 0; i < e.size(); ++i)
		{
			if(e[i].is_filled()
			&& !try_insert(e[i]))
			{
				// We need to make the table larger

				tsize = next_prime(tsize + 1);

				t.assign(tsize, T());

				goto try_insert;
			}
		}
	}

	void maybe_enlarge()
	{
		if(elems >= std::size_t(tsize * uload))
		{
			enlarge();
		}
	}

	void maybe_shrink()
	{
		if(elems <= std::size_t(tsize * lload)
		&& tsize > 11)
		{
			shrink();
		}
	}

	void enlarge()
	{
		tsize = next_prime(tsize + 1);

		std::vector<T> temp(tsize);

		t.swap(temp);

		// temp is now the old vector, reinsert all the old elements

		rehash(temp);
	}

	void shrink()
	{
		tsize = next_prime(std::size_t(elems / shload));

		std::vector<T> temp(tsize);

		t.swap(temp);

		// temp is now the old vector, reinsert all the old elements

		rehash(temp);
	}

	std::size_t tsize;
	std::vector<T> t;
	std::size_t elems;
	std::size_t deleted; // NOTE: Do we need to keep track of this?

	double uload; // Load limit where size is increased
	double lload; // Load limit where size is decreased
	double shload; // Approximate load after shrinking
};

}


#endif // UUID_080B6EF700D0430C6B7571952F9E9E99
