#include "brigade.hpp"

#include <cstring>


namespace gvl
{

uint8_t const* bucket_mem::get_ptr()
{
	// Data already read
	return vec.data();
}

void bucket_mem::get_vector(linked_vector<uint8_t>& dest)
{
	dest = vec;
}

bucket::read_result bucket_mem::read(std::size_t amount, bucket* dest)
{
	return bucket::read_result(bucket_source::ok, this);
}

void bucket_mem::cut_front(std::size_t amount)
{
	vec.cut_front(amount);
	size_ -= amount;
	sassert(vec.size() == size_);
}

bucket* bucket_mem::clone()
{
	return new bucket_mem(*this);
}

void bucket_writer::flush()
{
	if(!buffer_.empty())
	{
		//sink_->write(new bucket_mem(move(buffer_)));
		sink_->append(new bucket_mem(move(buffer_)));
	}
}

void bucket_writer::put(bucket* buf)
{
	flush();
	sink_->append(buf);
}

/// Convert the first amount bytes to a linked_vector and
/// assigns it to 'res'.
/// Precondition: min_size(l) >= amount
void sequence(list<bucket>& l, std::size_t amount, linked_vector<uint8_t>& res)
{
	// TODO: Explore possible optimizations,
	// such as inserting data into the first bucket if
	// possible.
	std::size_t converted = 0;
	
	uint8_t* p = res.assign(amount);
	
	list<bucket>::iterator i = l.begin(), e = l.end();
	for(; i != e; ++i)
	{
		std::size_t s = i->size();
		std::size_t after = converted + s;
		if(after >= amount)
		{
			std::memcpy(p, i->get_ptr(), amount - converted);
			if(after == amount)
				l.erase(l.begin(), i.next()); // Consumed the whole i, eat it
			else
			{
				// Cut off the front of i
				l.erase(l.begin(), i);
				i->cut_front(amount - converted);
			}

			return;
		}
		std::memcpy(p, i->get_ptr(), s);
		p += s;
		converted = after;
	}
	
	sassert(false);
}

}
