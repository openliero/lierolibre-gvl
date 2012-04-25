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

#ifndef UUID_A80850D4219545B8CF0EA18AA088876D
#define UUID_A80850D4219545B8CF0EA18AA088876D

#include <cstddef>
#include "../support/cstdint.hpp"

namespace gvl
{

struct int10
{
	int32_t& operator[](std::size_t i)
	{ return limb[i]; }

	int32_t operator[](std::size_t i) const
	{ return limb[i]; }

	int32_t limb[10];
};

struct curve25519
{
	static bool sign(uint8_t* v, uint8_t const* h, uint8_t const* x, uint8_t const* s);
	static void verify(uint8_t* Y, uint8_t const* v, uint8_t const* h, uint8_t const* P);
	static void clamp(uint8_t* k);
	static void keygen(uint8_t* P, uint8_t* s, uint8_t* k);
	static void curve(uint8_t* Z, uint8_t* k, uint8_t* P);
};

}

#endif // UUID_A80850D4219545B8CF0EA18AA088876D
