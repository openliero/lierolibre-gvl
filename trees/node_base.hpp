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

#ifndef UUID_B9454E4FCDB24701FB429EA10A6553B5
#define UUID_B9454E4FCDB24701FB429EA10A6553B5

#include "../support/debug.hpp"

namespace gvl
{

struct tree_node_multiway_common
{
private:
	typedef tree_node_multiway_common this_t;
public:
	tree_node_multiway_common()
	: prev(0)
	, first_child(0)
	, next_sibling(0)
	{
	}

	this_t* prev;
	this_t* first_child;
	this_t* next_sibling;

	bool is_first_child()
	{
		return prev->first_child == this;
	}

	bool is_root()
	{
		return !prev;
	}

	void unlink()
	{
		if(prev)
			update_prev(next_sibling);
		if(next_sibling)
			next_sibling->prev = prev;
	}

	void unlink_nonroot()
	{
		sassert(prev);
		update_prev(next_sibling);
		if(next_sibling)
			next_sibling->prev = prev;
	}

	void insert_before(this_t* n)
	{
		next_sibling = n;
		prev = n->prev;
		if(prev)
			n->update_prev(this);
		n->prev = this;
	}

	void insert_before_nonroot(this_t* n)
	{
		next_sibling = n;
		prev = n->prev;
		sassert(prev);
		n->update_prev(this);
		n->prev = this;
	}

	void insert_after(this_t* n)
	{
		next_sibling = n->next_sibling;
		prev = n;
		n->next_sibling->prev = this;
		n->next_sibling = this;
	}

	void insert_as_child(this_t* n)
	{
		prev = n;

		this_t* ch = n->first_child;
		next_sibling = ch;
		if(ch)
			ch->prev = this;
		n->first_child = this;
	}

private:
	void update_prev(this_t* n)
	{
		sassert(prev);
		if(is_first_child())
			prev->first_child = n;
		else
			prev->next_sibling = n;
	}
};

template<typename DerivedT>
struct tree_node_multiway : tree_node_multiway_common
{
private:
	typedef tree_node_multiway_common base_t;
	typedef DerivedT this_t;
public:

	void insert_before(this_t* n)
	{
		base_t::insert_before(n);
	}

	void insert_before_nonroot(this_t* n)
	{
		base_t::insert_before(n);
	}

	void insert_after(this_t* n)
	{
		base_t::insert_after(n);
	}

	void insert_as_child(this_t* n)
	{
		base_t::insert_as_child(n);
	}

	DerivedT* child() { return static_cast<DerivedT*>(first_child); }
	DerivedT* next() { return static_cast<DerivedT*>(next_sibling); }
};

// Same as tree_node_multiway_common, but without ability
// to iterate backwards (to previous siblings or parent).
// Should be slightly faster.
struct tree_node_multiway_forward_common
{
private:
	typedef tree_node_multiway_forward_common this_t;
public:
	this_t** prev_next;
	this_t* first_child;
	this_t* next_sibling;

	bool is_root()
	{
		return !prev_next;
	}

	void unlink()
	{
		if(next_sibling)
			next_sibling->prev_next = prev_next;
		if(prev_next)
			*prev_next = next_sibling;
	}

	void unlink_nonroot()
	{
		sassert(prev_next);
		if(next_sibling)
			next_sibling->prev_next = prev_next;
		*prev_next = next_sibling;
	}

	void insert_before(this_t* n)
	{
		next_sibling = n;
		prev_next = n->prev_next;
		if(prev_next)
			*prev_next = this;
		n->prev_next = &next_sibling;
	}

	void insert_before_nonroot(this_t* n)
	{
		next_sibling = n;
		prev_next = n->prev_next;
		sassert(prev_next);
		*prev_next = this;
		n->prev_next = &next_sibling;
	}

	void insert_after(this_t* n)
	{
		next_sibling = n->next_sibling;
		prev_next = &n->next_sibling;
		if(n->next_sibling)
			n->next_sibling->prev_next = &next_sibling;
		n->next_sibling = this;
	}

	void insert_as_child(this_t* n)
	{
		prev_next = &n->first_child;

		this_t* ch = n->first_child;
		next_sibling = ch;
		if(ch)
			ch->prev_next = &next_sibling;
		n->first_child = this;
	}
};

template<typename DerivedT>
struct tree_node_multiway_forward : tree_node_multiway_forward_common
{
private:
	typedef tree_node_multiway_forward_common base_t;
	typedef DerivedT this_t;
public:

	void insert_before(this_t* n)
	{
		base_t::insert_before(n);
	}

	void insert_before_nonroot(this_t* n)
	{
		base_t::insert_before_nonroot(n);
	}

	void insert_after(this_t* n)
	{
		base_t::insert_after(n);
	}

	void insert_as_child(this_t* n)
	{
		base_t::insert_as_child(n);
	}
};

template<typename DerivedT>
struct tree_node_binary
{
private:
	typedef tree_node_binary this_t;
public:
	DerivedT* left;
	DerivedT* right;
};

} // namespace gvl

#endif // UUID_B9454E4FCDB24701FB429EA10A6553B5
