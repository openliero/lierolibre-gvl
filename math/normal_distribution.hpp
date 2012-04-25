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

#ifndef UUID_786BDFAEC45D4C3F36D47BB76ABCE5DE
#define UUID_786BDFAEC45D4C3F36D47BB76ABCE5DE

#include "ieee.hpp"

#include <cmath>

namespace gvl
{

template<typename T = double>
struct normal_distribution
{
	typedef T value_type;

	template<typename Random>
	value_type normal_variate(Random rand, value_type mean = value_type(0), value_type sigma = value_type(1))
	{
		using std::sqrt;
		using std::log;
		using std::sin;
		using std::cos;

		if(!valid)
		{
			r1 = value_type(rand.get_double());
			value_type r2 = value_type(rand.get_double());
			cached_rho = sqrt(value_type(-2) * log(value_type(1) - r2));
		}
		else
		{
			valid = false;
		}

		value_type const pi = value_type(3.14159265358979323846);

		return cached_rho * (valid ?
                          cos(value_type(2) * pi * r1) :
                          sin(value_type(2) * pi * r1)) * sigma + mean;
	}

	normal_distribution()
	: valid(false)
	{
	}

	value_type r1, cached_rho;
	bool valid;
};

} // namespace gvl

#endif // UUID_786BDFAEC45D4C3F36D47BB76ABCE5DE
