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

#include "../../test/test.hpp"

#include "../pairing_heap.hpp"
#include "../../math/tt800.hpp"
#include "../../support/algorithm.hpp"
#include <functional>
#include <cmath>
#include <set>
#include <ctime>
#include <algorithm>
#include <set>
#include <queue>

#include "../../tut/quickcheck/context.hpp"
#include "../../tut/quickcheck/generator.hpp"
#include "../../tut/quickcheck/property.hpp"

#include "../../support/log.hpp"
#include "../../support/profile.hpp"

struct pairing_heap_integer : gvl::pairing_node<>
{
	pairing_heap_integer(int v)
	: v(v)
	{
	}
	
	bool operator<(pairing_heap_integer const& b) const
	{
		return v < b.v;
	}
	
	int v;
};

typedef gvl::pairing_heap<pairing_heap_integer> h_t;

#define TANDEM(x, op) do { (x)->first.op; (x)->second.op; } while(0)

template<typename T>
struct heap_model
{
	struct ptr_comp
	{
		bool operator()(T* a, T* b) const
		{
			return *a < *b;
		}
	};
	
	void insert(T* x)
	{ elements.insert(x); }
	
	void meld(heap_model& b)
	{
		elements.insert(b.elements.begin(), b.elements.end());
		b.elements.clear();
	}
	
	void erase_min()
	{
		delete *elements.begin();
		unlink_min();
	}
	
	void unlink_min()
	{ elements.erase(elements.begin());	}
	
	void unlink_all()
	{ elements.clear();	}
	
	T& min()
	{ return **elements.begin(); }
	
	std::size_t size() const { return elements.size(); }
	bool empty() const { return elements.empty(); }
	
	std::multiset<T*, ptr_comp> elements;
};

typedef std::pair<h_t, heap_model<pairing_heap_integer> > test_type;

QC_BEGIN_GEN(empty_heap_gen, test_type)
	TLOG("Empty");
	return ptr_t(new t);
QC_END_GEN()

QC_BEGIN_GEN(singleton_heap_gen, test_type)
	ptr_t ret(new t);
	int v = ctx.rand(10000);
	TANDEM(ret, insert(new pairing_heap_integer(v)));
	TLOG("Singleton(" << v << ")");
	return ret;
QC_END_GEN()

QC_BEGIN_GEN(merge_heap_gen, test_type)
	if(ctx.generator_depth() > 10)
	{
		return ctx.generate<t>("singleton");
	}
	TLOG("Meld(");
	ptr_t a(ctx.generate_any<t>());
	TLOG(", ");
	ptr_t b(ctx.generate_any<t>());
	TLOG(")");
	
	/*
	std::cout << "\n\n";
	a->first.print_tree();
	std::cout << "U\n";
	b->first.print_tree();
	std::cout << "=\n";
	*/
	a->first.meld(b->first);
	a->second.meld(b->second);
	/*
	a->first.print_tree();
	std::cout << "\n\n";
	*/
	return a;
QC_END_GEN()

QC_BEGIN_GEN(erase_min_heap_gen, test_type)
	TLOG("EraseMin(");
	ptr_t a(ctx.generate_any<t>());
	TLOG(")");
	if(!a->first.empty())
		a->first.erase_min();
	if(!a->second.empty())
		a->second.erase_min();
	return a;
QC_END_GEN()

QC_BEGIN_GEN(insert_heap_gen, test_type)
	TLOG("Insert(");
	ptr_t a(ctx.generate_any<t>());
	
	int v = ctx.rand(10000);
	TLOG(", " << v << ")");
	TANDEM(a, insert(new pairing_heap_integer(v)));
	return a;
QC_END_GEN()


QC_BEGIN_PROP(heap_integrity_property, test_type)
	gvl::qc::chk_result check(gvl::qc::context& ctx, QC_GEN_ANY(t, obj))
	{
		TLOG(std::endl << "=== heap_integrity_property ===");
		
		while(!obj->first.empty() && !obj->second.empty())
		{
			int a = obj->first.min().v;
			int b = obj->second.min().v;

			QC_ASSERT("elements are the same", a == b);
			obj->first.erase_min();
			obj->second.erase_min();
		}
		
		QC_ASSERT("both empty", obj->first.empty() && obj->second.empty());
		return gvl::qc::chk_ok;
	}
QC_END_PROP()

/*
template<typename T>
struct greater : std::binary_function<T const&, T const&, bool>
{
	bool operator()(T const& a, T const& b) const
	{
		return b < a;
	}
};*/

GVLTEST_SUITE(gvl, pairing_heap)

GVLTEST(gvl, pairing_heap, push_pop)
{
	h_t heap;
	
	int const count = 1000;
	std::vector<int> added;
	gvl::tt800 rand(1);
	
	for(int i = 0; i < count; ++i)
	{
		int v = rand.range(0, 10000);
		
		added.push_back(v);
		heap.insert(new pairing_heap_integer(v));
	}
	
	std::sort(added.begin(), added.end());
	
	for(int i = 0; i < count; ++i)
	{
		ASSERT("not empty", !heap.empty());
		ASSERT("all items are present and popped in the right order", added[i] == heap.min().v);
		heap.erase_min();
	}
}

GVLTEST(gvl, pairing_heap, quickcheck)
{
	gvl::qc::context ctx;
	ctx.add("singleton", new singleton_heap_gen, 0.5);
	ctx.add("merge", new merge_heap_gen);
	ctx.add("insert", new insert_heap_gen, 1.5);
	ctx.add("empty", new empty_heap_gen, 0.2);
	ctx.add("erase_min", new erase_min_heap_gen, 0.4);
	
	gvl::qc::test_property<heap_integrity_property>(ctx, 1000, 500);
}

GVLTEST(gvl, pairing_heap, profiling)
{
#if GVL_PROFILE
	std::vector<pairing_heap_integer> added;
	gvl::mwc rand;
	rand.seed(1);
	
	int const count = 500000;
	
	for(int i = 0; i < count; ++i)
	{
		int v = rand(0, 10000);
		
		added.push_back(pairing_heap_integer(v));
	}
	
	{
		h_t heap;
		
		GVL_PROF_TIMER("pairing heap");
		for(int i = 0; i < 20; ++i)
		{	
			for(std::size_t i = 0; i < added.size(); ++i)
			{
				heap.insert(&added[i]);
			}
			
			for(int i = 0; i < 200; ++i)
			//while(!heap.empty())
			{
				heap.unlink_min();
				//heap.erase_min();
			}
			
			heap.unlink_all();
		}
	}
	
	{
		std::priority_queue<int> heap;
		
		GVL_PROF_TIMER("binary heap");
		for(int i = 0; i < 20; ++i)
		{	
			
			for(std::size_t i = 0; i < added.size(); ++i)
			{
				heap.push(added[i].v);
			}
			
			for(int i = 0; i < 200; ++i)
			//while(!heap.empty())
			{
				heap.pop();
			}
		}
	}
	
#endif
}
