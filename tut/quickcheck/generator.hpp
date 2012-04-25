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

#ifndef GVL_TUT_QUICKCHECK_GENERATOR_HPP
#define GVL_TUT_QUICKCHECK_GENERATOR_HPP

#include <map>
#include <string>

namespace gvl
{
namespace qc
{

struct context;

struct base_generator
{
	base_generator(double weight = 1.0)
	: weight(weight)
	{
	}

	//virtual void* gen(context& ctx) = 0;

	double weight;
};

template<typename T>
struct generator : base_generator
{
	generator(double weight = 1.0)
	: base_generator(weight)
	{
	}

	//void* gen(context& ctx) { return gen_t(ctx); }
	virtual shared_ptr_any<T> gen_t(context& ctx) = 0;
};

#define QC_GEN_COMMON(type) \
	typedef type t; \
	typedef gvl::shared_ptr_any<type> ptr_t; \

#define QC_BEGIN_GEN(name, type) \
struct name : gvl::qc::generator<type> { \
	QC_GEN_COMMON(type) \
	ptr_t gen_t(gvl::qc::context& ctx) {

#define QC_END_GEN() } };


} // namespace qc
} // namespace gvl

#endif // GVL_TUT_QUICKCHECK_GENERATOR_HPP
