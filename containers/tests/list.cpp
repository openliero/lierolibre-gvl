#include "../../test/test.hpp"

#include "../list.hpp"

#include "../../math/cmwc.hpp"
#include "../../support/algorithm.hpp"
#include "../../support/macros.hpp"
#include <functional>
#include <memory>
#include <algorithm>

#include "../../tut/quickcheck/context.hpp"
#include "../../tut/quickcheck/generator.hpp"
#include "../../tut/quickcheck/property.hpp"

struct tag1;
struct tag2;

struct integer
	: gvl::list_node<tag1>
	, gvl::list_node<tag2>
{
	integer(int v)
	: v(v)
	{
	}
	
	bool operator<(integer const& b) const
	{
		return v < b.v;
	}
	
	int v;
};

typedef gvl::list<integer, tag1> integer_list;

QC_BEGIN_GEN(empty_list_gen, integer_list)
	return ptr_t(new t);
QC_END_GEN()

QC_BEGIN_GEN(singleton_list_gen, integer_list)
	ptr_t ret(new t);
	ret->push_back(new integer(ctx.rand(10000)));
	return ret;
QC_END_GEN()

QC_BEGIN_GEN(concat_list_gen, integer_list)
	if(ctx.generator_depth() > 10)
	{
		return ctx.generate<t>("singleton");
	}
	ptr_t a(ctx.generate_any<t>());
	ptr_t b(ctx.generate_any<t>());
	a->splice(*b);
	return a;
QC_END_GEN()

QC_BEGIN_GEN(erase_list_gen, integer_list)
	ptr_t a(ctx.generate_any<t>());
	if(!a->empty())
		a->pop_front();
	return a;
QC_END_GEN()

QC_BEGIN_GEN(sorted_list_gen, integer_list)
	ptr_t a(ctx.generate_any<t>());
	a->sort(std::less<integer>());
	return a;
QC_END_GEN()

QC_BEGIN_PROP(list_integrity_property, integer_list)
	gvl::qc::chk_result check(gvl::qc::context& ctx, QC_GEN_ANY(t, obj))
	{
		obj->integrity_check();
		return gvl::qc::chk_ok_reuse;
	}
QC_END_PROP()

QC_BEGIN_GENERIC_PROP(list_pop_front_property)
	gvl::qc::chk_result check(gvl::qc::context& ctx, QC_GEN_ANY(t, obj))
	{
		if(obj->empty())
			return gvl::qc::chk_not_applicable;
			
		std::size_t before_count = obj->size();
		obj->pop_front();
		std::size_t after_count = obj->size();
		QC_ASSERT("pop_front decrease count with 1", after_count + 1 == before_count);
		return gvl::qc::chk_ok_reuse;
	}
QC_END_PROP()

template<typename T>
struct greater : std::binary_function<T const&, T const&, bool>
{
	bool operator()(T const& a, T const& b) const
	{
		return b < a;
	}
};

GVLTEST_SUITE(gvl, list)

GVLTEST(gvl, list, insert_delete_sort)
{
	typedef gvl::list<integer, tag1> l1_t;
	typedef gvl::list<integer, tag2, gvl::dummy_delete> l2_t;
	l1_t l1;
	l2_t l2;
	
	gvl::mwc r(1234);
	
	for(int repeat = 0; repeat < 100; ++repeat)
	{
		std::size_t l1_count = 0;
		std::size_t l2_count = 0;
		
		int count = r(0, 200);
		
		for(int i = 0; i < count; ++i)
		{
			integer* o = new integer(r(0, 10000));
			l1.push_back(o);
			l2.push_back(o);
			++l1_count;
			++l2_count;
		}
		
		ASSERT("l1 size after inserting", l1.size() == l1_count);
		ASSERT("l2 size after inserting", l2.size() == l2_count);
		
		l1.integrity_check();
		l2.integrity_check();
		
		FOREACH_DELETE(l2_t, i, l2)
		{
			if((r() & 1) == 0)
			{
				l2.unlink(i);
				--l2_count;
			}
		}
		
		ASSERT("l2 size after erasing", l2.size() == l2_count);
		
		l1.sort(std::less<integer>());
		l2.sort(greater<integer>());
		
		ASSERT("l1 size after sorting", l1.size() == l1_count);
		ASSERT("l2 size after sorting", l2.size() == l2_count);
		ASSERT("l1 sorted", gvl::is_sorted(l1.begin(), l1.end()));
		ASSERT("l2 sorted", gvl::is_sorted(l2.begin(), l2.end(), greater<integer>()));
		
		l1.integrity_check();
		l2.integrity_check();
		
		l2.unlink_all();
		
		if(r() & 1)
		{
			FOREACH_DELETE(l1_t, i, l1)
			{
				l1.erase(i);
			}
		}
		else
		{
			l1.clear();
		}
		
		ASSERT("l1 empty", l1.empty());
		ASSERT("l2 empty", l2.empty());
	}
}

GVLTEST(gvl, list, quickcheck)
{
	gvl::qc::context ctx;
	ctx.add("singleton", new singleton_list_gen, 1.5);
	ctx.add("concat", new concat_list_gen, 1.5);
	ctx.add("sorted", new sorted_list_gen);
	ctx.add("empty", new empty_list_gen);
	ctx.add("erase", new erase_list_gen);
	
	gvl::qc::test_property<list_integrity_property>(ctx);
	gvl::qc::test_property<list_pop_front_property<integer_list> >(ctx);
}
