#ifndef UUID_957E3642BB06466DB21A21AFD72FAFAF
#define UUID_957E3642BB06466DB21A21AFD72FAFAF

#include "../containers/list.hpp"
#include "../support/debug.hpp"
#include "../support/cstdint.hpp"
#include "../resman/shared_ptr.hpp"
#include <memory>
#include <vector>
#include <new>
#include <stdexcept>
#include <string>

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
	
	~stream()
	{
		close();
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
	
	// Buffer a list of buckets.
	void write_buffered(list<bucket>& buckets)
	{
		out_buffer.buckets.splice(buckets);
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
	
	write_status close()
	{
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
	: bucket_list_size_(0)
	, cur_(0)
	, end_(0)
	, source_(source_init)
	{
	}
	
	stream_reader()
	: bucket_list_size_(0)
	, cur_(0)
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
	
	size_type read_size()
	{
		return bucket_list_size_ + (end_ - cur_);
	}
	
	uint8_t get()
	{
		// We keep this function small to encourage
		// inlining
		return (cur_ != end_) ? (*cur_++) : underflow_get_();
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
	
	// TODO: A get that returns a special value for EOF
		
	stream::read_status buffer(size_type amount)
	{
		size_type cur_read = read_size();
		while(cur_read < amount)
		{
			stream::read_status s = read_bucket_(amount - cur_read);
			if(s != stream::read_ok)
				return s;
			cur_read = read_size();
		}
		
		return stream::read_ok;
	}
	
	/* TODO
	bucket::status buffer_sequenced(size_type amount, linked_vector<uint8_t>& res)
	{
		bucket::status s = buffer(amount);
		if(s != bucket::ok)
			return s;
		reinsert_first_bucket();
		
		sequence(mem_buckets_, amount, res);
		return bucket::ok;
	}*/
	
	// TODO: This returned an auto_read_result before
	stream::read_result get_bucket(size_type amount = 0)
	{
		if(first_.get())
		{
			correct_first_bucket_();
			return stream::read_result(stream::read_ok, first_.release());
		}
		else if(!mem_buckets_.empty())
		{
			return stream::read_result(stream::read_ok, pop_bucket_());
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
		else if(!mem_buckets_.empty())
		{
			return stream::read_result(stream::read_ok, pop_bucket_());
		}
		else
			return try_read_bucket_and_return_(amount);
	}
	
	/*
	void assign(shared_ptr<stream> source_new)
	{
		bucket_list_size_ = 0;
		cur_ = 0;
		end_ = 0;
		first_.reset();
		mem_buckets_.clear();
		source_ = source_new;
	}*/
	
	shared_ptr<stream> detach()
	{
		reinsert_first_bucket();

		shared_ptr<stream> ret = source_;
		source_->unread(mem_buckets_);
		
		bucket_list_size_ = 0;
		source_.reset();
		sassert(cur_ == end_);
		sassert(!first_.get());
		sassert(mem_buckets_.empty());
		
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
	/// Precondition: cur_ == end_
	stream::read_status next_bucket_()
	{
		passert(cur_ == end_, "Still data in the first bucket");
		check_source();
		
		if(!mem_buckets_.empty())
		{
			//first_.reset(pop_bucket_());
			set_first_bucket_(pop_bucket_());
			return stream::read_ok;
		}
		
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
	
	stream::read_status read_bucket_(size_type amount)
	{
		check_source();
		//while(true)
		{
			stream::read_result r(source_->read(amount));
		
			if(r.s == stream::read_ok)
			{
				add_bucket_(r.b);
				return stream::read_ok;
			}
			else if(r.s == stream::read_eos)
			{
				return stream::read_eos;
			}
			
			/* TODO:
			derived()->flush();
			derived()->block();
			*/
		}
		
		return stream::read_blocking;
	}
	
	void check_source()
	{
		if(!source_)
			throw stream_read_error(stream::read_error, "No source assigned to stream_reader");
	}
	
	stream::read_status try_read_bucket_(size_type amount)
	{
		check_source();
			
		stream::read_result r(source_->read(amount));
	
		if(r.s == stream::read_ok)
		{
			add_bucket_(r.b);
			return stream::read_ok;
		}
		
		return r.s;
	}
	
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
	
	stream::read_result try_read_bucket_and_return_(size_type amount)
	{
		check_source();
		return source_->read(amount);
	}
	
	bucket* pop_bucket_()
	{
		// Let caller take care of this: passert(!first_.get(), "Still a bucket in first_");
		passert(!mem_buckets_.empty(), "mem_buckets_ is empty");
		
		bucket* b = mem_buckets_.first();
		mem_buckets_.unlink_front();

		size_type s = b->size();
		bucket_list_size_ -= s;
		return b;
	}
	
	/// Apply changes to first bucket
	void correct_first_bucket_()
	{
		if(first_.get())
		{
			bucket* b = first_.release();
			std::size_t old_size = b->size();
			b->cut_front(old_size - first_left());
			//mem_buckets_.relink_front(b);
		}
	}
	
	/// Apply changes to first bucket and put it back into mem_buckets_
	void reinsert_first_bucket()
	{
		if(first_.get())
		{
			bucket* b = first_.release();
			size_type old_size = b->size();
			b->cut_front(old_size - first_left());
			cur_ = end_ = 0;
			mem_buckets_.relink_front(b);
		}
	}
	
	void set_first_bucket_(bucket* b)
	{
		//passert(!first_.get(), "Still a bucket in first_");
		passert(mem_buckets_.empty(), "Still buckets in mem_buckets_");
		size_type s = b->size();
		
		first_.reset(b);
		// New first bucket, update cur_ and end_
		cur_ = b->get_ptr();
		end_ = cur_ + s;
		passert(bucket_list_size_ == 0, "Incorrect bucket_list_size_");
	}
	
	void add_bucket_(bucket* b)
	{
		size_type s = b->size();
		
		mem_buckets_.relink_back(b);
		bucket_list_size_ += s;
	}
	
#if 0 // Not needed (yet)
	/// To be used when there are no buckets left
	void add_bucket_empty_(bucket* b)
	{
		passert(!first_.get(), "Still a bucket in first_");
		passert(mem_buckets_.empty(), "Still buckets in mem_buckets_");
		size_type s = b->size();
		
		// TODO: Which to prefer here? Insertion into first or the beginning
		// of mem_buckets_?
#if 0
		first_.reset(b);
		// New first bucket, update cur_ and end_
		cur_ = b->get_ptr();
		end_ = cur_ + s;
		passert(bucket_list_size_ == 0, "Incorrect bucket_list_size_");
#else
		mem_buckets_.relink_back(b);
		bucket_list_size_ += s; // Buffered bucket
#endif
	}
#endif

	
	
	// Total size of buckets in mem_buckets_
	size_type bucket_list_size_;
	
	uint8_t const* cur_; // Pointer into first_
	uint8_t const* end_; // End of data in first_
	std::auto_ptr<bucket> first_;
	list<bucket> mem_buckets_;
	shared_ptr<stream> source_;
};

struct brigade;

struct stream_writer
{
	stream_writer(shared_ptr<stream> sink)
	: sink_(sink)
	, size_(0)
	, cap_(32)
	, buffer_(bucket_data_mem::create(32, 0))
	{
	}
	
	stream_writer()
	: sink_()
	, size_(0)
	, cap_(0)
	, buffer_()
	{
	}
	
	~stream_writer()
	{
		flush();
	}
/*
	brigade* sink_brigade()
	{ return static_cast<DerivedT*>(this)->sink_brigade(); }

	void assign(shared_ptr<stream> sink_new)
	{
		size_ = 0;
		cap_ = 32;
		buffer_.reset(bucket_data_mem::create(cap_, size_));
		sink_ = sink_new;
	}
*/
	
	stream::write_status flush();
	
	stream::write_status partial_flush();
	
	void put(uint8_t b)
	{
		sassert(size_ <= cap_);
		if(size_ == cap_)
			overflow_put_(b);
		else
		{
			buffer_->data[size_] = b;
			++size_;
		}
	}
	
	void put(bucket* buf);
	
	shared_ptr<stream> detach()
	{
		flush_buffer();
		partial_flush();
		
		// Buffer any remaining buckets
		sink_->write_buffered(mem_buckets_);
		
		shared_ptr<stream> ret = sink_;
		sink_.reset();
		return ret;
	}
	
	void attach(shared_ptr<stream> new_sink)
	{
		if(sink_)
			throw stream_error("A sink is already attached to the stream_writer");
	}
	
private:
	void flush_buffer();
	
	void overflow_put_(uint8_t b)
	{
		if(size_ >= 1024)
		{
			flush();
			sassert(size_ < cap_);
			buffer_->data[size_] = b;
			++size_;
		}
		else
		{
			buffer_->size_ = size_; // Correct size
			cap_ *= 2;
			buffer_.reset(buffer_->enlarge(cap_));
			buffer_->unsafe_push_back(b);
			sassert(size_ + 1 == buffer_->size_);
			size_ = buffer_->size_;
		}
	}

	bucket_size size_;
	bucket_size cap_;
	list<bucket> mem_buckets_;
	std::auto_ptr<bucket_data_mem> buffer_;
	shared_ptr<stream> sink_;
};

inline stream_writer& operator<<(stream_writer& writer, char const* str)
{
	std::size_t len = std::strlen(str);
	writer.put(new bucket(str, len));
	return writer;
}

inline stream_writer& operator<<(stream_writer& writer, std::string const& str)
{
	writer.put(new bucket(str.data(), str.size()));
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
			throw stream_error("No source to pull in filter");
		
		read_status status = apply(true, false, amount);
		if(status != read_ok)
			return read_result(status);
		read_result res(read_ok, in_buffer.unlink_first());
		return res;
	}
	
	write_result write_bucket(bucket* b)
	{
		if(!sink)
			throw stream_error("No sink to push in filter");
		
		unlink(b);
		filter_buffer.append(b);
		apply(false);
		write_status rstatus = flush_filtered();
		if(rstatus != write_ok)
		{
			return write_result(rstatus, true);
		}
		
		return write_result(write_ok, true);
	}
	
	
	
	write_status propagate_flush()
	{
		sassert(out_buffer.empty());
		
		if(!sink)
			throw stream_error("No sink to push in filter");
		
		apply(false, true);
		write_status res = flush_filtered();
		if(res != write_ok)
			return res;
		if(!filter_buffer.empty())
			return write_would_block; // Still data that has not been filtered
		return write_ok;
	}
	
	pump_result pump()
	{
		if(!source)
			throw stream_error("No source to pull in filter");
		if(!sink)
			throw stream_error("No sink to push in filter");
			
		if(in_buffer.empty())
		{
			read_status rstatus = apply(true);
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

	write_status flush_filtered()
	{
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
	// If flush is false, the filter should make every effort to
	// filter all buckets in filter_buffer.
	virtual read_status apply(bool can_pull, bool flush = false, size_type amount = 0)
	{
		if(!out_buffer.empty())
		{
			in_buffer.buckets.splice(out_buffer.buckets);
			return read_ok;
		}
		else if(can_pull)
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
	
	//brigade buffer; // TODO: Use stream::in_buffer
};

template<typename Writer>
inline void write_uint16(Writer& writer, unsigned int v)
{
	writer.put(uint8_t((v >> 8) & 0xff));
	writer.put(uint8_t(v & 0xff));
}

template<typename Writer>
inline void write_uint32(Writer& writer, uint32_t v)
{
	writer.put(uint8_t((v >> 24) & 0xff));
	writer.put(uint8_t((v >> 16) & 0xff));
	writer.put(uint8_t((v >> 8) & 0xff));
	writer.put(uint8_t(v & 0xff));
}

template<typename Reader>
inline unsigned int read_uint16(Reader& reader)
{
	unsigned int ret = reader.get() << 8;
	ret |= reader.get();
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

}

#endif // UUID_957E3642BB06466DB21A21AFD72FAFAF
