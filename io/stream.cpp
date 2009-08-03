#include "stream.hpp"

#include "../support/log.hpp"

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

void stream_writer::flush_buffer()
{
	if(size_ > 0)
	{
		if(!sink_)
			throw stream_write_error(stream::write_error, "No sink assigned to stream_writer");
		buffer_->size_ = size_;
		mem_buckets_.push_back(new bucket(buffer_.release(), 0, size_));
		cap_ = 32;
		size_ = 0;
		buffer_.reset(bucket_data_mem::create(cap_, size_));
	}
}

void stream_writer::flush()
{
	flush_buffer();
	partial_flush();
}

void stream_writer::partial_flush()
{
	while(!mem_buckets_.empty())
	{
		stream::write_result res = sink_->write(mem_buckets_.first());
		if(!res.consumed)
			break;
	}
}

void stream_writer::put_bucket(bucket* buf)
{
	flush_buffer();
	gvl::unlink(buf);
	mem_buckets_.push_back(buf);
	partial_flush();
}

}
