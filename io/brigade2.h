#ifndef UUID_0C9C2A832513404AD6879FA554E66DA6
#define UUID_0C9C2A832513404AD6879FA554E66DA6

#include <stddef.h>
#include "../containers/list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t gvl_bucket_size;
typedef unsigned char byte;
typedef void (*gvl_bucket_destroy)(struct gvl_bucket_data_*);

typedef struct gvl_bucket_ gvl_bucket;
typedef struct gvl_bucket_data_ gvl_bucket_data;

/*
typedef struct gvl_object_type_
{
	
} gvl_object_type;*/

typedef struct gvl_bucket_source_type_
{
	gvl_bucket*      (*read)   (void*, size_t hint_amount);
} gvl_bucket_source_type;

typedef struct gvl_bucket_type_
{
	gvl_bucket_source_type base;
	void (*destroy)(gvl_bucket_data*);
	
} gvl_bucket_type;

/* Bucket data base */
struct gvl_bucket_data_
{
	int ref_count;
};

struct gvl_object_
{
	void* type;
} gvl_object;

struct gvl_bucket_source_
{
	gvl_object base;
} gvl_bucket_source;

struct gvl_bucket_
{
	gvl_bucket_source base;
	
	gvl_list_node list_node;
	
	gvl_bucket_data* data;
	long begin;
	long end;
};

#define DOWNCAST(to, member_in_to, ptr) ((to*)((char*)(ptr) - offsetof(to, member_in_to)))
#define UPCAST(member_in_ptr, ptr) (&(ptr)->member_in_ptr)

#define GVL_LIST_NODE_FROM_BUCKET(self) UPCAST(list_node, self)
#define GVL_BUCKET_FROM_LIST_NODE(self) DOWNCAST(gvl_bucket, list_node, self)

INLINE void gvl_bucket_init(
	gvl_bucket* self,
	gvl_bucket_type* type,
	gvl_bucket_data* data,
	long begin,
	long end)
{
	self->type = type;
	self->data = data;
	self->begin = begin;
	self->end = end;
}

INLINE void gvl_bucket_init_copy(
	gvl_bucket* self,
	gvl_bucket* from,
	long begin,
	long end)
{
	gvl_bucket_data* data = from->data;
	self->type = from->type;
	self->data = data;
	++data->ref_count;
	
	self->begin = begin;
	self->end = end;
}

INLINE void gvl_bucket_new_copy(gvl_bucket_data* from, long begin, long end)
{
	gvl_bucket* self = malloc(sizeof(gvl_bucket));
	gvl_bucket_init_copy(self, from, begin, end);
}

INLINE void gvl_bucket_destroy(gvl_bucket* self)
{
	gvl_bucket_data* data = self->data;
	if (--self->ref_count == 0)
		self->type->destroy(data);
}

#define BLOCK(x) do x; while(0)

#define gvl_bucket_destroy(self) BLOCK({ gvl_bucket_data* self_ = (self); self_->type->destroy(self_); })

INLINE size_t gvl_bucket_size(gvl_bucket* self)
{
	return self->end - self->begin;
}

struct gvl_bucket_data_mem_
{
	gvl_bucket_data base;
	byte* end;
	byte* limit;
	byte  data[1]; // Actually arbitrary size
	
} gvl_bucket_data_mem;

gvl_bucket_data_mem* gvl_bucket_data_mem_new(size_t capacity)
{
	gvl_bucket_data_mem* self = malloc(sizeof(gvl_bucket_data_mem) - sizeof(byte) + capacity);
	
	gvl_bucket_data_init(&self->base);
	self->limit = &self->data[capacity];
	self->end = &self->data[0];
}

#define gvl_bucket_data_mem_ptr(self) ((byte const*)((self)->data))

INLINE gvl_bucket_data_mem_push_back_unsafe_unique(gvl_bucket_data_mem* self, byte b)
{
	*self->end++ = b;
}

#define gvl_bucket_cut_front(self, amount) BLOCK((self)->begin += (amount))
#define gvl_bucket_cut_back(self, amount) BLOCK((self)->end -= (amount))

void gvl_bucket_split(gvl_bucket* self, size_t point)
{
	size_t self_size = gvl_bucket_size(self);
	
	if (point == 0 || point == self_size)
		return;
		
	gvl_list_link_after(&self->list_node,
		gvl_bucket_new_copy(self, self->begin + point, self->end));
		
	gvl_bucket_cut_back(self, self_size - point);
}

typedef struct gvl_bucket_reader_
{
	char* begin;
	char* end;
	gvl_list buffer;
	gvl_bucket_source source;
	char begin_initialized;
	void (*on_error)();
} gvl_bucket_reader;


INLINE byte gvl_bucket_reader_underflow_get_(gvl_bucket_reader* self)
{
	if(gvl_bucket_reader_next_bucket_(self) != gvl_bucket_ok)
	{
		self->on_error();
		return 0;
	}
	
	return *self->cur++;
}

INLINE byte gvl_bucket_reader_get(gvl_bucket_reader* self)
{
	return (self->cur != self->end ? *self->cur++ : gvl_bucket_reader_underflow_get_(self));
}

gvl_bucket_status gvl_bucket_reader_next_bucket_(gvl_bucket_reader* self)
{
	if (begin_initialized)
	{
		gvl_bucket_destroy(
	}
}


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

#ifdef __cplusplus
}
#endif


#endif // UUID_0C9C2A832513404AD6879FA554E66DA6
