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

#ifndef UUID_DE2C7B4CE76345E6A69E1F9BBB7FC7DB
#define UUID_DE2C7B4CE76345E6A69E1F9BBB7FC7DB

#error Total mishmash

#include "node_base.hpp"
#include <climits>
#include <cstddef>
#include <algorithm>
#include "function.hpp"
#include "debug.hpp"

namespace vl
{


struct avl_node_common
{
	avl_node_common* left;
	avl_node_common* right;
	bool threads[2];
	signed char balance;
};

template<typename DerivedT>
struct avl_node : avl_node_common
{

};

template<typename T, typename Compare = std::less<T>, typename Deleter = default_delete>
struct avl_tree : Compare
{
	static T* downcast(avl_node_common* p)
	{
		return static_cast<T*>(static_cast<avl_node<T>*>(p));
	}

	static avl_node<T>* upcast(T* p)
	{
		return static_cast<avl_node<T>*>(p);
	}

	avl_tree()
	: n(0)
	, root(0)
	{
	}

	bool empty() const
	{
		return n == 0;
	}

	void swap(avl_tree& b)
	{
		std::swap(n, b.n);
		std::swap(root, b.root);
	}

	void insert(T* el_)
	{
		fib_node_common* el = upcast(el_);

		el->degree = 0;
		el->parent = 0;
		el->child = 0;
		el->left = el;
		el->right = el;
		el->mark = false;

		root_list_insert_(el);

		if(!pmin || Compare::operator()(*el_, *downcast(pmin)))
			pmin = el; // A new smallest

		++n;
	}

	// NOTE: It's assumed that the key was decreased
	// right before this was called.
	// One could make a debug check that makes sure the
	// key wasn't increased beyond it's children, but
	// it would affect the complexity of the operation.
	void decreased_key(T* el_)
	{
		fib_node_common* el = upcast(el_);

		fib_node_common* elp = el->parent;

		if(elp && Compare::operator()(*el_, *downcast(elp)))
		{
			cut_(el, elp);
			cascading_cut_(elp);
		}

		if(Compare::operator()(*el_, *downcast(pmin)))
		{
			pmin = el;
		}
	}

	void unlink_min()
	{
		passert(pmin, "Empty heap");

		fib_node_common* el = pmin;

		fib_node_common* x = el->child;
		if(x)
		{
			fib_node_common* orig = x;
			do
			{
				fib_node_common* next = x->right;
				x->parent = 0;
				root_list_insert_(x);
				x = next;
			}
			while(x != orig);
		}

		root_list_remove_(el);
		--n;

		if(n == 0)
			pmin = 0;
		else
		{
			//pmin = el->right; // Why is this done? pmin is both incorrect and overwritten in consolidate_
			consolidate_();
		}
	}

	void unlink(T* el_)
	{
		fib_node_common* el = upcast(el_);

		force_to_min_(el);
		unlink_min();
	}

	void erase_min()
	{
		fib_node_common* old = pmin;
		unlink_min();
		Deleter::run(old);
	}

	void erase(T* el)
	{
		unlink(el);
		Deleter::run(el);
	}

	T& min()
	{
		passert(pmin, "Empty heap");
		return *downcast(pmin);
	}

private:

	// Force el to become the smallest element by
	// pretending it compares smaller to all other elements.
	// cut_ and cascading_cut_ do not do any comparisons, so it's fine.
	void force_to_min_(fib_node_common* el)
	{
		fib_node_common* elp = el->parent;

		if(elp)
		{
			cut_(el, elp);
			cascading_cut_(elp);
		}

		pmin = el;
	}

	void root_list_insert_(fib_node_common* el)
	{
		if(!root)
		{
			root = el;
			el->left = el;
			el->right = el;
		}
		else
			insert_after_(root, el);
	}

	void root_list_remove_(fib_node_common* el)
	{
		if(el->left == el)
			root = 0;
		else
			root = remove_(el);
	}

	void insert_after_(fib_node_common* a, fib_node_common* b)
	{
		fib_node_common* old_a_right = a->right;
		b->right = old_a_right;
		old_a_right->left = b;
		a->right = b;
		b->left = a;
	}

	void insert_before_(fib_node_common* a, fib_node_common* b)
	{
		fib_node_common* old_a_left = a->left;
		b->right = a;
		a->left = b;
		old_a_left->right = b;
		b->left = old_a_left;
	}

	fib_node_common* remove_(fib_node_common* el)
	{
		fib_node_common* next = 0;
		if(el->left != el)
			next = el->left;

		if(el->parent && el->parent->child == el)
			el->parent->child = next;

		fib_node_common* right = el->right;
		fib_node_common* left = el->left;
		right->left = left;
		left->right = right;

		// NOTE: Do we need to do this?
		el->parent = 0;
		el->left = el;
		el->right = el;

		return next;
	}

	void consolidate_()
	{
		std::size_t const max_n_lg2 = CHAR_BIT * sizeof(std::size_t);
		fib_node_common* a[max_n_lg2] = {};

		// TODO: We don't need to do root_list_remove_ for each, we could
		// just iterate through them all, then set root = 0

		while(fib_node_common* w = root)
		{
			fib_node_common* x = w;
			root_list_remove_(w);
			std::size_t d = x->degree;

			sassert(d < max_n_lg2);
			while(fib_node_common* y = a[d])
			{
				if(Compare::operator()(*downcast(y), *downcast(x)))
					std::swap(x, y);
				link_(y, x);
				a[d] = 0;
				++d;
				sassert(d < max_n_lg2);
			}
			a[d] = x;
		}

		pmin = 0;

		for(std::size_t i = 0; i < max_n_lg2; ++i)
		{
			if(a[i])
			{
				root_list_insert_(a[i]);
				if(!pmin || Compare::operator()(*downcast(a[i]), *downcast(pmin)))
					pmin = a[i];
			}
		}
	}

	void link_(fib_node_common* y, fib_node_common* x)
	{
		if(!x->child)
			x->child = y;
		else
			insert_before_(x->child, y);
		y->parent = x;
		++x->degree;
		y->mark = false;
	}

	void cut_(fib_node_common* a, fib_node_common* b)
	{
		remove_(a);
		--b->degree;
		root_list_insert_(a);

		a->parent = 0;
		a->mark = false;
	}

	void cascading_cut_(fib_node_common* el)
	{
		while(fib_node_common* elp = el->parent)
		{
			if(!el->mark)
			{
				el->mark = true;
				return;
			}
			else
			{
				cut_(el, elp);
				el = elp;
			}
		}
	}

	void clean_()
	{
		n = 0;
		pmin = 0;
		root = 0;
	}

	std::size_t n;
	fib_node_common* pmin;
	fib_node_common* root;
};

}

#endif // UUID_DE2C7B4CE76345E6A69E1F9BBB7FC7DB
