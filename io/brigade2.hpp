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

#ifndef UUID_0C9C2A832513404AD6879FA554E66DA6
#define UUID_0C9C2A832513404AD6879FA554E66DA6

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
	/// takes ownership of 'b' and unlinks it from it's list<bucket>.
	/// NOTE: 'b' must be inserted into a list<bucket>
	virtual status write(bucket* b) = 0;

	virtual ~bucket_sink()
	{ }
};

struct bucket_data : bucket_source
{
	bucket_data()
	: ref_count_(1)
	{
	}

	void add_ref()
	{
		++ref_count_;
	}

	void release()
	{
		if (--ref_count_ <= 0)
			delete this;
	}



	int ref_count_;
};

/// Abstract bucket base
struct bucket : bucket_source, list_node<bucket>
{
	bucket()
	: begin_(0)
	, end_(-1)
	{
	}

	bucket(bucket const& b, long begin, long end)
	: begin_(begin)
	, end_(end)
	{
	}

	bucket(size_type size)
	: size_(size)
	, size_known_(true)
	{

	}

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
		relink_after(new bucket(*this, begin_ + point, end_));
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

	void cut_front(size_type amount)
	{
		passert(size_known(), "Size is unknown");
		begin_ += amount;
	}

	void cut_back(size_type amount)
	{
		passert(size_known(), "Size is unknown");
		end_ += amount;
		passert(begin_ <= end_, "");
	}

	bucket* clone() const
	{
		return new bucket(*this, begin_, end_);
	}

	~bucket()
	{
	}


protected:

	std::auto_ptr<bucket_data> data_;
	long begin_;
	long end_;
};

struct bucket_data_mem : bucket_data
{
	bucket_data_mem(bucket_mem const& other)
	: bucket(other)
	, vec(other.vec)
	{
	}

	bucket_data_mem(linked_vector<uint8_t> const& other)
	: bucket(other.size())
	, vec(other)
	{
	}

	bucket_data_mem(move_holder<linked_vector<uint8_t> > other)
	: bucket(other->size())
	, vec(other)
	{
	}

	bucket_data_mem(std::size_t s)
	: bucket(s)
	, vec(s)
	{
	}

	bucket_data_mem(char const* p, std::size_t s)
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

// Provides functions for extracting data
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


#endif // UUID_0C9C2A832513404AD6879FA554E66DA6
