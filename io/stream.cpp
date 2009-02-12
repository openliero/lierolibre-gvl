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

void stream_writer::flush()
{
	if(size_ > 0)
	{
		if(!sink_)
			throw stream_write_error(stream::write_error, "No sink assigned to stream_writer");
		buffer_->size_ = size_;
		stream::write_result res = sink_->write(new bucket(buffer_.release(), 0, size_));
		sassert(res.s == stream::write_ok);
		// TODO: Check for error/blocking
		cap_ = 32;
		size_ = 0;
		buffer_.reset(bucket_data_mem::create(cap_, size_));
	}
}

void stream_writer::put(bucket* buf)
{
	flush();
	stream::write_result res = sink_->write(buf); // TODO: buf must be singleton
	sassert(res.s == stream::write_ok);
	// TODO: Check for error/blocking
}

}
