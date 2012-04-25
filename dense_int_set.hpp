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

#ifndef UUID_C3EF93888B9C481B892430BE338FAC14
#define UUID_C3EF93888B9C481B892430BE338FAC14

#include "support/cstdint.hpp"
#include "support/debug.hpp"
#include "support/opt.hpp"
#include "meta/static_log2.hpp"

namespace gvl
{

template<typename T = uint32_t, T Min = T(0), T Max = T(0)-1, typename MapT = uint_fast32_t>
struct dense_int_set
{
	static int const branch_shift = static_log2<GVL_BITS_IN(MapT)>::value;
	static int const branch_factor = 1 << branch_shift;
	static T const branch_mask = ~(T(branch_factor) - 1);

	struct node
	{
		node(
			MapT map_init,
			node* parent_init,
			T begin_init,
			int level_shift_init)
		: map(map_init)
		, parent(parent_init)
		, begin(begin_init)
		, end(begin + (T(1) << (level_shift_init + branch_shift)) - 1)
		, level_mask(~((T(1) << level_shift_init) - 1))
		, level_shift(level_shift_init)
		{

		}

		MapT map;
		node* parent;
		T begin, end;
		T level_mask; // ~((1 << level_shift) - 1)
		int level_shift;

		union child
		{
			node* n;
			MapT map;
		};

		bool is_bottom()
		{ return level_shift == branch_shift; }

		child children[branch_factor];
	};

	dense_int_set()
	: root(0)
	{
	}

	void expand_to_fit_(T v, node*& n)
	{
		if(v < n->begin || v > n->end)
		{
			int level_shift_new = n->level_shift;
			int sh;
			T begin_new = n->begin;

			do
			{
				level_shift_new = level_shift_new + branch_shift;
				sh = level_shift_new + branch_shift;
				begin_new = (begin_new >> sh) << sh;
			}
			while(v < begin_new || v > begin_new + (T(1) << sh) - 1)

			int idx = ((n->begin - begin_new) >> level_shift_new);
			node* n_new = new node(MapT(1) << idx, 0, begin_new, level_shift_new);

			GVL_SASSERT(false == n_new->is_bottom());

			n_new->children[idx].n = root;
			n->parent = n_new;
			n = n_new;
		}
	}

	void insert(T v)
	{
		if(!root)
			return insert_root_(v);

		insert_(v, root);
	}

	int count(T v)
	{
		if(!root)
			return 0;

		return count_(v, root_begin, root_width, root_level, root);
	}

private:

	int count_(T v, node* n)
	{
		do
		{
			if(v < begin || v >= begin + width)
				return 0;

			if(level == 0)
				return (n->map >> (v - begin)) & 1;

			inner_node* in = static_cast<inner_node*>(n);

			int idx = ((v - begin) >> level_shift);
			sassert(idx >= 0 && idx < branch_factor);

			// Descend
			begin += idx << level_shift;
			width >>= branch_shift;
			level_shift -= branch_shift;
			--level;

			n = in->children[idx];
		}
		while(true);
	}

	void erase_(T v, T begin, T width, int level, int level_shift, node* n)
	{
		do
		{
			if(v < begin || v >= begin + width)
				return;

			if(level == 0)
			{
				n->map &= ~(MapT(1) << (v - begin));

				// Remove nodes as long as they're empty
				while(!n->map)
				{
					// Ascend
					level_shift += branch_shift;
					int sh = level_shift + branch_shift;
					begin = (begin >> sh) << sh;
					width <<= branch_shift;
					++level;
					n = n->parent;
					int idx = ((v - begin) >> level_shift) << level_shift;
					sassert(idx >= 0 && idx < branch_factor);

					inner_node* in = static_cast<inner_node*>(n);

					n->map &= ~(MapT(1) << idx);

					sassert(in->children[idx]);
					delete in->children[idx];
					in->children[idx] = 0;
				}
				return;
			}

			inner_node* in = static_cast<inner_node*>(n);

			int idx = ((v - begin) >> level_shift);
			sassert(idx >= 0 && idx < branch_factor);

			// Descend
			begin += idx << level_shift;
			width >>= branch_shift;
			level_shift -= branch_shift;
			--level;

			n = in->children[idx];
		}
		while(true);
	}

	// Insert into or under a node.
	// Assumes the node covers v.
	void insert_(T v, T begin, T width, int level, int level_shift, node** np)
	{
		do
		{
			expand_to_fit_(v, *np);

			node* n = *np;

			sassert(v >= n->begin && v < n->end);
			sassert(n->end - n->begin == 1 << (n->level_shift + branch_shift));

			if(n->is_bottom())
			{
				// Reached bottom, set the bit in the map
				T offset = (v - begin);
				int child_n = (offset >> branch_shift);
				n->map |= MapT(1) << child_n;
				n->children[child_n].map |= (MapT(1) << (offset & branch_mask));
				return;
			}

			int idx = (v - begin) >> n->level_shift;
			sassert(idx >= 0 && idx < branch_factor);

			// width is always branch_factor^(level + 2)
			// See sassert at the top.

			node*& child = n->children[idx].n;
			if(((n->map >> idx) & 1) == 0)
			{
				// Have to create child
				int level_shift_new = n->level_shift + branch_shift;
				int sh = level_shift_new + branch_shift;
				T begin_new = n->begin + (idx << n->level_shift);


				child = new node(0, n, begin_new, n->level_shift - branch_shift);

				// Set occupied bit.
				// NOTE: This is done after creating the node,
				// so that if that fails, the bit is not set.
				n->map |= MapT(1) << idx;
			}

			// Go to the new node and repeat
			n = child;
		}
		while(true);
	}

	// Insert as root.
	// Assumes there's no root.
	void insert_root_(T v)
	{
		GVL_ASSUME(!root);
		root_level = 0;
		root_level_shift = 0;
		root_begin = (v >> branch_shift) << branch_shift;
		root_width = branch_factor;
		root = construct_bottom_node_(v, root_begin);
	}

	node* construct_bottom_node_(T v, T begin)
	{
		MapT map = MapT(1) << (v - begin);
		return new node(map);
	}

	/*
	inner_node* construct_inner_node_(int occupied_idx)
	{
		MapT map = MapT(1) << occupied_idx;
		return new inner_node(map);
	}*/


	node* root;

	// These are only defined when root != 0
	T root_begin, root_width; // Range of root
	int root_level, root_level_shift;
};

}

#endif // UUID_C3EF93888B9C481B892430BE338FAC14
