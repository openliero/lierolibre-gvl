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

#ifndef UUID_6DD5291E8D2F459FA7A8469E4A95E300
#define UUID_6DD5291E8D2F459FA7A8469E4A95E300

#include <iterator>

namespace gvl
{

#if 0
template<typename T, void (T::*)(T&)>
struct check_swap
{ typedef T type; };

template<typename T>
void swap(typename check_swap<T, &T::swap>::type& x, T& y)
{ x.swap(y); }

template<typename T>
struct ref_wrapper
{
	ref_wrapper(T& v)
	: v(v)
	{ }
	T& v;
};

template<typename T>
void swap(ref_wrapper<T> x, T& y)
{
	T temp(x.v);
	x.v = y;
	y = temp;
}
#endif

template<typename InputIterator, typename StrictWeakOrdering>
bool is_sorted(InputIterator b, InputIterator e, StrictWeakOrdering comp)
{
	typedef std::iterator_traits<InputIterator> traits;
	typedef typename traits::value_type value_type;
	typedef typename traits::pointer pointer;

	pointer prev = &*b;

	for(++b; b != e; ++b)
	{
		pointer cur = &*b;
		if(comp(*cur, *prev))
			return false;
	}

	return true;
}

template<typename InputIterator>
bool is_sorted(InputIterator b, InputIterator e)
{
	return gvl::is_sorted(b, e, std::less<
		typename std::iterator_traits<InputIterator>::value_type>());
}

template<typename Range, typename Pred>
Range drop_while(Range r, Pred pred)
{
	while(!r.empty())
	{
		if(!pred(r.front()))
			break;
		r.pop_front();
	}

	return r;
}

} // namespace gvl

#endif // UUID_6DD5291E8D2F459FA7A8469E4A95E300
