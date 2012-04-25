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

#ifndef GVL_ALLOCATOR_HPP
#define GVL_ALLOCATOR_HPP

#include <memory>
#include <cstdlib>

namespace gvl
{

template<class T>
struct malloc_allocator
{
	typedef T                 value_type;
	typedef value_type*       pointer;
	typedef value_type const* const_pointer;
	typedef value_type&       reference;
	typedef value_type const& const_reference;
	typedef std::size_t       size_type;
	typedef std::ptrdiff_t    difference_type;

	malloc_allocator() {}
	malloc_allocator(malloc_allocator const&) {}
	~malloc_allocator() {}

	pointer address(reference x) const
	{ return &x; }

	const_pointer address(const_reference x) const
	{ return &x; }

	pointer allocate(size_type n, const_pointer = 0)
	{
		void* p = std::malloc(n * sizeof(T));
		if (!p)
			throw std::bad_alloc();
		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type)
	{
		std::free(p);
	}

	size_type max_size() const
	{
		return static_cast<size_type>(-1) / sizeof(value_type);
	}

	void construct(pointer p, value_type const& x)
	{
		new(p) value_type(x);
	}

	void destroy(pointer p) { p->~value_type(); }
private:
	void operator=(malloc_allocator const&);
};

template<class T>
inline bool operator==(malloc_allocator<T> const&,
                       malloc_allocator<T> const&)
{
  return true;
}

template<class T>
inline bool operator!=(malloc_allocator<T> const&,
                       malloc_allocator<T> const&)
{
  return false;
}


}

#endif // GVL_ALLOCATOR_HPP
