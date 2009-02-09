#ifndef UUID_E21BF45A612A4B4DCF6585A0D086DF7C
#define UUID_E21BF45A612A4B4DCF6585A0D086DF7C

#include "../containers/list.hpp"
#include "../containers/pod_vector.hpp"
#include "../containers/linked_vector.hpp"
#include "../support/debug.hpp"
#include "../support/cstdint.hpp"
#include "../resman/shared_ptr.hpp"
#include <memory>

namespace gvl
{

struct bucket_mem;
struct bucket;
struct brigade;

typedef std::size_t bucket_size;

struct bucket_source
{
	typedef bucket_size size_type;
	
	enum status
	{
		ok,
		blocking,
		eos,
		error
	};
	
	struct read_result
	{
		explicit read_result(status s)
		: s(s)
		, b(0)
		{
		}
		
		read_result(status s, bucket* b)
		: s(s)
		, b(b)
		{
		}
		
		status s;
		bucket* b;
	};
	
	// IMPORTANT: TODO: std::auto_ptr isn't necessarily safe
	// to have in a struct that is meant to be copied. How
	// do we fix this? Temporarily, we have a copy ctor that
	// does const_cast.
	struct auto_read_result
	{
		auto_read_result(auto_read_result const& other)
		: s(other.s)
		, b(const_cast<std::auto_ptr<bucket>&>(other.b))
		{
		}
		
		explicit auto_read_result(status s)
		: s(s)
		{
		}
		
		auto_read_result(status status, bucket* b)
		: s(s)
		, b(b)
		{
		}
		
		/// IMPORTANT: Don't use the read_result passed
		/// after constructing this.
		auto_read_result(read_result const& other)
		: s(other.s)
		, b(other.b)
		{
		}
		
		status s;
		std::auto_ptr<bucket> b;
	};
	
	virtual read_result read(size_type amount = 0, bucket* dest = 0) = 0;
	
	virtual ~bucket_source()
	{ }
};

struct bucket_sink
{
	typedef bucket_size size_type;
	
	enum status
	{
		ok,
		part,
		would_block,
		error
	};
	
	/// Writes a bucket to a sink. If bucket_sink::ok is returned,
	/// takes ownership of 'b'.
	/// NOTE: 'b' must be inserted into a list<bucket>, the function
	/// can unlink it.
	virtual status write(bucket* b) = 0;
	
	virtual ~bucket_sink()
	{ }
};

/// Abstract bucket base
struct bucket : list_node<>
{
	bucket()
	: size_(0)
	, size_known_(false)
	{
	}
	
	bucket(size_type size)
	: size_(size)
	, size_known_(true)
	{
		
	}
		
	bool size_known() const { return size_known_; }
	//std::size_t begin() const { return begin_; }
	size_type size() const
	{
		passert(size_known(), "Size is unknown");
		return size_;
	}
	
#if 0 // TODO: Fix this
	/// Split the bucket at 'point' relative to the beginning.
	/// Will not split unless necessary.
	/// Precondition: size_known() && 0 <= point <= size()
	void split(std::size_t point)
	{
		passert(size_known(), "Size is unknown");
		passert(0 <= point && point <= size(), "Split point is out of bounds");
		
		if(point == 0 || point == size())
			return; // No need to do anything
				
		// Insert before
		// TODO: We should actually let the bucket split itself.
		// It should at least know when it's being copied.
		relink(new bucket(*this, begin_, begin_ + point));
		begin_ += point;
	}
#endif
	
	/// Returns a pointer to memory for the data of the bucket.
	/// This function may convert the bucket
	virtual uint8_t const* get_ptr()
	{
		// Default definition throws an error
		passert(size_known_, "Size is unknown");
		passert(!size_known_, "Buckets with known size must define get_ptr()");
		throw "TODO: get_ptr() not implemented";
	}
	
	virtual void get_vector(linked_vector<uint8_t>& dest)
	{
		passert(!size_known_, "Buckets with known size must define get_vector()");
		throw "TODO: get_ptr() not implemented";
	}
	
	virtual void cut_front(size_type amount)
	{
		passert(size_known_, "Size is unknown");
		passert(!size_known_, "Buckets with known size must define cut_front()");
		throw "TODO: get_ptr() not implemented";
	}
	
	virtual bucket* clone()
	{
		throw "Bucket is not clonable";
	}
	
	virtual ~bucket()
	{
	}
	
	
protected:

/*
	std::size_t begin_;
	std::size_t end_;*/
	size_type size_;
	bool size_known_;
};

struct bucket_mem : bucket
{
	bucket_mem(bucket_mem const& other)
	: bucket(other)
	, vec(other.vec)
	{
	}
	
	bucket_mem(linked_vector<uint8_t> const& other)
	: bucket(other.size())
	, vec(other)
	{
	}
	
	bucket_mem(move_holder<linked_vector<uint8_t> > other)
	: bucket(other->size())
	, vec(other)
	{
	}
	
	bucket_mem(std::size_t s)
	: bucket(s)
	, vec(s)
	{
	}
	
	bucket_mem(char const* p, std::size_t s)
	: bucket(s)
	, vec(reinterpret_cast<uint8_t const*>(p), s)
	{
	}
	
	uint8_t const* get_ptr();
	void get_vector(linked_vector<uint8_t>& dest);
	read_result read(size_type amount = 0, bucket* dest = 0);
	void cut_front(size_type amount);
	bucket* clone();
	
	//pod_vector<uint8_t> vec;
	linked_vector<uint8_t> vec;
};

void sequence(list<bucket>& l, std::size_t amount, linked_vector<uint8_t>& res);

// Provides functions for extacting data
// from a brigade in a convenient and
// efficient manner.
// NOTE: You are not allowed to modify buckets
// that are buffered.
template<typename DerivedT>
struct bucket_reader
{
	typedef bucket::size_type size_type;
	
	bucket_reader(bucket_source* source)
	: bucket_list_size_(0)
	, cur_(0)
	, end_(0)
	, source_(source)
	{
	}
	
	DerivedT* derived()
	{ return static_cast<DerivedT*>(this); }
	
/*
	// Different naming to avoid infinite recursion if
	// not defined in DerivedT.
	bucket_source* get_source()
	{ return derived()->source(); }
	*/
	
	bucket_source* get_source()
	{ return source_; }
	
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
	
	// TODO: A get that returns a special value for EOF
		
	bucket::status buffer(size_type amount)
	{
		size_type cur_read = read_size();
		while(cur_read < amount)
		{
			bucket::status s = read_bucket_(amount - cur_read);
			if(s != bucket::ok)
				return s;
			cur_read = read_size();
		}
		
		return bucket::ok;
	}
	
	
	bucket::status buffer_sequenced(size_type amount, linked_vector<uint8_t>& res)
	{
		bucket::status s = buffer(amount);
		if(s != bucket::ok)
			return s;
		reinsert_first_bucket();
		
		sequence(mem_buckets_, amount, res);
		return bucket::ok;
	}
	
	bucket::auto_read_result get_bucket(size_type amount = 0)
	{
		if(first_.get())
		{
			correct_first_bucket_();
			return bucket::read_result(bucket::ok, first_.release());
		}
		else if(!mem_buckets_.empty())
		{
			return bucket::read_result(bucket::ok, pop_bucket_());
		}
		else
			return read_bucket_and_return_(amount);
	}
	
	// Non-blocking
	bucket::auto_read_result try_get_bucket(size_type amount = 0)
	{
		if(first_.get())
		{
			correct_first_bucket_();
			return bucket::read_result(bucket::ok, first_.release());
		}
		else if(!mem_buckets_.empty())
		{
			return bucket::read_result(bucket::ok, pop_bucket_());
		}
		else
			return try_read_bucket_and_return_(amount);
	}
	
	/// Amount of data left in the first bucket
	std::size_t first_left() const { return end_ - cur_; }
	
private:
	
	uint8_t underflow_get_()
	{
		if(next_bucket_() != bucket::ok)
			throw "ffs! unexpected error in underflow_get_";
		
		return *cur_++;
	}
	
	/// Discards the current first bucket (if any) and tries to read
	/// a bucket if necessary.
	/// Precondition: cur_ == end_
	bucket::status next_bucket_()
	{
		passert(cur_ == end_, "Still data in the first bucket");
		
		if(!mem_buckets_.empty())
		{
			//first_.reset(pop_bucket_());
			set_first_bucket_(pop_bucket_());
			return bucket::ok;
		}
		
		// Need to read a bucket
		
		// Reset first
		// No need to do this: cur_ = end_ = 0;
		first_.reset();
		
		while(true)
		{
			bucket::read_result r(get_source()->read());

			if(r.s == bucket::ok)
			{
				// Callers of next_bucket_ expect the result
				// in first_
				set_first_bucket_(r.b);
				return bucket::ok;
			}
			else if(r.s == bucket::eos)
			{
				return bucket::eos;
			}
			
			derived()->block();
		}
	}
	
	bucket::status read_bucket_(size_type amount)
	{
		while(true)
		{
			bucket::read_result r(get_source()->read(amount));
		
			if(r.s == bucket::ok)
			{
				add_bucket_(r.b);
				return bucket::ok;
			}
			else if(r.s == bucket::eos)
			{
				return bucket::eos;
			}
			
			derived()->flush();
			derived()->block();
		}
	}
	
	bucket::status try_read_bucket_(size_type amount)
	{
		bucket::read_result r(get_source()->read(amount));
	
		if(r.s == bucket::ok)
		{
			add_bucket_(r.b);
			return bucket::ok;
		}
		
		return r.s;
	}
	
	bucket::read_result read_bucket_and_return_(size_type amount)
	{
		while(true)
		{
			bucket::read_result r(get_source()->read(amount));
		
			if(r.s != bucket::blocking)
				return r;
			
			derived()->flush();
			derived()->block();
		}
	}
	
	bucket::read_result try_read_bucket_and_return_(size_type amount)
	{
		return get_source()->read(amount);
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
	bucket_source* source_;
};

struct brigade;

struct bucket_writer
{
	bucket_writer(brigade* sink)
	: sink_(sink)
	{
	}
/*
	brigade* sink_brigade()
	{ return static_cast<DerivedT*>(this)->sink_brigade(); }
*/
	void flush();
	
	void put(uint8_t b)
	{
		if(buffer_.full())
			overflow_put_(b);
		else
			buffer_.unsafe_unique_push_back(b);
	}
	
	void put(bucket* buf);
	
private:
	void overflow_put_(uint8_t b)
	{
		if(buffer_.size() >= 1024)
			flush();
		buffer_.push_back(b);
	}

	linked_vector<uint8_t> buffer_;
	brigade* sink_;
};

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

struct brigade : bucket_source
{
	typedef bucket_size size_type;
	
	read_result read(size_type amount = 0, bucket* dest = 0)
	{
		list<bucket>::iterator i = buckets.begin();
		
		if(i == buckets.end())
			return read_result(eos);
			
		read_result r(i->read(amount, 0));
		if(r.s == bucket_source::ok)
			buckets.unlink(r.b); // Success, we may unlink the bucket
			
		return r;
	}

	void prepend(bucket* b)
	{
		buckets.relink_front(b);
	}
	
	void append(bucket* b)
	{
		buckets.relink_back(b);
	}
	
	list<bucket> buckets;
};

}


#endif // UUID_E21BF45A612A4B4DCF6585A0D086DF7C
