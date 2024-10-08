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

#ifndef UUID_C3FD1AEF51C649B23B829FB145771D9C
#define UUID_C3FD1AEF51C649B23B829FB145771D9C

#include "../support/cstdint.hpp"
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <climits>

namespace gvl
{

struct common_ibitstream
{

};

struct common_obitstream
{

};

template<typename DerivedT>
struct basic_ibitstream // : common_ibitstream
{
	//typedef WordT word;
	typedef uint8_t byte;
	//static std::size_t const word_bits = sizeof(word) * CHAR_BIT;
	typedef unsigned int bitbuf_t; // We hope unsigned int is the fastest type to deal with

	basic_ibitstream()
	: in_bits(0)
	, in_bits_left(0)
	{
	}

	template<typename T2>
	basic_ibitstream(basic_ibitstream<T2> const& b)
	: in_bits(b.in_bits)
	, in_bits_left(b.in_bits_left)
	{

	}

	DerivedT& derived()
	{
		return *static_cast<DerivedT*>(this);
	}

	// Number of calls to get_byte that will be done
	// if 'bits' bits are read from the stream after this
	// point.
	std::size_t in_bytes_required(std::size_t bits) const
	{
		return (bits + 8 - 1 - in_bits_left) / 8;
	}

	void swap(basic_ibitstream& b)
	{
		std::swap(in_bits_left, b.in_bits_left);
		std::swap(in_bits, b.in_bits);
	}

	void ignore(uint32_t bits);
	uint32_t get();
	uint32_t get_uint(uint32_t bits);
	void get_block(void* ptr_, size_t len);
	uint32_t get_lim(uint32_t low, uint32_t high);
	void resetg();

	// NOTE: Assumes that the stream has been read up to a byte boundary.
	// Stream contents becomes undefined if this isn't true.
	inline uint8_t unsafe_aligned_get_byte();
	inline bool is_aligned();

	void get_debug_mark();

	uint32_t get_trunc(uint32_t count);
	uint32_t get_unary();
	uint32_t get_golomb(uint32_t m);
	uint32_t get_rice(uint32_t shift);

	bitbuf_t in_bits;
	uint32_t in_bits_left;
};

template<typename DerivedT>
struct basic_obitstream : common_obitstream
{
	//typedef WordT word;
	//static std::size_t const word_bits = sizeof(word) * CHAR_BIT;
	typedef uint8_t byte;
	typedef unsigned int bitbuf_t; // We hope unsigned int is the fastest type to deal with

	basic_obitstream()
	: out_bits(0)
	, out_bits_left(8)
	{
	}

	template<typename T2>
	basic_obitstream(basic_obitstream<T2> const& b)
	: out_bits(b.out_bits)
	, out_bits_left(b.out_bits_left)
	{

	}

	DerivedT& derived()
	{
		return *static_cast<DerivedT*>(this);
	}


	// Bits written if 'bytes' calls to put_byte were made
	// after an initial state of out_bits_left == 8.
	std::size_t out_bits_written(std::size_t bytes) const
	{
		return bytes * 8 + (8 - out_bits_left);
	}

	void swap(basic_obitstream& b)
	{
		std::swap(out_bits_left, b.out_bits_left);
		std::swap(out_bits, b.out_bits);
	}

	void put(uint32_t bit);
	void put_uint(uint32_t i, uint32_t bits);
	void put_block(void const* ptr_, std::size_t len);
	void put_lim(uint32_t v, uint32_t low, uint32_t high);

	// Called to allocate a byte just before it's begun.
	// put_byte() should then write the data to that byte.
	// By default we don't preallocate bytes.
	void allocate_out_byte() { }

	void resetp();
	void finish();

	// NOTE: Assumes that the stream has been written up to a byte boundary.
	// Stream contents becomes undefined if this isn't true.
	inline void unsafe_aligned_put_byte(uint8_t w);
	inline bool is_aligned();

	void put_debug_mark();

	void put_trunc(uint32_t v, uint32_t count);
	void put_unary(uint32_t v);
	void put_golomb(uint32_t v, uint32_t m);
	void put_rice(uint32_t v, uint32_t shift);

	bitbuf_t out_bits;
	uint32_t out_bits_left;
};

// Dummy obitstream to allow normalization of interface of bitstreams.
// Throws on all output operations.
struct dummy_obitstream
{
	void not_supported() const
	{ throw std::runtime_error("Not supported"); }

	void put(uint32_t) { not_supported(); }
	void put_uint(uint32_t, uint32_t) { not_supported(); }
	void put_block(void const*, std::size_t) { not_supported(); }
	void put_lim(uint32_t, uint32_t, uint32_t) { not_supported(); }

	/*
	template<typename Source>
	void put(Source&, std::size_t) { not_supported(); }
	*/

	void resetp() { not_supported(); }
	void finish() { not_supported(); }
	void clear() { not_supported(); }

	void put_trunc(uint32_t, uint32_t) { not_supported(); }
	void put_unary(uint32_t) { not_supported(); }
	void put_golomb(uint32_t, uint32_t) { not_supported(); }
	void put_rice(uint32_t, uint32_t) { not_supported(); }

	void put_debug_mark() { not_supported(); }
};

// Dummy ibitstream to allow normalization of interface of bitstreams.
// Throws on all input operations.
struct dummy_ibitstream
{
	void not_supported() const
	{ throw std::runtime_error("Not supported"); }

	std::size_t in_bytes_required(std::size_t bits) const { not_supported(); return 0; }

	void     ignore(uint32_t bits) { not_supported(); }
	uint32_t get() { not_supported(); return 0; }
	uint32_t get_uint(uint32_t bits) { not_supported(); return 0; }
	void     get_block(void* ptr_, size_t len) { not_supported(); }
	uint32_t get_lim(uint32_t low, uint32_t high) { not_supported(); return 0; }
	void     resetg() { not_supported(); }

	uint32_t get_trunc(uint32_t count) { not_supported(); return 0; }
	uint32_t get_unary() { not_supported(); return 0; }
	uint32_t get_golomb(uint32_t m) { not_supported(); return 0; }
	uint32_t get_rice(uint32_t shift) { not_supported(); return 0; }

	void get_debug_mark() { not_supported(); }
};

} // namespace gvl

#include "bitstream_templ.hpp"

#endif // UUID_C3FD1AEF51C649B23B829FB145771D9C
