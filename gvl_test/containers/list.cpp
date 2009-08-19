#include <gvl/tut/tut.hpp>

#include <gvl/list.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <gvl/support/macros.hpp>
#include <functional>
#include <memory>
#include <algorithm>

#include <gvl/tut/quickcheck/context.hpp>
#include <gvl/tut/quickcheck/generator.hpp>
#include <gvl/tut/quickcheck/property.hpp>

namespace tut
{

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

#define QC_BEGIN_GEN(name, type) \
struct name : gvl::qc::generator<type> { \
	typedef type t; \
	t* gen_t(gvl::qc::context& ctx) {

#define QC_END_GEN() } };

#define QC_BEGIN_PROP(name, type) \
struct clear_property : gvl::qc::property<type> { \
	typedef type t;
	
#define QC_END_PROP() };

typedef gvl::list<integer, tag1> integer_list;

QC_BEGIN_GEN(singleton_list_gen, integer_list)
	std::auto_ptr<t> ret(new t);
	ret->push_back(new integer(1));
	return ret.release();
QC_END_GEN()

QC_BEGIN_GEN(concat_list_gen, integer_list)
	if(ctx.generator_depth() > 10)
	{
		std::auto_ptr<t> ret(new t);
		ret->push_back(new integer(1));
		return ret.release();
	}
	std::auto_ptr<t> a(ctx.generate_any<t>());
	std::auto_ptr<t> b(ctx.generate_any<t>());
	a->splice(*b);
	return a.release();
QC_END_GEN()

QC_BEGIN_GEN(sorted_list_gen, integer_list)
	std::auto_ptr<t> a(ctx.generate_any<t>());
	a->sort(std::less<integer>());
	return a.release();
QC_END_GEN()

QC_BEGIN_PROP(clear_property, integer_list)
	bool check(gvl::qc::context& ctx, t& obj)
	{
		std::cout << obj.size() << std::endl;
		obj.clear();
		return obj.empty();
	}
QC_END_PROP()



struct list_data
{
	
};

typedef test_group<list_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::list");
} // namespace

namespace tut
{

template<typename T>
struct greater : std::binary_function<T const&, T const&, bool>
{
	bool operator()(T const& a, T const& b) const
	{
		return b < a;
	}
};

template<>
template<>
void object::test<1>()
{
	typedef gvl::list<integer, tag1> l1_t;
	typedef gvl::list<integer, tag2, gvl::dummy_delete> l2_t;
	l1_t l1;
	l2_t l2;
	
	gvl::tt800 r(1234);
	
	for(int repeat = 0; repeat < 100; ++repeat)
	{
		std::size_t l1_count = 0;
		std::size_t l2_count = 0;
		
		int count = r.range(0, 200);
		
		for(int i = 0; i < count; ++i)
		{
			integer* o = new integer(r.range(0, 10000));
			l1.push_back(o);
			l2.push_back(o);
			++l1_count;
			++l2_count;
		}
		
		ensure("l1 size after inserting", l1.size() == l1_count);
		ensure("l2 size after inserting", l2.size() == l2_count);
		
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
		
		ensure("l2 size after erasing", l2.size() == l2_count);
		
		l1.sort(std::less<integer>());
		l2.sort(greater<integer>());
		
		ensure("l1 size after sorting", l1.size() == l1_count);
		ensure("l2 size after sorting", l2.size() == l2_count);
		ensure("l1 sorted", gvl::is_sorted(l1.begin(), l1.end()));
		ensure("l2 sorted", gvl::is_sorted(l2.begin(), l2.end(), greater<integer>()));
		
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
		
		ensure("l1 empty", l1.empty());
		ensure("l2 empty", l2.empty());
	}
}

template<>
template<>
void object::test<2>()
{
	gvl::qc::context ctx;
	ctx.add("singleton", new singleton_list_gen);
	ctx.add("concat", new concat_list_gen);
	ctx.add("sorted", new sorted_list_gen);
	
	gvl::qc::test_property<clear_property>(ctx);
}

} // namespace tut
