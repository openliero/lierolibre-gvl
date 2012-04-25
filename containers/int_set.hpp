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

#ifndef UUID_C27C476E17CF4899A94FCBB265827A0F
#define UUID_C27C476E17CF4899A94FCBB265827A0F

#include <cstddef>
#include <stdexcept>

namespace gvl
{

template<typename T>
struct int_set
{
	typedef std::size_t size_type;

	typedef T* iterator;

	int_set(size_type max)
	: n(0)
	, dense(new T[max])
	, sparse(new size_type[max])
	, max(max)
	{

	}

	bool contains(T v)
	{
		if(v >= sparse.size())
			return false;
		if(sparse[v] < n && dense[sparse[v]] == v)
			return true;
		return false;
	}

	void insert(T v)
	{
		if(contains(v))
			return;
		if(v >= max)
			throw std::out_of_range("integer is out of range");
		dense[n] = v;
		sparse[v] = n;
		++n;
	}

	void remove(T v)
	{
		if(!contains(v))
			return;
		size_type dense_pos = sparse[v];
		--n;
		T last = dense[n];
		dense[dense_pos] = last;
		sparse[last] = dense_pos;
	}

	iterator begin()
	{ return dense; }

	iterator end()
	{ return dense + n; }

private:
	size_t n;
	T* dense;
	size_type* sparse;
	size_type max;
};

}

#endif // UUID_C27C476E17CF4899A94FCBB265827A0F
