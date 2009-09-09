#include "encoding.hpp"

namespace gvl
{

stream::write_status octet_stream_writer::flush_buffer(bucket_size new_buffer_size)
{
	std::size_t size = buffer_size_();
	if(size > 0)
	{
		sassert((cap_ & (cap_ - 1)) == 0);
			
		correct_buffer_();
		gvl::list<bucket> b;
		b.push_back(new bucket(buffer_.release(), 0, size));
		
		stream::write_result res = sink_->write_or_buffer(b.first());

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
		
		return res.s;
	}
	
	return stream::write_ok;
}

stream::write_status octet_stream_writer::weak_flush(bucket_size new_buffer_size)
{
	return flush_buffer(new_buffer_size);
}

stream::write_status octet_stream_writer::flush(bucket_size new_buffer_size)
{
	stream::write_status res = flush_buffer(new_buffer_size);
	if(res != stream::write_ok)
		return res;
		
	return sink_->flush();
}

stream::write_status octet_stream_writer::put_bucket(bucket* buf)
{
	stream::write_status res = flush_buffer();

	stream::write_result res2 = sink_->write_or_buffer(buf);
	return res != stream::write_ok ? res : res2.s;
}

}
