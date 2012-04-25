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

#ifndef UUID_B28D1ACCA789486008A1FF8B92CF00C5
#define UUID_B28D1ACCA789486008A1FF8B92CF00C5

#include <stdexcept>

namespace gvl
{

struct assert_failure : std::runtime_error
{
	assert_failure(std::string const& str)
	: std::runtime_error(str)
	{
	}
};

void passert_fail(char const* cond, char const* file, int line, char const* msg);


#ifndef NDEBUG
#define GVL_PASSERT(cond, msg) \
	if(!(cond)) gvl::passert_fail(#cond, __FILE__, __LINE__, msg)
#define GVL_SASSERT(cond) \
	if(!(cond)) gvl::passert_fail(#cond, __FILE__, __LINE__, "")
#else
#define GVL_PASSERT(cond, msg) ((void)0)
#define GVL_SASSERT(cond) ((void)0)
#endif

#if !GVL_NO_BS // GVL_NO_BS == 1 is currently not supported by gvl headers
#define passert(cond, msg) GVL_PASSERT(cond, msg)
#define sassert(cond) GVL_SASSERT(cond)
#endif

#define GVL_STATIC_ASSERT(cond) typedef char static_assert_[(cond) ? 1 : 0]

#if GVL_MSVCPP
# if _MSC_FULL_VER >= 140050320
#  define GVL_DEPRECATED(TEXT) __declspec(deprecated(TEXT))
# else
#  define GVL_DEPRECATED(TEXT) __declspec(deprecated)
# endif
#elif GVL_GCC
# define GVL_DEPRECATED(TEXT) __attribute__ ((deprecated(TEXT)));
#endif

#ifndef GVL_DEPRECATED
# define GVL_DEPRECATED(TEXT)
#endif

#define GVL_UNUSED(var) ((void)var)

}

#endif // UUID_B28D1ACCA789486008A1FF8B92CF00C5
