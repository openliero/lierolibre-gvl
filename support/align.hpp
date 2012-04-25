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

#ifndef UUID_2B6D8149059740980E707BA5CBAC8E41
#define UUID_2B6D8149059740980E707BA5CBAC8E41

#include <cstddef>

#ifdef BOOST_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4121) // alignment is sensitive to packing
#endif
#if defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
#pragma option push -Vx- -Ve-
#endif

// Adapted from boost

namespace gvl
{

namespace detail
{

template <typename T>
struct alignment_of_hack
{
	char c;
	T t;
	alignment_of_hack();
};

template <unsigned A, unsigned S> // Why does boost use unsigned here?
struct alignment_logic
{
	static std::size_t const value = A < S ? A : S;
};

class alignment_dummy;
typedef void (*function_ptr)();
typedef int (alignment_dummy::*member_ptr);
typedef int (alignment_dummy::*member_function_ptr)();

} // namespace detail

template<typename T>
struct alignment_of
{
	static std::size_t const value =
        detail::alignment_logic<
			sizeof(detail::alignment_of_hack<T>) - sizeof(T), sizeof(T)
		>::value;
};

template<typename T>
struct alignment_of<T&>
: alignment_of<T*>
{
};

// Use: union foo { max_align bar; ... }
union max_align
{
	char dummy0; short dummy1; int dummy2; long dummy3;
	float dummy4; double dummy5; long double dummy6;
	void* dummy7;
	detail::function_ptr dummy8;
	detail::member_ptr dummy9;
	detail::member_function_ptr dummy10;
};

} // namespace gvl

#if defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
#pragma option pop
#endif
#ifdef BOOST_MSVC
#   pragma warning(pop)
#endif

#endif // UUID_2B6D8149059740980E707BA5CBAC8E41
