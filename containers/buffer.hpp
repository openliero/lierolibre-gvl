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

#ifndef UUID_ADFEAF07FE6C4EB59559E1AFCB250911
#define UUID_ADFEAF07FE6C4EB59559E1AFCB250911

#include "../support/cstdint.hpp"
#include "../support/debug.hpp"
#include <cstdlib>

namespace gvl
{

struct buffer_memory;

struct buffer
{
	uint8_t* base;
	uint8_t* end;
	uint8_t* limit;
	uint8_t* mem;
	bool redirected;

	struct redirect_tag {};

	buffer();
	buffer(void const* p, std::size_t s);
	buffer(void* p, std::size_t s, redirect_tag);
	~buffer();
	void resize(std::size_t new_size);
	uint8_t* insert_uninitialized(std::size_t extra_space);
	void reserve(buffer* self, std::size_t extra_space);
	void insert(void const* p, std::size_t s);
	void insert_fill(uint8_t b, std::size_t s);
	void concat(buffer& other);

	void begin_redirect(uint8_t* ptr, std::size_t len, buffer_memory& old);
	void end_redirect(buffer_memory& old);

	void clear() { end = base = mem; }
	std::size_t size() const { return end - base; }

	void put(uint8_t b)
	{
		if(end == limit)
		{
			resize(size() + 1);
			end[-1] = b;
		}
		else
			*end++ = b;
	}

	uint8_t get()
	{
		passert(base < end, "Buffer underrun");
		uint8_t v = base[0];
		consume(1);
		return v;
	}

	uint8_t& operator[](std::size_t o)
	{
		passert(o < size(), "Out of bounds");
		return base[o];
	}

	uint8_t const& operator[](std::size_t o) const
	{
		passert(o < size(), "Out of bounds");
		return base[o];
	}

	void consume(std::size_t amount)
	{
		passert(amount <= size(), "Buffer underrun");
		base += amount;
	}

	std::size_t cost_of_insert(std::size_t amount)
	{
		if(end + amount > limit)
		{
			std::size_t cost = amount;
			std::size_t size = this->size();
			if(base != mem)
				cost += size;
			if(mem + size + amount > limit)
				cost += size * 2 + amount;
			return cost;
		}

		return amount;
	}

	bool empty() const { return base == end; }
	uint8_t* offset(std::size_t o) { return base + o; }
	uint8_t* data() { return base; }
	uint8_t const* data() const { return base; }

};

struct buffer_memory
{
	uint8_t* mem;
	uint8_t* limit;

	void save(buffer& buf)
	{
		passert(!buf.redirected, "Can't store memory from redirected buffer");
		mem = buf.mem;
		limit = buf.limit;
		buf.redirected = true;
	}

	void restore(buffer& buf)
	{
		buf.base = mem;
		buf.end = mem;
		buf.limit = limit;
		buf.mem = mem;
		buf.redirected = false;
	}
};

} // namespace gvl

#endif // UUID_ADFEAF07FE6C4EB59559E1AFCB250911
