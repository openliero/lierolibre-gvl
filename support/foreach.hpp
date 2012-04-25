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

#ifndef GVL_FOREACH_HPP
#define GVL_FOREACH_HPP

namespace gvl
{

namespace foreach_detail_
{

struct auto_any_base
{
	operator bool() const
	{ return false; }
};

template<typename T>
struct auto_any : auto_any_base
{
	auto_any(T const& v)
	: value(v)
	{
	}

	mutable T value;
};

typedef auto_any_base const& auto_any_t;

template<typename T>
inline auto_any<T> wrap(T const& t)
{
    return t;
}

inline bool set_false(bool& x)
{
	x = false;
	return false;
}

template<typename T>
inline T* encode_type(T const& x)
{ return 0; }

template<typename T>
inline T& cast(auto_any_t x, T*)
{
	return static_cast<auto_any<T> const&>(x).value;
}

}

#define GVL_FOREACH_WRAP(x) gvl::foreach_detail_::wrap(x)
#define GVL_FOREACH_TYPEOF(x) (true ? 0 : gvl::foreach_detail_::encode_type(x))
#define GVL_FOREACH_CAST(x, type_expr) gvl::foreach_detail_::cast((x), GVL_FOREACH_TYPEOF(type_expr))

#define GVL_FOREACH_DONE(VAR, r) (GVL_FOREACH_CAST(VAR, (r)).empty())
#define GVL_FOREACH_NEXT(VAR, r) (GVL_FOREACH_CAST(VAR, (r)).pop_front())
#define GVL_FOREACH_DEREF(VAR, r) (GVL_FOREACH_CAST(VAR, (r)).front())

#define GVL_FOREACH(VAR, RANGE) \
	if (gvl::foreach_detail_::auto_any_t _foreach_range = GVL_FOREACH_WRAP(RANGE)) {} else   \
	for (bool _foreach_continue = true;                                                         \
		_foreach_continue && !GVL_FOREACH_DONE(_foreach_range, RANGE);                                    \
		_foreach_continue ? GVL_FOREACH_NEXT(_foreach_range, RANGE) : (void)0)                            \
		if  (gvl::foreach_detail_::set_false(_foreach_continue)) {} else                      \
		for (VAR = GVL_FOREACH_DEREF(_foreach_range, RANGE); !_foreach_continue; _foreach_continue = true)


}

#endif // GVL_FOREACH_HPP
