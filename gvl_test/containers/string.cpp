#include <gvl/tut/tut.hpp>

#include <gvl/containers/string.hpp>
#include <gvl/tut/quickcheck/quickcheck.hpp>
#include <gvl/support/algorithm.hpp>

namespace tut
{

QC_BEGIN_GEN(empty_string_gen, gvl::string)
	return ptr_t(new t);
QC_END_GEN()

QC_BEGIN_GEN(char_string_gen, gvl::string)
	ptr_t ret(new t);
	ret->push_back((uint8_t)ctx.rand('a', 'z'+1));
	return ret;
QC_END_GEN()

QC_BEGIN_GEN(concat_string_gen, gvl::string)
	if(ctx.generator_depth() > 10)
	{
		return ctx.generate<t>("char");
	}
	ptr_t a(ctx.generate_any<t>());
	ptr_t b(ctx.generate_any<t>());
	a->append(*b);
	return a;
QC_END_GEN()

QC_BEGIN_GEN(change_char_gen, gvl::string)
	ptr_t a(ctx.generate_any<t>());
	if(!a->empty())
	{
		uint8_t new_c = ctx.rand(256);
		a->set(ctx.rand(a->size()), new_c);
	}
	return a;
QC_END_GEN()

QC_BEGIN_GEN(assign_swap_gen, gvl::string)
	ptr_t a(ctx.generate_any<t>());
	
	gvl::string x;
	x = *a;
	x.swap(*a);
	return a;
QC_END_GEN()

QC_BEGIN_PROP(concat_property, gvl::string)
	chk_result check(gvl::qc::context& ctx, QC_GEN_ANY(t, a), QC_GEN_ANY(t, b))
	{
		std::size_t a_size = a->size();
		std::size_t b_size = b->size();
		
		ptr_t ret(new t(*a));
		ret->append(*b);
		QC_ASSERT("concatentated size is correct", ret->size() == a_size + b_size);
		
		for(std::size_t i = 0; i < a_size; ++i)
			QC_ASSERT("char match", (*ret)[i] == (*a)[i]);
			
		for(std::size_t i = 0; i < b_size; ++i)
			QC_ASSERT("char match", (*ret)[a_size + i] == (*b)[i]);
		
		return chk_ok_reuse;
	}
QC_END_PROP()

QC_BEGIN_PROP(cstr_property, gvl::string)
	chk_result check(gvl::qc::context& ctx, QC_GEN_ANY(t, a))
	{
		// c_str() will not return the whole string if it contains zeroes
		for(std::size_t i = 0; i < a->size(); ++i)
		{
			if((*a)[i] == 0)
				return chk_not_applicable;
		}
		QC_ASSERT("c_str terminates correctly", a->size() == std::strlen(a->c_str()));
		return chk_ok_reuse;
	}
QC_END_PROP()

QC_BEGIN_PROP(set_property, gvl::string)
	chk_result check(gvl::qc::context& ctx, QC_GEN_ANY(t, a))
	{
		if(a->empty())
			return chk_not_applicable;
		gvl::string x = *a;
		std::size_t idx = ctx.rand(a->size());
		uint8_t new_c = ctx.rand(256);
		uint8_t old_c = (*a)[idx];
		a->set(idx, new_c);
		QC_ASSERT("copy was not affected", x[idx] == old_c);
		QC_ASSERT("original was affected", (*a)[idx] == new_c);
		return chk_ok_reuse;
	}
QC_END_PROP()

QC_BEGIN_PROP(clear_property, gvl::string)
	chk_result check(gvl::qc::context& ctx, QC_GEN_ANY(t, a))
	{
		if(a->empty())
			return chk_not_applicable;
		gvl::string x = *a;
		a->clear();
		QC_ASSERT("copy was not affected", !x.empty());
		QC_ASSERT("original was affected", a->empty());
		return chk_ok_reuse;
	}
QC_END_PROP()

struct string_data
{
	
};

typedef test_group<string_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::string");
} // namespace

namespace tut
{


template<>
template<>
void object::test<1>()
{
	gvl::qc::context ctx;
	
	ctx.add("char", new char_string_gen, 0.5);
	ctx.add("concat", new concat_string_gen);
	ctx.add("empty", new empty_string_gen, 0.5);
	ctx.add("change char", new change_char_gen);
	ctx.add("assign_swap_gen", new change_char_gen);
	
	gvl::qc::test_property<concat_property>(ctx);
	gvl::qc::test_property<cstr_property>(ctx);
	gvl::qc::test_property<set_property>(ctx);
	gvl::qc::test_property<clear_property>(ctx);
	
}

template<>
template<>
void object::test<2>()
{
	gvl::string x("hello, "), y("world");
	gvl::swap(x, y);
	
	ensure("swapped", y[6] == ' ');
	ensure("swapped", x[0] == 'w');
}


} // namespace tut
