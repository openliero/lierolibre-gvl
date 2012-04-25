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

#ifndef UUID_F29926F3240844A09DCFB9B1828C7DC8
#define UUID_F29926F3240844A09DCFB9B1828C7DC8

#ifndef NDEBUG
#include "ieee.hpp"
#endif
#include "../support/debug.hpp"

namespace gvl
{

template<typename DerivedT, typename ValueT>
struct prng_common
{
	typedef ValueT value_type;

	DerivedT& derived()
	{ return *static_cast<DerivedT*>(this); }

	// Number in [0.0, 1.0)
	double get_double()
	{
		uint32_t v = derived()();
		// This result should be exact if at least double-precision is used. Therefore
		// there shouldn't be any reason to use gD.
		double ret = v / 4294967296.0;
		sassert(ret == gD(v, 4294967296.0));
		return ret;
	}

	// NOTE! Not reproducible right now. We don't want
	// to take the (potential) hit if it's not necessary.
	// Number in [0.0, max)
	double get_double(double max)
	{
		return get_double() * max;
	}

	// Number in [0, max)
	uint32_t operator()(uint32_t max)
	{
		uint64_t v = derived()();
		v *= max;
		return uint32_t(v >> 32);
	}

	// Number in [min, max)
	uint32_t operator()(uint32_t min, uint32_t max)
	{
		sassert(min < max);
		return operator()(max - min) + min;
	}
};

} // namespace gvl

#endif // UUID_F29926F3240844A09DCFB9B1828C7DC8

