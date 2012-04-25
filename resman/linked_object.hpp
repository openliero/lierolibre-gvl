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

#ifndef UUID_E2DA39C2967941F4DE389AB24A11196D
#define UUID_E2DA39C2967941F4DE389AB24A11196D

#include "../support/move.hpp"
#include "../support/debug.hpp"

namespace gvl
{

struct linked_object_common
{
	linked_object_common()
	{
		// Assigned in body to avoid warnings
		prev = this;
		next = this;
	}

	linked_object_common(linked_object_common const& other)
	: prev(&other)
	, next(other.next)
	{
		other.next = this;
		next->prev = this;
	}

	linked_object_common(move_holder<linked_object_common> other)
	{
		// Correct neighbours first, otherwise we get the incorrect prev/next in case other->next == &*other
		other->prev->next = this;
		other->next->prev = this;

		// NOTE: prev/next of other may have changed!
		prev = other->prev;
		next = other->next;

		// Make unique
		other->prev = &*other;
		other->next = &*other;
	}

	linked_object_common& operator=(linked_object_common const& other)
	{
		unlink();
		link(other);
		return *this;
	}

	bool unique()
	{
		return next == this;
	}

	void make_unique()
	{
		unlink();
		next = this;
		prev = this;
	}

	virtual ~linked_object_common()
	{
		next->prev = prev;
		prev->next = next;
	}

	// TODO: Make these protected by making linked_iterator
	// a friend (in some way)
	mutable linked_object_common const* prev;
	mutable linked_object_common const* next;

protected:
	void link(linked_object_common const& other)
	{
		prev = &other;
		next = other.next;
		other.next = this;
		next->prev = this;
	}

	void unlink()
	{
		next->prev = prev;
		prev->next = next;
	}


};

template<typename DerivedT>
struct linked_object : protected linked_object_common
{
	struct linked_iterator : std::iterator<std::bidirectional_iterator_tag, DerivedT>
	{
		friend struct linked_object;

		linked_iterator()
		{
		}

		DerivedT& operator*()
		{ return *static_cast<DerivedT*>(p); }

		DerivedT* operator->()
		{ return static_cast<DerivedT*>(p); }

		linked_iterator& operator++()
		{
			p = const_cast<linked_object_common*>(p->next);
			return *this;
		}

		linked_iterator operator++(int)
		{
			linked_iterator ret(*this);
			++ret;
			return ret;
		}

		linked_iterator& operator--()
		{
			p = const_cast<linked_object_common*>(p->prev);
			return *this;
		}

		linked_iterator operator--(int)
		{
			linked_iterator ret(*this);
			--ret;
			return ret;
		}

		bool operator==(linked_iterator rhs) const
		{ return p == rhs.p; }
		bool operator!=(linked_iterator rhs) const
		{ return p != rhs.p; }

	private:
		linked_iterator(linked_object_common* p)
		: p(p)
		{
		}

		linked_object_common* p;
	};

	linked_object()
	{
	}

	// protected inheritance to make sure only the correct type is copied
	linked_object(linked_object const& other)
	: linked_object_common(other)
	{
	}

	linked_object(move_holder<linked_object> other)
	: linked_object_common(move<linked_object_common>(other))
	{
	}

	linked_object& operator=(linked_object const& other)
	{
		linked_object_common::operator=(other);
		return *this;
	}

	linked_iterator linked_begin()
	{ return linked_iterator(const_cast<linked_object_common*>(next)); }

	linked_iterator linked_end()
	{ return linked_iterator(this); }
};

} // namespace gvl

#endif // UUID_E2DA39C2967941F4DE389AB24A11196D
