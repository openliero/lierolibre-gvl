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

void stream_writer::flush_buffer(bucket_size new_buffer_size)
{
	std::size_t size = buffer_size_();
	if(size > 0)
	{
		sassert((cap_ & (cap_ - 1)) == 0);
			
		correct_buffer_();
		mem_buckets_.push_back(new bucket(buffer_.release(), 0, size));
		
		// TODO: Improve
		if(size > estimated_needed_buffer_size_)
			estimated_needed_buffer_size_ <<= 1;
		else if(size + 256 < estimated_needed_buffer_size_)
			estimated_needed_buffer_size_ >>= 1;
		estimated_needed_buffer_size_ = std::max(estimated_needed_buffer_size_, default_initial_bucket_size);
		
		if(new_buffer_size == 0)
			cap_ = estimated_needed_buffer_size_;
		else
		{
			cap_ = 1;
			while(cap_ < new_buffer_size)
				cap_ *= 2;
		}
		cap_ = std::min(cap_, max_bucket_size);

		buffer_.reset(bucket_data_mem::create(cap_, 0));
		
		cur_ = buffer_->data;
		end_ = buffer_->data + cap_;
	}
}

stream::write_status stream_writer::weak_flush(bucket_size new_buffer_size)
{
	flush_buffer(new_buffer_size);
	return partial_flush();
}

stream::write_status stream_writer::flush(bucket_size new_buffer_size)
{
	flush_buffer(new_buffer_size);
	stream::write_status res = partial_flush();
	if(res != stream::write_ok)
		return res;
		
	return sink_->flush();
}

stream::write_status stream_writer::partial_flush()
{
	if(!sink_)
		throw stream_write_error(stream::write_error, "No sink assigned to stream_writer");
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


stream::write_status stream_writer::put_bucket(bucket* buf)
{
	flush_buffer();
	gvl::unlink(buf);
	mem_buckets_.push_back(buf);
	return partial_flush();
}

}
