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

#ifndef UUID_3EEC2EFDDD1945483B3E30B8FE895AA9
#define UUID_3EEC2EFDDD1945483B3E30B8FE895AA9

namespace gvl
{

// NOTE: One can't bind rvalues to move_holder. Instead
// one has to explicitly use rvalue() and make sure it's only
// used for rvalues.

template<typename T>
struct move_holder
{
	move_holder(T& v)
	: v(v)
	{
	}

	operator T&()
	{ return v; }

	T& operator*()
	{ return v; }

	T* operator->()
	{ return &v; }

private:
	T& v;
};

template<typename T>
inline move_holder<T> move(T& v)
{
	return move_holder<T>(v);
}

template<typename T>
inline move_holder<T> rvalue(T const& v)
{
	return move_holder<T>(const_cast<T&>(v));
}

} // namespace gvl

#endif // UUID_3EEC2EFDDD1945483B3E30B8FE895AA9
