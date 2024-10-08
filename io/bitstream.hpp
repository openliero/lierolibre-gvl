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

#ifndef UUID_4C44D1D95C5C40B194BE5294E8B984DE
#define UUID_4C44D1D95C5C40B194BE5294E8B984DE

#include "../support/cstdint.hpp"
#include "../support/bits.hpp"
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <climits>

namespace gvl
{

#define GVL_BITMASK(b) (~(unsigned int)0) >> (32-(b))

template<typename DerivedT, int BufBytes = 4>
struct basic_ibitstream
{
	typedef uint8_t byte;
	typedef unsigned int bitbuf_t; // We hope unsigned int is the fastest type to deal with

	static int const in_buf_bits = BufBytes * 8;
	static unsigned int const in_buf_mask = GVL_BITMASK(in_buf_bits);

	basic_ibitstream()
	: in_bits(0)
	, in_bit_count(in_buf_bits)
	{
	}

	template<typename T2>
	basic_ibitstream(basic_ibitstream<T2, BufBytes> const& b)
	: in_bits(b.in_bits)
	, in_bit_count(b.in_bit_count)
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
		return in_chunks_required(bits) * BufBytes;
	}

	std::size_t in_chunks_required(std::size_t bits) const
	{
		return ((bits + in_bit_count - 1) / in_buf_bits);
	}

	uint32_t in_bits_in_buffer() const
	{ return in_buf_bits - in_bit_count; }

	unsigned int in_bits_as_uint() const
	{ return gvl::shr_1_32(in_bits, in_bit_count); }

	template<int C>
	bitbuf_t get_chunk();

	template<int C>
	void ignore_chunks(int c);

	bitbuf_t get_buf()
	{ return derived().get_chunk<BufBytes>(); }

	void ignore_bufs(int c)
	{ derived().ignore_chunks<BufBytes>(c); }

	void swap(basic_ibitstream& b)
	{
		std::swap(in_bit_count, b.in_bit_count);
		std::swap(in_bits, b.in_bits);
	}

	void ignore(uint32_t bits);
	uint32_t get();
	uint32_t get_uint(uint32_t bits);
	void get_block(void* ptr_, size_t len);
	uint32_t get_lim(uint32_t low, uint32_t high);
	void resetg();

	void get_debug_mark();

	bitbuf_t in_bits;
	uint32_t in_bit_count;
};

template<typename DerivedT, int BufBytes = 4>
struct basic_obitstream
{
	typedef uint8_t byte;
	typedef unsigned int bitbuf_t; // We hope unsigned int is the fastest type to deal with

	static int const out_buf_bits = BufBytes * 8;
	static unsigned int const out_buf_mask = GVL_BITMASK(out_buf_bits);

	basic_obitstream()
	: out_bits(0)
	, out_bit_count(0)
	{
	}

	template<typename T2>
	basic_obitstream(basic_obitstream<T2, BufBytes> const& b)
	: out_bits(b.out_bits)
	, out_bit_count(b.out_bit_count)
	{

	}

	DerivedT& derived()
	{
		return *static_cast<DerivedT*>(this);
	}


	// Bits written if 'bytes' calls to put_byte were made
	// after an initial state of out_bit_count == 0.
	uint32_t out_bits_written(uint32_t bytes) const
	{ return bytes * 8 + out_bit_count;	}

	// Bits that can be written before 'bytes' calls to
	// put_byte is issued.
	uint32_t out_bits_writable(uint32_t bytes) const
	{ return bytes * 8 - out_bit_count;	}

	template<int C>
	void put_chunk(bitbuf_t);

	void put_buf(bitbuf_t v)
	{ return derived().put_chunk<BufBytes>(v); }

	uint32_t out_bits_in_buffer() const
	{ return out_bit_count; }

	uint32_t out_bits_bufferable() const
	{ return out_buf_bits - out_bit_count; }

	unsigned int out_bits_as_uint() const
	{ return out_bits; }

	void swap(basic_obitstream& b)
	{
		std::swap(out_bit_count, b.out_bit_count);
		std::swap(out_bits, b.out_bits);
	}

	void put(uint32_t bit);
	void put_uint(uint32_t i, uint32_t bits);
	void put_block(void const* ptr_, std::size_t len);

	template<typename T, int BufBytes2>
	void put(basic_ibitstream<T, BufBytes2>& src, int bits);

	void resetp();

	// Finish and align to the buffer size. This will
	// call put_chunk<BufBytes> at most once.
	void finish();

	// Finish with a different byte alignment. This
	// will call put_chunk<C> at most [32/(8*C)] times.
	template<int ByteAlignment, int C>
	void finish_aligned();

	void put_debug_mark();

	bitbuf_t out_bits;
	uint32_t out_bit_count;
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

	void resetp() { not_supported(); }
	void finish() { not_supported(); }
	void clear() { not_supported(); }

	template<int C>
	void finish_aligned() { not_supported(); }

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
	void     resetg() { not_supported(); }

	void get_debug_mark() { not_supported(); }
};

} // namespace gvl

#include "bitstream_templ.hpp"

#endif // UUID_4C44D1D95C5C40B194BE5294E8B984DE
