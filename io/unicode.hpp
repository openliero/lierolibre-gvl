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

#ifndef UUID_0DA946DFD27148AC249303959F48AA8C
#define UUID_0DA946DFD27148AC249303959F48AA8C

#include "encoding.hpp"

namespace gvl
{

// NOTE: This does not yet handle surrogate pairs, so it only works with characters in BMP.

struct utf16_be_stream_reader : protected octet_stream_reader
{
	typedef octet_stream_reader base;

	utf16_be_stream_reader(stream_ptr str)
	: base(str)
	{
	}

	uint32_t get()
	{
		uint8_t h = base::get();
		uint8_t l = base::get();

		// TODO: Handle surrogate pairs

		return (h << 8) | l;
	}

	void get(uint32_t* dest, std::size_t len)
	{
		// TODO: Can optimize this
		for(std::size_t i = 0; i < len; ++i)
			dest[i] = get();
	}
};

} // namespace gvl

#endif // UUID_0DA946DFD27148AC249303959F48AA8C
