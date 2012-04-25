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

#ifndef UUID_5849ECCC7F1142CA9E5E9CA33B298A6D
#define UUID_5849ECCC7F1142CA9E5E9CA33B298A6D

#include <cstddef>
#include <utility>
#include <string>
#include "cstdint.hpp"

namespace gvl
{

typedef uint32_t hash_t;
int const hash_bits = 32;

inline hash_t hash(uint32_t v, uint32_t p = 0x3C618459)
{
	v ^= p;
	v *= v * 2 + 1;
	return p - v;
}

inline hash_t hash(int v, uint32_t p = 0x3C618459)
{ return hash(uint32_t(v), p); }

inline hash_t hash(void const* v, uint32_t p = 0x3C618459)
{ return hash(hash_t(std::size_t(v)), p); }

template<typename T1, typename T2>
inline hash_t hash(std::pair<T1, T2> const& v)
{
	return (hash(v.first) * 2654435761ul) ^ hash(v.second);
}

inline hash_t hash(std::string const& v, unsigned p = 0x3C618459)
{
	std::size_t amount = 512;

	if(amount > v.size())
		amount = v.size();

	hash_t h = p;
	for(std::size_t i = 0; i < amount; ++i)
	{
		h = h*33 ^ (unsigned char)v[i];
	}
	return h;
}

template<typename T>
inline hash_t hash(T const& v, unsigned p = 0x3C618459)
{
	return v.hash(p);
}

struct hash_functor
{
	template<typename T>
	hash_t operator()(T const& v, unsigned p = 0x3C618459) const
	{
		return hash(v, p);
	}
};

std::size_t next_prime(std::size_t n);
std::size_t prev_prime(std::size_t n);

} // namespace vl

#endif // UUID_5849ECCC7F1142CA9E5E9CA33B298A6D
