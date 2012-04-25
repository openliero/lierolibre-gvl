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

#ifndef GVL_TUT_QUICKCHECK_PROPERTY_HPP
#define GVL_TUT_QUICKCHECK_PROPERTY_HPP

#include "generator.hpp"
#include "context.hpp"

#include <map>
#include <string>
#include "../../io/iostream.hpp"

namespace gvl
{
namespace qc
{

enum chk_result
{
	chk_ok, // Checked out
	chk_ok_reuse, // Checked out, and passed object can be reused
	chk_fail, // Check failed
	chk_not_applicable // Check is not applicable to the generated objects
};

template<typename T>
struct property
{
	virtual shared_ptr_any<T> generate(context& ctx)
	{
		return ctx.generate_any<T>();
	}

	virtual chk_result do_check(context&) = 0;

	bool run(context& ctx, int max_generated, int max_tested)
	{
		int tests_ran = 0;
        int generated = 0;

        context::current = &ctx;

		for(generated = 0; generated < max_generated; )
        {
            ++generated;

			ctx.reset_assert_fails();

            chk_result res = do_check(ctx);

            if(res != chk_not_applicable)
            {
				++tests_ran;
				if(res == chk_fail || ctx.get_assert_fails() > 0)
				{
					gvl::cout() << "Property failed on test " << tests_ran << gvl::endl;
					return false;
				}

				if(res == chk_ok_reuse)
				{
				}

				if(tests_ran >= max_tested)
					break;
			}
        }

        context::current = 0;

        return true;
	}
};

template<typename Prop>
bool test_property(context& ctx, int max_generated = 200, int max_tested = 100)
{
	Prop prop;
	return prop.run(ctx, max_generated, max_tested);
}

// VC++ 2008 has a bug where default values that call a template
// function in a different namespace with explicit template parameters
// are rejected. We pass a cast zero-pointer instead of explicit template
// parameters.

template<typename T>
static gvl::shared_ptr_any<T> cur_generate_any(T* = 0)
{
	return gvl::qc::context::current->generate_any<T>();
}

template<typename T>
static gvl::shared_ptr_any<T> cur_generate(std::string const& name, T* = 0)
{
	return gvl::qc::context::current->generate<T>(name);
}


#define QC_BEGIN_PROP(name, type) \
struct name : gvl::qc::property<type> { \
	typedef type t; \
	typedef gvl::shared_ptr_any<type> ptr_t; \
	virtual gvl::qc::chk_result do_check(gvl::qc::context& ctx) \
	{ return check(ctx); }

#define QC_END_PROP() };

#define QC_BEGIN_GENERIC_PROP(name) \
template<typename T_> \
struct name : gvl::qc::property<T_> { \
	typedef T_ t; \
	typedef gvl::shared_ptr_any<T_> ptr_t; \
	virtual gvl::qc::chk_result do_check(gvl::qc::context& ctx) \
	{ return check(ctx); }

#define QC_CUR_CTX (*gvl::qc::context::current)

#if 0
#define QC_GEN(type, name, g)  gvl::shared_ptr_any<type> name = gvl::qc::cur_generate<type>(g, (type*)0)
#define QC_GEN_ANY(type, name) gvl::shared_ptr_any<type> name = gvl::qc::cur_generate_any<type>((type*)0)
#else
#define QC_GEN(type, name, g)  gvl::shared_ptr_any<type> name = QC_CUR_CTX.generate<type>(g)
#define QC_GEN_ANY(type, name) gvl::shared_ptr_any<type> name = QC_CUR_CTX.generate_any<type>()
#endif

} // namespace qc
} // namespace gvl

#endif // GVL_TUT_QUICKCHECK_PROPERTY_HPP
