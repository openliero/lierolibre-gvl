#include "brigade3.hpp"

namespace gvl
{

void bucket_writer::flush()
{
	if(size_ > 0)
	{
		sink_->write(new bucket(buffer_.release(), 0, size_));
		buffer_.reset(bucket_data_mem::create(32, 0));
		size_ = 0;
	}
}

void bucket_writer::put(bucket* buf)
{
	flush();
	sink_->write(buf); // TODO: buf must be singleton
}

}
