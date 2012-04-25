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

#ifndef UUID_9F0036A7B22E453BA9D2A6810D1B70DD
#define UUID_9F0036A7B22E453BA9D2A6810D1B70DD

namespace gvl
{

template<typename T>
struct basic_flags
{
	basic_flags(T f)
	: flags_(f)
	{
	}

	void replace(T f, T mask)
	{
		flags_ ^= ((flags_ ^ f) & mask);
	}

	void set(T f)
	{
		flags_ |= f;
	}

	void reset(T f)
	{
		flags_ &= ~f;
	}

	void replace(T f)
	{
		flags_ = f;
	}

	bool any(T f) const
	{
		return (flags_ & f) != 0;
	}

	bool all(T f) const
	{
		return (flags_ & f) == f;
	}

	bool no(T f) const
	{
		return (flags_ & f) == 0;
	}

	void toggle(T f)
	{
		flags_ ^= f;
	}

	T as_integer()
	{
		return flags_;
	}

private:
	T flags_;
};

typedef basic_flags<unsigned int> flags;

} // namespace gvl

#endif // UUID_9F0036A7B22E453BA9D2A6810D1B70DD
