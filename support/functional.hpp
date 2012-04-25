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

#ifndef UUID_64B1EAC4E4F545FA3B131FA346621126
#define UUID_64B1EAC4E4F545FA3B131FA346621126

namespace gvl
{

struct default_delete
{
	template<typename T>
	void operator()(T* p) const
	{
		delete p;
	}
};

struct dummy_delete
{
	template<typename T>
	void operator()(T const&) const
	{
		// Do nothing
	}
};

struct default_compare
{
	template<typename T>
	int operator()(T const& a, T const& b) const
	{
		if(a < b)
			return -1;
		else if(b < a)
			return 1;
		else
			return 0;
	}
};

#if 0

template<typename T>
void caller(void* p)
{
	static_cast<T*>(p)->operator()();
}

struct functor_wrapper
{
	template<typename T>
	functor_wrapper(T& t)
	: f(&caller<T>)
	, p(&t)
	{

	}

	void call()
	{ f(p); }

	void(*f)(void* p);
	void* p;
};

#endif

} // namespace gvl

#endif // UUID_64B1EAC4E4F545FA3B131FA346621126
