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

#ifndef UUID_D366D77D33CE4989790A2D947781E6E3
#define UUID_D366D77D33CE4989790A2D947781E6E3

#include "stream.hpp"

#include <cstdio>
//#include "../support/log.hpp"

namespace gvl
{

struct fstream : stream
{
	fstream(char const* path, char const* mode)
	{
		FILE* f_init = std::fopen(path, mode);
		init(f_init);
	}

	fstream(FILE* f_init)
	{
		init(f_init);
	}

	void init(FILE* f_init)
	{
		f = f_init;
		if(!f)
			throw stream_error("Couldn't open file");
	}

	~fstream()
	{
		if(f)
		{
			std::fclose(f);
		}
	}

	read_result read_bucket(size_type amount = 0, bucket* dest = 0)
	{
		if(!f)
			return read_result(read_error);

		char buf[4096];
		if(amount > 4096 || amount == 0)
			amount = 4096;
		std::size_t read_bytes = std::fread(buf, 1, amount, f);
		if(read_bytes == 0)
			return read_result(read_eos);

		bucket_data_mem* mem = bucket_data_mem::create(read_bytes, read_bytes);
		std::memcpy(mem->data, buf, read_bytes);
		return read_result(read_ok, new bucket(mem, 0, read_bytes));
	}

	write_result write_bucket(bucket* b)
	{
		if(!f)
			return write_result(write_error, false);

		std::size_t len = b->size();
		std::size_t written = std::fwrite(b->get_ptr(), 1, len, f);
		if(written == len)
		{
			unlink(b);
			delete b;
			return write_result(write_ok, true);
		}
		else
		{
			b->cut_front(written);
			return write_result(write_part, false);
		}
	}

	write_status propagate_flush()
	{
		if(!f)
			return write_error;
		std::fflush(f);
		return write_ok;
	}

	write_status propagate_close()
	{
		if(f)
		{
			std::fclose(f);
			f = 0;
		}

		return write_ok;
	}

	read_status seekg(uint64_t pos)
	{
		if(!f)
			return read_error;
		in_buffer.buckets.clear(); // Discard buffered input
		std::fseek(f, (long)pos, SEEK_SET);
		return read_ok;
	}

	FILE* f;
};

}

#endif // UUID_D366D77D33CE4989790A2D947781E6E3
