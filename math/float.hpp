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

#ifndef UUID_9A3CA425DFA94C58CFBDEB99F70179F2
#define UUID_9A3CA425DFA94C58CFBDEB99F70179F2

#include <cmath>

#include "../support/platform.hpp"

namespace gvl
{

#if GVL_MSVCPP && !GVL_X86_64
inline long lrint(double x)
{
	long r;
	__asm
	{
		fld x
		fistp r
	}

	return r;
}

#elif GVL_GCC && GVL_X86

// TODO: Not tested at all
inline long lrint(double x)
{
	long ret;
	__asm__ __volatile__ (
		"fldq %0;"
		"fistpl %1;" : "m" (x), "=m" (ret) : : "st") ;
	return ret;
}
#endif

inline long round_floor(double x)
{
	return lrint(x - 0.5);
}

inline long round_ceil(double x)
{
	return lrint(x + 0.5);
}

} // namespace gvl

#endif // UUID_9A3CA425DFA94C58CFBDEB99F70179F2
