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

#ifndef UUID_64B0E52C08964372A4C2ED8157FCEE5F
#define UUID_64B0E52C08964372A4C2ED8157FCEE5F

#include <stdexcept>
#include <cstring>
#include <string>
#include "../containers/buffer.hpp"
//#include "common.hpp"
#include "../support/debug.hpp"
#include "../support/flags.hpp"
#include "../support/cstdint.hpp"

namespace gvl
{

typedef uint8_t byte;

struct stream_error : std::exception
{
	stream_error(int e)
	{
	}

	virtual char const* what() const throw()
	{
		return "Stream error";
	}

	int e;
};

struct stream_eof : stream_error
{
	stream_eof()
	: stream_error(-1)
	{
	}

	virtual char const* what() const throw()
	{
		return "EOF";
	}
};

struct device_buf
{
	enum
	{
		eof_bit = (1<<0),
		error_bit = (1<<1)
	};

	static std::size_t const in_buffer_size = 1024;
	static std::size_t const eof_value = std::size_t(-1);

	device_buf();

	device_buf(byte const* p, std::size_t s);

	virtual ~device_buf();

	byte*       out_reserve(std::size_t size);
	void        out(byte const* p, std::size_t size);
	inline void out(byte p);
	bool        flush();


	void close();

	inline std::size_t in_available() const;
	bool               in(std::size_t size);
	byte const*        in_buffer() const;
	void               in_consume(std::size_t size);

	void in_pad(std::size_t size);
	void in_direct_fill(byte const* p, std::size_t size);

	bool eof() const;

	void exceptions(unsigned char f)
	{
		throw_on = basic_flags<unsigned char>(f);
	}

	void blocking(void (*in_blocking_new)(device_buf*))
	{
		in_blocking = in_blocking_new;
	}

	bool good() const
	{
		return state.no(eof_bit | error_bit);
	}

protected:
	void check_throw()
	{
		if(state.any(throw_on.as_integer()))
			throw stream_error(0);
	}

	virtual void do_close();
	virtual std::size_t write(byte const* p, std::size_t size) = 0;
	virtual std::size_t read(byte* p, std::size_t size) = 0;

	bool try_read_amount_(std::size_t size);


	buffer out_buf;
	buffer in_buf;

	std::size_t out_buffer_size;
	void (*in_blocking)(device_buf*); // Function to be called when a read request fails because of underflow
	basic_flags<unsigned char> state;
	basic_flags<unsigned char> throw_on;
};

inline void device_buf::out(byte p)
{
	// TODO: If out_buf was resized so that it would be full at the next flush moment,
	// we only need to do one check here.
	out_buf.put(p);
	if(out_buf.size() > out_buffer_size)
		flush(); // Time to flush
}

inline std::size_t device_buf::in_available() const
{
	return in_buf.size();
}

} // namespace gvl

#endif // UUID_64B0E52C08964372A4C2ED8157FCEE5F
