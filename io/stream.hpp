#ifndef UUID_957E3642BB06466DB21A21AFD72FAFAF
#define UUID_957E3642BB06466DB21A21AFD72FAFAF

#include "../containers/list.hpp"
#include "../support/debug.hpp"
#include "../support/cstdint.hpp"
#include "../support/platform.hpp"
#include "../resman/shared_ptr.hpp"
#include <memory>
#include <vector>
#include <new>
#include <stdexcept>
#include <string>
#include <cstring>

#include <cstdio> // TEMP

namespace gvl
{

struct bucket;
struct brigade;

typedef std::size_t bucket_size;

struct stream_error : std::runtime_error
{
	stream_error(std::string const& msg)
	: std::runtime_error(msg)
	{
	}
};


struct bucket_data : shared
{
	typedef bucket_size size_type;
	static size_type const nsize = size_type(-1);
	
	bucket_data()
	{
	}
	
	virtual size_type size() const = 0;
	
	
	virtual uint8_t const* get_ptr(bucket& owner_bucket, size_type offset) = 0;
	
	// Placement-new
	void* operator new(std::size_t, void* p)
	{
		return p;
	}
	
	// To shut up the compiler
	void operator delete(void*, void*)
	{
	}
	
	// Allocation is overloaded in order to allow arbitrary sized objects
	void* operator new(std::size_t n)
	{
		return new char[n];
	}
	
	void operator delete(void* m)
	{
		delete [] static_cast<char*>(m);
	}
};

struct bucket : list_node<>
{
	typedef bucket_size size_type;
	
	bucket()
	: begin_(0)
	, end_(-1)
	{
	}
	
	bucket(bucket const& b, std::size_t begin, std::size_t end)
	: data_(b.data_)
	, begin_(begin)
	, end_(end)
	{
		
	}
	
	bucket(bucket_data* data)
	: data_(data)
	, begin_(0)
	, end_(0)
	{
		end_ = data_->size();
		if(end_ == bucket_data::nsize)
		{
			end_ = 0;
			begin_ = 1;
		}
	}
	
	bucket(bucket_data* data, size_type begin, size_type end)
	: data_(data)
	, begin_(begin)
	, end_(end)
	{
	}
	
	bucket(void const* ptr, size_type len);
			
	bool size_known() const { return begin_ <= end_; }
	
	size_type begin() const
	{
		passert(size_known(), "Size is unknown");
		return begin_;
	}
	
	size_type end() const
	{
		passert(size_known(), "Size is unknown");
		return end_;
	}
	
	size_type size() const
	{
		passert(size_known(), "Size is unknown");
		return static_cast<size_type>(end_ - begin_);
	}
	
	void split(std::size_t point)
	{
		passert(size_known(), "Size is unknown");
		passert(0 <= point && point <= size(), "Split point is out of bounds");
		
		if(point == 0 || point == size())
			return; // No need to do anything
				
		// Insert before
		// TODO: We should actually let the bucket split itself.
		// It should at least know when it's being copied.
		relink_after(this, new bucket(*this, begin_ + point, end_));
		end_ -= (size() - point);
	}
	
	/*
	/// Returns a pointer to memory for the data of the bucket.
	/// This function may convert the bucket
	uint8_t const* get_ptr()
	{
		// Default definition throws an error
		passert(size_known_, "Size is unknown");
		passert(!size_known_, "Buckets with known size must define get_ptr()");
		throw "TODO: get_ptr() not implemented";
	}*/
	
	uint8_t const* get_ptr()
	{
		return data_->get_ptr(*this, begin_);
	}
		
	void cut_front(size_type amount)
	{
		passert(size_known(), "Size is unknown");
		begin_ += amount;
		passert(begin_ <= end_, "Underflow");
	}
	
	void cut_back(size_type amount)
	{
		passert(size_known(), "Size is unknown");
		end_ -= amount;
		passert(begin_ <= end_, "Underflow");
	}
	
	bucket* clone() const
	{
		return new bucket(*this, begin_, end_);
	}
	
	~bucket()
	{
	}
	
	
protected:

	shared_ptr<bucket_data> data_;
	std::size_t begin_;
	std::size_t end_;
};

struct brigade
{
	typedef bucket_size size_type;
	
	/*
	read_result read(size_type amount = 0, bucket* dest = 0)
	{
		list<bucket>::iterator i = buckets.begin();
		
		if(i == buckets.end())
			return read_result(eos);
			
		read_result r(i->read(amount, 0));
		if(r.s == bucket_source::ok)
			buckets.unlink(r.b); // Success, we may unlink the bucket
			
		return r;
	}*/

	void prepend(bucket* b)
	{
		buckets.relink_front(b);
	}
	
	void append(bucket* b)
	{
		buckets.relink_back(b);
	}
	
	bool empty() const
	{
		return buckets.empty();
	}
	
	bucket* first()
	{
		return buckets.first();
	}
	
	bucket* unlink_first()
	{
		bucket* ret = buckets.first();
		buckets.unlink_front();
		return ret;
	}
	
	bucket* unlink_last()
	{
		bucket* ret = buckets.last();
		buckets.unlink_back();
		return ret;
	}
	
	list<bucket> buckets;
};

struct stream : shared
{
	typedef bucket_size size_type;
	
	enum read_status
	{
		read_ok = 0,
		read_blocking,
		read_eos,
		read_error,
		
		read_none_done // Special for filters
	};
	
	enum write_status
	{
		write_ok = 0,
		write_part,
		write_would_block,
		write_error
	};
	
	enum state
	{
		state_open,
		state_closed
	};
	
	struct read_result
	{
		explicit read_result(read_status s)
		: s(s)
		, b(0)
		{
		}
		
		read_result(read_status s, bucket* b)
		: s(s)
		, b(b)
		{
		}
		
		read_status s;
		bucket* b;
	};
	
	struct write_result
	{
		explicit write_result(write_status s, bool consumed)
		: s(s)
		, consumed(consumed)
		{
		
		}
		
		write_status s;
		bool consumed;
	};
	
	stream()
	: cur_state(state_open)
	{
	}
	
	~stream()
	{
		// We don't close here, because derived objects are already destroyed
	}
	
	virtual read_result read_bucket(size_type amount = 0, bucket* dest = 0) = 0;
	/*
	{
		return read_result(read_blocking);
	}*/
	
	read_result read(size_type amount = 0)
	{
		if(!in_buffer.empty())
			return read_result(read_ok, in_buffer.unlink_first());

		return read_bucket(amount);
	}
	
	// Unread buckets so that subsequent calls to read will
	// return them.
	void unread(list<bucket>& buckets)
	{
		in_buffer.buckets.splice_front(buckets);
	}
	
	void unread(bucket* b)
	{
		in_buffer.buckets.relink_front(b);
	}
	
	/// Writes a bucket to a sink. If bucket_sink::ok is returned,
	/// takes ownership of 'b' and unlinks it from it's list<bucket>.
	/// NOTE: 'b' must be inserted into a list<bucket> or be a singleton.
	virtual write_result write_bucket(bucket* b) = 0;
	/*
	{
		return write_result(write_would_block, false);
	}*/
	
	write_result write(bucket* b)
	{
		write_result res = flush_out_buffer();
		if(!res.consumed)
			return res;

		return write_bucket(b);
	}
	
	/// NOTE: 'b' must be inserted into a list<bucket> or be a singleton.
	write_result write_or_buffer(bucket* b)
	{
		write_result res = write_bucket(b);
		if(!res.consumed)
			write_buffered(b);
		res.consumed = true;
		return res;
	}
	
	// Buffer a list of buckets.
	void write_buffered(list<bucket>& buckets)
	{
		out_buffer.buckets.splice(buckets);
	}
		
	/// NOTE: 'b' must be inserted into a list<bucket> or be a singleton.
	void write_buffered(bucket* b)
	{
		gvl::unlink(b);
		out_buffer.buckets.relink_front(b);
	}
	
	write_status flush()
	{
		write_result res = flush_out_buffer();
		if(!res.consumed)
			return res.s;
		
		return propagate_flush();
	}
	
	write_result flush_out_buffer()
	{
		while(!out_buffer.empty())
		{
			bucket* buffered = out_buffer.first();
			write_result res = write_bucket(buffered);
			if(!res.consumed)
				return res;
		}
		
		return write_result(write_ok, true);
	}
	
	// NOTE! This may NEVER throw!
	write_status close()
	{
		if(cur_state != state_open)
			return write_ok;
			
		cur_state = state_closed;
			
		write_result res = flush_out_buffer();
		if(!res.consumed)
			return res.s;
		
		return propagate_close();
	}
		
	/// This is supposed to propagate a flush to
	/// the underlying sink. E.g. in a filter, it would
	/// propagate the flush to the connected sink.
	virtual write_status propagate_flush()
	{
		return write_ok;
	}
	
	virtual write_status propagate_close()
	{
		return write_ok;
	}
	
	brigade in_buffer;
	brigade out_buffer;
	state cur_state;
};


struct stream_read_error : stream_error
{
	stream_read_error(stream::read_status s, std::string const& msg)
	: stream_error(msg), s(s)
	{
	}
	
	stream::read_status s;
};

struct stream_write_error : stream_error
{
	stream_write_error(stream::write_status s, std::string const& msg)
	: stream_error(msg), s(s)
	{
	}
	
	stream::write_status s;
};

typedef shared_ptr<stream> stream_ptr;


struct bucket_data_mem : bucket_data
{
/*
	bucket_data_mem(bucket_data_mem const& other)
	: bucket(other)
	, vec(other.vec)
	{
	}*/
	
	//struct swap_tag {};
	
	static std::size_t compute_size(bucket_size n)
	{
		return sizeof(bucket_data_mem) + n*sizeof(uint8_t) - 1*sizeof(uint8_t);
	}
	
	static bucket_data_mem* create(bucket_size capacity, bucket_size size)
	{
		sassert(size <= capacity);
		void* mem = new char[compute_size(capacity)];
		return new (mem) bucket_data_mem(size);
	}
	
	/*
	bucket_data_mem(std::vector<uint8_t> const& other)
	: vec(other)
	{
	}
	
	bucket_data_mem(std::vector<uint8_t>& other, swap_tag)
	{
		vec.swap(other);
	}
	*/
	
	bucket_data_mem(bucket_size size)
	: size_(size)
	{
	}
	
	uint8_t const* get_ptr(bucket& owner_bucket, size_type offset)
	{
		return data + offset;
	}
	
	size_type size() const
	{
		return size_;
	}
	
	void unsafe_push_back(uint8_t el)
	{
		data[size_] = el;
		++size_;
	}
	
	void unsafe_push_back(uint8_t const* p, bucket_size len)
	{
		std::memcpy(&data[size_], p, len);
		size_ += len;
	}
		
	bucket_data_mem* enlarge(bucket_size n)
	{
		bucket_data_mem* new_data = create(n, size_);
		std::memcpy(new_data->data, data, size_);
		//new_data->size_ = size_;
		return new_data;
	}
	
	std::size_t size_;
	uint8_t data[1];
};

//void sequence(list<bucket>& l, std::size_t amount, linked_vector<uint8_t>& res);

// Provides functions for extracting data
// from a stream in a convenient and
// efficient manner.
// NOTE: You are not allowed to modify buckets
// that are buffered.
struct stream_reader
{
	typedef bucket::size_type size_type;
	
	stream_reader(shared_ptr<stream> source_init)
	: cur_(0)
	, end_(0)
	, source_(source_init)
	{
	}
	
	stream_reader()
	: cur_(0)
	, end_(0)
	, source_()
	{
	}
	
/*
	// Different naming to avoid infinite recursion if
	// not defined in DerivedT.
	bucket_source* get_source()
	{ return derived()->source(); }
	*/
	
	/*
	shared_ptr<stream> get_source()
	{ return source_; }*/
	
	uint8_t get()
	{
		// We keep this function small to encourage
		// inlining
		return (cur_ != end_) ? (*cur_++) : underflow_get_();
	}
	
	void get_c(uint8_t* dest, std::size_t len)
	{
		// TODO: Can optimize this
		for(std::size_t i = 0; i < len; ++i)
			dest[i] = get();
	}
	
	stream::read_status get(uint8_t& ret)
	{
		if(cur_ != end_)
		{
			ret = *cur_++;
			return stream::read_ok;
		}
		else
		{
			return underflow_get_(ret);
		}
	}
	
	bool at_eos()
	{
		if(cur_ != end_)
			return false;
			
		stream::read_status status = next_bucket_();
		if(status == stream::read_eos)
			return true;
	
		return false; // TODO: Is erroring count as eos?	
	}
	
	// TODO: A get that returns a special value for EOF

	// TODO: This returned an auto_read_result before
	stream::read_result get_bucket(size_type amount = 0)
	{
		if(first_.get())
		{
			correct_first_bucket_();
			return stream::read_result(stream::read_ok, first_.release());
		}
		else
			return read_bucket_and_return_(amount);
	}
	
	// Non-blocking
	stream::read_result try_get_bucket(size_type amount = 0)
	{
		if(first_.get())
		{
			correct_first_bucket_();
			return stream::read_result(stream::read_ok, first_.release());
		}
		else
			return try_read_bucket_and_return_(amount);
	}
	
	shared_ptr<stream> detach()
	{
		correct_first_bucket_();

		shared_ptr<stream> ret = source_.release();
		source_->unread(first_.release());
		cur_ = end_ = 0;
		sassert(cur_ == end_);
		sassert(!first_.get());
		
		return ret;
	}
	
	void attach(shared_ptr<stream> source_new)
	{
		if(source_)
			throw stream_error("A source is already attached to this stream_reader");
		
		source_ = source_new;
	}
	
	/// Amount of data left in the first bucket
	std::size_t first_left() const { return end_ - cur_; }
	
private:
	
	uint8_t underflow_get_()
	{
		stream::read_status status = next_bucket_();
		if(status != stream::read_ok)
			throw stream_read_error(status, "Read error in get()");
		
		return *cur_++;
	}
	
	stream::read_status underflow_get_(uint8_t& ret)
	{
		stream::read_status s = next_bucket_();
		if(s != stream::read_ok)
			return s;
		
		ret = *cur_++;
		return stream::read_ok;
	}
	
	/// Discards the current first bucket (if any) and tries to read
	/// a bucket if necessary.
	/// May throw.
	/// Precondition: cur_ == end_
	stream::read_status next_bucket_()
	{
		passert(cur_ == end_, "Still data in the first bucket");
		check_source();
		
		// Need to read a bucket
		
		// Reset first
		// No need to do this: cur_ = end_ = 0;
		first_.reset();
		
		//while(true)
		{
			stream::read_result r(source_->read());

			if(r.s == stream::read_ok)
			{
				// Callers of next_bucket_ expect the result
				// in first_
				set_first_bucket_(r.b);
				return stream::read_ok;
			}
			else if(r.s == stream::read_eos)
			{
				return stream::read_eos;
			}
			
			// TODO: derived()->block();
		}
		
		return stream::read_blocking;
	}
	
	void check_source()
	{
		if(!source_)
			throw stream_read_error(stream::read_error, "No source assigned to stream_reader");
	}
		
	// May throw
	stream::read_result read_bucket_and_return_(size_type amount)
	{
		check_source();
		//while(true)
		{
			stream::read_result r(source_->read(amount));
		
			if(r.s != stream::read_blocking)
				return r;
			
			/* TODO:
			derived()->flush();
			derived()->block();
			*/
		}
		
		return stream::read_result(stream::read_blocking);
	}
	
	// May throw
	stream::read_result try_read_bucket_and_return_(size_type amount)
	{
		check_source();
		return source_->read(amount);
	}
		
	/// Apply changes to first bucket
	void correct_first_bucket_()
	{
		if(first_.get())
		{
			std::size_t old_size = first_->size();
			first_->cut_front(old_size - first_left());
		}
	}
	
	void set_first_bucket_(bucket* b)
	{
		//passert(!first_.get(), "Still a bucket in first_");
		size_type s = b->size();
		
		first_.reset(b);
		// New first bucket, update cur_ and end_
		cur_ = b->get_ptr();
		end_ = cur_ + s;
	}
	
	uint8_t const* cur_; // Pointer into first_
	uint8_t const* end_; // End of data in first_
	std::auto_ptr<bucket> first_;
	shared_ptr<stream> source_;
};

struct brigade;

struct stream_writer
{
	static bucket_size const default_initial_bucket_size = 512;
	static bucket_size const max_bucket_size = 32768;
	
	stream_writer(shared_ptr<stream> sink)
	: sink_(sink)
	, cur_(0)
	, end_(0)
	, cap_(default_initial_bucket_size)
	, buffer_(bucket_data_mem::create(cap_, 0))
	{
		read_in_buffer_();
	}
	
	stream_writer()
	: sink_()
	//, size_(0)
	, cur_(0)
	, end_(0)
	, cap_(0)
	, buffer_()
	, estimated_needed_buffer_size_(default_initial_bucket_size)
	{
	}
	
	~stream_writer()
	{
		if(sink_)
			flush();
	}
	
	stream::write_status flush(bucket_size new_buffer_size = 0);
	stream::write_status weak_flush(bucket_size new_buffer_size = 0);
	//stream::write_status partial_flush();

	stream::write_status put(uint8_t b)
	{
		// We keep this function small to encourage
		// inlining of the common case
		return (cur_ != end_) ? (*cur_++ = b, stream::write_ok) : overflow_put_(b);
	}
	
	stream::write_status put(uint8_t const* p, std::size_t len)
	{
		// We keep this function small to encourage
		// inlining of the common case
		if(std::size_t(end_ - cur_) >= len)
		{
#if GVL_X86 || GVL_X86_64 // TODO: A define that says whether unaligned access is allowed
			if(len < 64) // TODO: Tweak this limit
			{
				while(len > 4)
				{
					*reinterpret_cast<uint32_t*>(cur_) = *reinterpret_cast<uint32_t const*>(p);
					len -= 4;
					cur_ += 4;
					p += 4;
				}
				while(len--)
					*cur_++ = *p++;
					
				return stream::write_ok;
			}
#endif
			std::memcpy(cur_, p, len);
			cur_ += len;
			return stream::write_ok;
		}
		else
		{
			return overflow_put_(p, len);
		}
	}
	
	stream::write_status put_bucket(bucket* buf);
	
	shared_ptr<stream> detach()
	{
		flush_buffer();
		
		// Buffer any remaining buckets
		// partial_flush already does this: sink_->write_buffered(mem_buckets_);
		
		return sink_.release();
	}
	
	void attach(shared_ptr<stream> new_sink)
	{
		if(sink_)
			throw stream_error("A sink is already attached to the stream_writer");
		sink_ = new_sink;
		cap_ = default_initial_bucket_size;
		buffer_.reset(bucket_data_mem::create(cap_, 0));
		read_in_buffer_();
	}
	
	void check_sink()
	{
		if(!sink_)
			throw stream_write_error(stream::write_error, "No sink assigned to stream_writer");
	}
	
private:
	stream::write_status flush_buffer(bucket_size new_buffer_size = 0);
	
	std::size_t buffer_size_()
	{
		return (cur_ - buffer_->data);
	}
	
	void correct_buffer_()
	{
		buffer_->size_ = buffer_size_();
	}
	
	void read_in_buffer_()
	{
		cur_ = buffer_->data + buffer_->size_;
		end_ = buffer_->data + cap_;
	}
	
	stream::write_status overflow_put_(uint8_t b)
	{
		check_sink();
		
		if(buffer_size_() >= max_bucket_size)
		{
			stream::write_status ret = weak_flush();
			sassert(cur_ != end_);
			*cur_++ = b;
			return ret;
		}
		else
		{
			correct_buffer_();
			cap_ *= 2;
			buffer_.reset(buffer_->enlarge(cap_));
			buffer_->unsafe_push_back(b);
			
			read_in_buffer_();
			return stream::write_ok;
		}
	}
	
	
	stream::write_status overflow_put_(uint8_t const* p, std::size_t len)
	{
		check_sink();
		
		// As long as fitting in the current buffer would make it
		// too large, write as much as possible and flush.
		while((cur_ - buffer_->data) + len >= max_bucket_size)
		{
			std::size_t left = end_ - cur_;
			// Copy as much as we can
			std::memcpy(cur_, p, left);
			cur_ += left;
			p += left;
			len -= left;
			
			// Flush and try to allocate a buffer large enough for the rest of the data
			stream::write_status ret = weak_flush(len);
			if(ret != stream::write_ok)
				return ret;
		}
		
		// Write the rest
		ensure_cap_((cur_ - buffer_->data) + len);
		
		std::memcpy(cur_, p, len);
		cur_ += len;
		return stream::write_ok;
	}
	
	void ensure_cap_(std::size_t s)
	{
		if(cap_ < s)
		{
			correct_buffer_();
			while(cap_ < s)
				cap_ *= 2;
			buffer_.reset(buffer_->enlarge(cap_));
			//sassert(size_ == buffer_->size_);
			cur_ = buffer_->data + buffer_->size_;
			end_ = buffer_->data + cap_;
			sassert(std::size_t(cur_ - buffer_->data) == buffer_->size_);
		}
	}

	//bucket_size size_;
	shared_ptr<stream> sink_;
	uint8_t* cur_; // Pointer into buffer_
	uint8_t* end_; // End of capacity in buffer_
	bucket_size cap_;
	//list<bucket> mem_buckets_;
	std::auto_ptr<bucket_data_mem> buffer_;
	std::size_t estimated_needed_buffer_size_;
};

inline stream_writer& operator<<(stream_writer& writer, char const* str)
{
	std::size_t len = std::strlen(str);
	writer.put(reinterpret_cast<uint8_t const*>(str), len);
	return writer;
}

inline stream_writer& operator<<(stream_writer& writer, std::string const& str)
{
	writer.put(reinterpret_cast<uint8_t const*>(str.data()), str.size());
	return writer;
}

/*
/// A sink that forwards to a brigade
template<typename DerivedT, brigade& (DerivedT::*Get)()>
struct basic_brigade_sink : bucket_sink
{
	brigade& get()
	{ return (static_cast<DerivedT*>(this)->*Get)(); }
	
	bucket_sink::status write(bucket* b)
	{
		b->unlink();
		get().append(b);
		return bucket_sink::ok;
	}
};
*/



struct filter : stream
{
	struct pump_result
	{
		pump_result(read_status r, write_status w)
		: r(r), w(w)
		{
		}
		
		read_status r;
		write_status w;
	};
	
	enum apply_mode
	{
		am_non_pulling,
		am_pulling,
		am_flushing,
		am_closing
	};
	
	filter()
	{
	}
		
	filter(shared_ptr<stream> source_init, shared_ptr<stream> sink_init)
	: source(source_init)
	, sink(sink_init)
	{
	}
	
	read_result read_bucket(size_type amount = 0, bucket* dest = 0)
	{
		if(!source)
			return read_result(read_error);
		
		read_status status = apply(am_pulling, amount);
		if(status != read_ok)
			return read_result(status);
		read_result res(read_ok, in_buffer.unlink_first());
		return res;
	}
	
	write_result write_bucket(bucket* b)
	{
		if(!sink)
			return write_result(write_error, false);
		
		unlink(b);
		filter_buffer.append(b);
		apply(am_non_pulling);
		write_status rstatus = flush_filtered();
		if(rstatus != write_ok)
		{
			return write_result(rstatus, true);
		}
		
		return write_result(write_ok, true);
	}
	
	
	
	write_status propagate_flush()
	{
		sassert(out_buffer.empty()); // stream should have taken care of this
		
		// We don't check sink here so that
		// we only error on missing sink if there's actually anything
		// left to write.
		
		if(!sink)
		{
			if(!out_buffer.empty() || !filter_buffer.empty())
				return write_error; // Still data to filter or not written
		}
		else
		{
			apply(am_flushing);
			write_status res = flush_filtered();
			if(res != write_ok)
				return res;
			if(!filter_buffer.empty())
				return write_would_block; // Still data that has not been filtered
		}
		return write_ok;
	}
	
	write_status propagate_close()
	{
		sassert(out_buffer.empty());
		
		if(!sink)
		{
			if(!out_buffer.empty() || !filter_buffer.empty())
				return write_error; // Still data to filter or not written
		}
		else
		{
			apply(am_closing);
			write_status res = flush_filtered();
			if(res != write_ok)
				return res;
			if(!filter_buffer.empty())
				return write_would_block; // Still data that has not been filtered
		}
		return write_ok;
	}
	
	pump_result pump()
	{
		if(!source)
			return pump_result(read_error, write_error);
		if(!sink)
			return pump_result(read_error, write_error);
			
		if(in_buffer.empty())
		{
			read_status rstatus = apply(am_pulling);
			if(rstatus != read_ok)
				return pump_result(rstatus, write_ok);
		}
		write_status wstatus = flush_filtered();
		return pump_result(read_ok, wstatus);
	}
	
	void attach_source(shared_ptr<stream> source_new)
	{
		source = source_new;
	}
	
	void attach_sink(shared_ptr<stream> sink_new)
	{
		sink = sink_new;
	}
		
protected:

	// Preconditions: sink
	write_status flush_filtered()
	{
		sassert(sink); // Precondition
		
		while(!in_buffer.empty())
		{
			write_result res = sink->write(in_buffer.first());
			if(res.s != write_ok)
			{
				return res.s;
			}
		}
		
		return write_ok;
	}
	
	

	// Filter buckets in filter_buffer and append the result to in_buffer.
	// If mode is flushing or closing, the filter should make every effort to
	// filter all buckets in filter_buffer.
	// 
	// If mode is pulling, the filter should make some effort to produce
	// at least one filtered bucket.
	// 
	// TODO: Return value of this function is quite useless at the moment.
	virtual read_status apply(apply_mode mode, size_type amount = 0)
	{
		// We bypass filter_buffer
		if(!out_buffer.empty())
		{
			in_buffer.buckets.splice(out_buffer.buckets);
			return read_ok;
		}
		else if(mode == am_pulling)
		{
			read_result res = source->read(amount);
			if(res.s == read_ok)
				in_buffer.append(res.b);
			return res.s;
		}
		else
			return read_blocking;
	}
	
	read_status try_pull(size_type amount = 0)
	{
		read_result res = source->read(amount);
		if(res.s == read_ok)
			filter_buffer.append(res.b);
		return res.s;
	}
		
	shared_ptr<stream> source;
	shared_ptr<stream> sink;

	brigade filter_buffer;
};

typedef shared_ptr<filter> filter_ptr;

struct brigade_buffer : stream
{
	read_result read_bucket(size_type amount = 0, bucket* dest = 0)
	{
	/* stream::read already checked in_buffer
		if(!buffer.empty())
		{
			read_result res(read_ok, buffer.first());
			unlink(res.b);
			return res;
		}
		*/
		return read_result(read_blocking);
	}
	
	write_result write_bucket(bucket* b)
	{
		unlink(b);
		in_buffer.append(b);
		return write_result(write_ok, true);
	}
	
	void clear()
	{
		in_buffer.buckets.clear();
	}
	
	void to_str(std::string& ret)
	{
		ret.clear();
		for(list<bucket>::iterator i = in_buffer.buckets.begin(); i != in_buffer.buckets.end(); ++i)
		{
			char const* p = reinterpret_cast<char const*>(i->get_ptr());
			ret.insert(ret.end(), p, p + i->size());
		}
	}
};

template<typename Writer>
inline void write_uint16(Writer& writer, unsigned int v)
{
	sassert(v < 0x10000);
	writer.put(uint8_t((v >> 8) & 0xff));
	writer.put(uint8_t(v & 0xff));
}

template<typename Writer>
inline void write_uint16_le(Writer& writer, unsigned int v)
{
	sassert(v < 0x10000);
	writer.put(uint8_t(v & 0xff));
	writer.put(uint8_t((v >> 8) & 0xff));
}

template<typename Writer>
inline void write_uint32(Writer& writer, uint32_t v)
{
	writer.put(uint8_t((v >> 24) & 0xff));
	writer.put(uint8_t((v >> 16) & 0xff));
	writer.put(uint8_t((v >> 8) & 0xff));
	writer.put(uint8_t(v & 0xff));
}

template<typename Writer>
inline void write_uint32_le(Writer& writer, uint32_t v)
{
	writer.put(uint8_t(v & 0xff));
	writer.put(uint8_t((v >> 8) & 0xff));
	writer.put(uint8_t((v >> 16) & 0xff));
	writer.put(uint8_t((v >> 24) & 0xff));
}

template<typename Reader>
inline unsigned int read_uint16(Reader& reader)
{
	unsigned int ret = reader.get() << 8;
	ret |= reader.get();
	return ret;
}

template<typename Reader>
inline unsigned int read_uint16_le(Reader& reader)
{
	unsigned int ret = reader.get();
	ret |= reader.get() << 8;
	return ret;
}

template<typename Reader>
inline uint32_t read_uint32(Reader& reader)
{
	unsigned int ret = reader.get() << 24;
	ret |= reader.get() << 16;
	ret |= reader.get() << 8;
	ret |= reader.get();
	return ret;
}

template<typename Reader>
inline uint32_t read_uint32_le(Reader& reader)
{
	unsigned int ret = reader.get();
	ret |= reader.get() << 8;
	ret |= reader.get() << 16;
	ret |= reader.get() << 24;
	return ret;
}

}

#endif // UUID_957E3642BB06466DB21A21AFD72FAFAF
