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

#include "buffer.hpp"

#include <cstring>
#include <cstdlib>

namespace gvl
{

buffer::buffer()
: base(0), end(0), limit(0)
, mem(0), redirected(false)
{
}

buffer::buffer(void const* p, std::size_t s)
: base(static_cast<uint8_t*>(std::malloc(s)))
, end(base + s)
, limit(base + s)
, mem(base)
, redirected(false)
{
	std::memcpy(base, p, s);
}

buffer::buffer(void* p, std::size_t s, buffer::redirect_tag)
: base(static_cast<uint8_t*>(p))
, end(base + s)
, limit(base + s)
, mem(base)
, redirected(true)
{

}

buffer::~buffer()
{
	if(!redirected)
		std::free(mem);
}

void buffer::resize(std::size_t new_size)
{
retry:
	uint8_t* p = base + new_size;
	if(p > limit)
	{
		if(base != mem)
		{
			std::memmove(mem, base, size());
			base = mem;
			goto retry;
		}

		std::size_t min_cap = new_size;
		std::size_t new_cap = size() * 2;
		if(new_cap < min_cap)
			new_cap = min_cap;

		if(redirected)
		{
			uint8_t* old_base = base;
			std::size_t old_size = size();
			base = mem = static_cast<uint8_t*>(std::malloc(new_cap));
			std::memcpy(base, old_base, old_size);
			redirected = false; /* Not redirected anymore */
		}
		else
			base = mem = static_cast<uint8_t*>(std::realloc(mem, new_cap));
		limit = mem + new_cap;
		end = base + new_size;
	}
	else
	{
		end = p;
	}
}

void buffer::begin_redirect(uint8_t* ptr, std::size_t len, buffer_memory& old)
{
	old.save(*this);
	mem = ptr;
	base = ptr;
	end = ptr + len;
	limit = ptr + len;
}

// NOTE: TODO: This is quite error prone as there is no way to check if end_redirect is called in error
void buffer::end_redirect(buffer_memory& old)
{
	if(!redirected)
		std::free(mem); /* The only way a redirection can cease is if resize allocated a new buffer */
	old.restore(*this);
}

uint8_t* buffer::insert_uninitialized(std::size_t extra_space)
{
	uint8_t* new_end = end + extra_space;

	if(new_end > limit)
	{
		std::size_t old_size = size();
		resize(old_size + extra_space);
		return base + old_size;
	}
	else
	{
		uint8_t* old_end = end;
		end = new_end;
		return old_end;
	}
}

void buffer::reserve(buffer* self, std::size_t extra_space)
{
	if((end + extra_space) > limit)
	{
		resize(size() + extra_space);
	}
}

void buffer::insert(void const* p, std::size_t s)
{
	std::memcpy(insert_uninitialized(s), p, s);
}

void buffer::insert_fill(uint8_t b, std::size_t s)
{
	std::memset(insert_uninitialized(s), b, s);
}

void buffer::concat(buffer& other)
{
	insert(other.base, other.size());
}

} // namespace gvl
