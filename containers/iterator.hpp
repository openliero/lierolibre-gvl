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

#ifndef UUID_45275E9930944E2D32B8A686DB5E647A
#define UUID_45275E9930944E2D32B8A686DB5E647A

namespace gvl
{

#if 0
struct auto_any_base
{
    operator bool() const
    {
        return false;
    }
};

template<typename T>
struct auto_any : auto_any_base
{
	auto_any(T const& t)
	: item(t)
	{
	}

	mutable T item;
};

typedef auto_any_base const& auto_any_t;

T* type_as_pointer(T const&)
{
	return 0;
}

template<typename T>
inline auto_any<T> to_auto_any(T const& t)
{
    return t;
}

#define GVL_TYPEOF_AS_POINTER(V)    (true ? 0 : type_as_pointer(V))

#define GVL_EVAL_AUTO_ANY(expr) to_auto_any(expr)

if(auto_any_t _iter = GVL_EVAL_AUTO_ANY(iter)) {} else
for(;

#endif

#include <iterator>
#include <cstdlib>

// Deprecated, see range.hpp
template<typename ForwardIterator>
struct iterator_range
{
	typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename std::iterator_traits<ForwardIterator>::difference_type difference_type;
	typedef typename std::iterator_traits<ForwardIterator>::distance_type distance_type;
	typedef typename std::iterator_traits<ForwardIterator>::pointer pointer;
	typedef typename std::iterator_traits<ForwardIterator>::reference reference;
	typedef std::size_t size_type;

	typedef ForwardIterator iterator;

	iterator_range(ForwardIterator begin_init, ForwardIterator end_init)
	: begin_(begin_init)
	, end_(end_init)
	{
	}

	ForwardIterator begin()
	{
		return begin_;
	}

	ForwardIterator end()
	{
		return end_;
	}

	std::size_t size() const
	{
		return end_ - begin_;
	}

private:
	ForwardIterator begin_;
	ForwardIterator end_;
};

} // namespace gvl

#endif // UUID_45275E9930944E2D32B8A686DB5E647A
