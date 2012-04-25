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

#ifndef UUID_706A361125654B859FE4FABB7193DE74
#define UUID_706A361125654B859FE4FABB7193DE74

#include "bitstream.hpp"
#include "../support/cstdint.hpp"
#include "../containers/deque.hpp"
#include <vector>
#include <stdexcept>

namespace gvl
{

struct vector_bitstream;

template<typename RandomAccessIterator, int BufBytes>
struct range_ibitstream
	: gvl::basic_ibitstream<range_ibitstream<RandomAccessIterator, BufBytes>, BufBytes>
	// obitstream that throws on output operations.
	// Needed to normalize interface for templates (notably uevent<>)
	, gvl::dummy_obitstream
{
	typedef gvl::basic_ibitstream<range_ibitstream<RandomAccessIterator, BufBytes>, BufBytes> ibase;

	range_ibitstream(
		RandomAccessIterator cur,
		RandomAccessIterator end)
	: cur(cur)
	, end(end)
	{
	}

	range_ibitstream(vector_bitstream const&);

	// Default copy ctor and op= are fine

	template<int C>
	unsigned int get_chunk()
	{
		sassert(C == BufBytes);
		if(cur == end)
			throw std::runtime_error("Bitstream exhausted");
		return *cur++;
	}

	template<int C>
	void ignore_chunks(int c)
	{
		sassert(C == BufBytes);
		if(end - cur < c)
			throw std::runtime_error("Bitstream exhausted");
		cur += c;
	}

	std::size_t bits_left() const
	{
		return (end - cur) * 8 + (ibase::in_buf_bits - this->in_bit_count);
	}

	RandomAccessIterator cur;
	RandomAccessIterator end;
};

typedef range_ibitstream<uint32_t const*, 4> array_ibitstream;


template<typename OutputRange, int BufBytes>
struct range_obitstream
	: gvl::basic_obitstream<range_obitstream<OutputRange, BufBytes>, BufBytes>
	, gvl::dummy_ibitstream
{
	typedef gvl::basic_obitstream<range_obitstream<OutputRange, BufBytes>, BufBytes> obase;

	range_obitstream()
	{
	}

	range_obitstream(OutputRange r)
	: range(r)
	{
	}

	// Default copy ctor and op= are fine

	template<int C>
	inline void put_chunk(unsigned int chunk)
	{
		sassert(C == BufBytes);
		range.put(chunk);
	}

	OutputRange range;
};


struct vector_bitstream
	: gvl::basic_ibitstream<vector_bitstream, 4>
	, gvl::basic_obitstream<vector_bitstream, 4>
{
	typedef gvl::basic_ibitstream<vector_bitstream, 4> ibase;
	typedef gvl::basic_obitstream<vector_bitstream, 4> obase;

	vector_bitstream()
	: in_pos(0)
	{
	}

#if GVL_CPP0X
	vector_bitstream(vector_bitstream&& other)
	: ibase(std::move(other))
	, obase(std::move(other))
	, in_pos(other.in_pos)
	, data(std::move(other.data))
	{
		// Destruction is safe already
	}

	vector_bitstream& operator=(vector_bitstream&& other)
	{
		ibase::operator=(std::move(other));
		obase::operator=(std::move(other));

		in_pos = other.in_pos;
		data = std::move(other.data);
		return *this;
	}
#endif

	// NOTE: Caller is expected to verify that the number of bits
	// left in (bs) is at least (size_init).
	vector_bitstream(array_ibitstream& bs, std::size_t size_init)
	: gvl::basic_ibitstream<vector_bitstream>(bs)
	, in_pos(0)
	, data(bs.cur, bs.cur + bs.in_chunks_required(size_init))
	{
	}

	template<int C>
	inline unsigned int get_chunk();

	template<int C>
	inline void put_chunk(unsigned int chunk);

	template<int C>
	inline void ignore_chunks(int c);

	uint32_t size() const
	{
		return out_bits_written(uint32_t(data.size()));
	}

	void swap(vector_bitstream& b)
	{
		gvl::basic_ibitstream<vector_bitstream>::swap(b);
		gvl::basic_obitstream<vector_bitstream>::swap(b);

		std::swap(in_pos, b.in_pos);
		data.swap(b.data);
	}

	// To be able to overload put
	using gvl::basic_obitstream<vector_bitstream>::put;

	// NOTE: Does not consume anything from bs
	void put(vector_bitstream& bs)
	{
		put_uint(bs.in_bits_as_uint(), bs.in_bits_in_buffer());
		for(std::size_t i = bs.in_pos; i < bs.data.size(); ++i)
			put_uint(bs.data[i], 32);
		put_uint(bs.out_bits_as_uint(), bs.out_bits_in_buffer());
	}

	void rewindg()
	{
		resetg();
		in_pos = 0;
	}

	void clear()
	{
		resetp();
		resetg();
		data.clear();
		rewindg();
	}

	std::size_t in_pos;
	std::vector<uint32_t> data;

private:
	// Non-copyable
	vector_bitstream(vector_bitstream const&);
	vector_bitstream& operator=(vector_bitstream const&);
};

template<>
inline unsigned int vector_bitstream::get_chunk<4>()
{
	if(in_pos == data.size())
		throw std::runtime_error("Bitstream exhausted");
	return gvl::bswap_le(data[in_pos++]);
}

template<>
inline void vector_bitstream::put_chunk<4>(unsigned int chunk)
{
	data.push_back(gvl::bswap_le(chunk));
}

template<>
inline void vector_bitstream::ignore_chunks<4>(int c)
{
	if(data.size() - in_pos < c)
		throw std::runtime_error("Bitstream exhausted");
	in_pos += c;
}

template<typename RandomAccessIterator, int BufBytes>
inline range_ibitstream<RandomAccessIterator, BufBytes>::range_ibitstream(vector_bitstream const& bs)
: gvl::basic_ibitstream<range_ibitstream<RandomAccessIterator, BufBytes> >(bs)
, cur(bs.data.empty() ? 0 : (&bs.data[0] + bs.in_pos))
, end(cur + bs.in_chunks_required(bs.size()))
{
}

struct deque_bitstream :
	gvl::basic_ibitstream<deque_bitstream, 4>,
	gvl::basic_obitstream<deque_bitstream, 4>
{
	typedef gvl::basic_ibitstream<deque_bitstream, 4> ibase;
	typedef gvl::basic_obitstream<deque_bitstream, 4> obase;

	deque_bitstream()
	{
	}

#if GVL_CPP0X
	deque_bitstream(deque_bitstream&& other)
	: ibase(std::move(other))
	, obase(std::move(other))
	, data(std::move(other.data))
	{
		// Destruction is safe already
	}

	deque_bitstream& operator=(deque_bitstream&& other)
	{
		ibase::operator=(std::move(other));
		obase::operator=(std::move(other));

		data = std::move(other.data);
		return *this;
	}
#endif

	template<int C>
	inline void put_chunk(unsigned int v);

	template<int C>
	inline unsigned int get_chunk();

	template<int C>
	inline void ignore_chunks(std::size_t c);

	uint32_t size() const
	{
		return out_bits_written(uint32_t(data.size()));
	}

#if 0
	void swap(deque_bitstream& b)
	{
		gvl::basic_obitstream<deque_bitstream>::swap(b);

		data.swap(b.data);
	}
#endif

	// To be able to overload put
	using gvl::basic_obitstream<deque_bitstream>::put;

	// NOTE: Does not consume anything from bs
	void put(vector_bitstream const& bs)
	{
		put_uint(bs.in_bits_as_uint(), bs.in_bits_in_buffer());
		for(std::size_t i = bs.in_pos; i < bs.data.size(); ++i)
			put_uint(bs.data[i], 32);
		put_uint(bs.out_bits_as_uint(), bs.out_bits_in_buffer());
	}

	void clear()
	{
		resetp();
		data.clear();
	}

	gvl::deque<uint32_t> data;

private:
	// Non-copyable
	deque_bitstream(deque_bitstream const&);
	deque_bitstream& operator=(deque_bitstream const&);
};

template<>
inline void deque_bitstream::put_chunk<4>(unsigned int v)
{
	data.push_back(gvl::bswap_le(v));
}

template<>
inline unsigned int deque_bitstream::get_chunk<4>()
{
	if(data.empty())
		throw std::runtime_error("Bitstream exhausted");
	unsigned int v = gvl::bswap_le(data.front());
	data.pop_front();
}

template<>
inline void deque_bitstream::ignore_chunks<4>(std::size_t c)
{
	if(data.size() < c)
		throw std::runtime_error("Bitstream exhausted");
	data.pop_front_n(c);
}


} // namespace gvl

#endif // UUID_706A361125654B859FE4FABB7193DE74
