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

#include "stream.hpp"

#include "../support/log.hpp"
#include <utility>

namespace gvl
{

bucket::bucket(void const* ptr, size_type len)
{
	bucket_data_mem* data_init = bucket_data_mem::create(len, len);
	std::memcpy(data_init->data, ptr, len);
	data_.reset(data_init);
	begin_ = 0;
	end_ = len;
}


#if 0
stream::write_status octet_stream_writer::partial_flush()
{
	if(!sink_)
		throw stream_write_error(stream::write_error, "No sink assigned to octet_stream_writer");
	stream::write_status stat = stream::write_ok;
	while(!mem_buckets_.empty())
	{
		stream::write_result res = sink_->write(mem_buckets_.first());
		stat = res.s;
		if(!res.consumed)
			break;
	}
	
	// Buffered remaining
	if(!mem_buckets_.empty())
	{
		sink_->write_buffered(mem_buckets_);
	}
	
	return stat;
}
#endif


}
