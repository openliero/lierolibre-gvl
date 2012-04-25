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

#ifndef UUID_1EB2DB01C1DC4C74569FE0B1E099291B
#define UUID_1EB2DB01C1DC4C74569FE0B1E099291B

#include "stream.hpp"
#include <zlib.h>

namespace gvl
{

// TODO: IMPORTANT: apply should return read_ok if it appends a bucket to in_buffer,
// otherwise it should return something else. Make sure this happens everywhere.

struct deflate_filter : filter
{
	deflate_filter(bool compress)
	: compress(compress)
	, ended(false)
	{
		str.zalloc = 0;
		str.zfree = 0;
		str.opaque = 0;

		if(compress)
			deflateInit(&str, Z_DEFAULT_COMPRESSION);
		else
			inflateInit(&str);

		str.avail_in = 0;

		prepare_out_bucket();
	}

	~deflate_filter()
	{
		close();

		if(compress)
			deflateEnd(&str);
		else
			inflateEnd(&str);
	}

	read_status apply(apply_mode mode, size_type amount = 0)
	{
		if(ended)
			return read_blocking;

		bool written = false;

		int deflate_flags = Z_NO_FLUSH;
		if(mode == am_flushing)
			deflate_flags = Z_SYNC_FLUSH;
		else if(mode == am_closing)
		{
			if(compress)
				deflate_flags = Z_FINISH;
			else
				deflate_flags = Z_SYNC_FLUSH;
		}

/*
		if((rand() % 110) == 0)
			throw gvl::stream_error("Random error");
*/
		while(true)
		{
			if(str.avail_in == 0)
			{
				if(filter_buffer.empty())
				{
					if(mode == am_non_pulling)
					{
						break; // Done
					}
					else if(mode == am_pulling)
					{
						read_result res = source->read(amount);
						if(res.s != read_ok)
						{
							if(str.next_out != &buffer->data[0])
							{
								// We have a bucket, fine
								break;
							}
							else
							{
								return res.s;
							}
						}
						else
						{
							prepare_in_bucket(res.b);
						}
					}
				}
				else
				{
					prepare_in_bucket(filter_buffer.unlink_first());
				}
			}

			if(str.avail_out == 0)
			{
				written = true;
				prepare_out_bucket();
				if(mode == am_pulling)
					break;
			}

			/*
				am_non_pulling:
					Just run deflate until there is no more buffered input.
				am_pulling:
					Run deflate until we cannot pull anything more OR
					we have written one bucket.
				am_flushing:
					Run deflate until there is no more buffered input
				am_closing
					Run deflate until Z_STREAM_END is returned
			*/

			int ret;
			if(compress)
				ret = deflate(&str, deflate_flags);
			else
				ret = inflate(&str, deflate_flags);

			if(ret == Z_STREAM_END)
			{
				ended = true;
				break;
			}
			else if(ret != Z_OK)
			{
				printf("avail_in = %d, avail_out = %d\n", str.avail_in, str.avail_out);
				throw stream_error("Error while deflating");
			}

			if((deflate_flags == Z_SYNC_FLUSH) && str.avail_in == 0)
				break;
		}

		// Force away any out_bucket when flushing or closing
		if((mode == am_flushing || mode == am_closing || mode == am_pulling)
		&&  str.next_out != &buffer->data[0])
		{
			// We have a bucket, fine, pass it through and make a new.
			written = true;
			prepare_out_bucket();
		}

		return written ? read_ok : read_blocking;
	}

	void prepare_in_bucket(bucket* in_bucket_new)
	{
		in_bucket.reset(in_bucket_new);
		str.next_in = reinterpret_cast<Bytef*>(const_cast<uint8_t*>(in_bucket->get_ptr()));
		str.avail_in = uInt(in_bucket->size());
	}

	void prepare_out_bucket()
	{
		if(buffer.get())
		{
			buffer->size_ = str.next_out - &buffer->data[0];
			in_buffer.append(new gvl::bucket(buffer.release()));
		}

		std::size_t const buf_size = 1024;
		buffer.reset(gvl::bucket_data_mem::create(buf_size, 0));
		str.avail_out = buf_size;
		str.next_out = reinterpret_cast<Bytef*>(&buffer->data[0]);
	}

	z_stream str;
	std::auto_ptr<bucket_data_mem> buffer;
	std::auto_ptr<bucket> in_bucket;

	bool compress;
	bool ended; // TODO: The stream should have some state for closed
};

}

#endif // UUID_1EB2DB01C1DC4C74569FE0B1E099291B
