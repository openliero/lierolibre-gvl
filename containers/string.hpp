#ifndef GVL_STRING_HPP
#define GVL_STRING_HPP

#include "bucket.hpp"
#include "../support/debug.hpp"
#include <cstring>
#include <string>

// Alternative implementation


namespace gvl
{

template<std::size_t InlineCapacity = 8>
struct basic_string_pod
{
	std::size_t size_;
	std::size_t cap_;
	union
	{
		bucket_data_mem* data;
		mutable uint8_t inline_data[InlineCapacity];
	} d;
};

template<std::size_t InlineSize = 7, bool Cow = false>
struct basic_string : basic_string_pod<InlineSize + 1> // Includes the 0-terminator
{
	typedef basic_string_pod<InlineSize + 1> base;

	struct take_data_tag {};

	template<std::size_t InlineSize2, bool Cow2>
	friend struct basic_string;

	// All ranges are invalidated when the string is changed except via
	// range functions, in which case all but the manipulating range are
	// invalidated. All ranges are also invalidated when the string is copied.
	struct range
	{
		template<std::size_t InlineSize2, bool Cow2>
		friend struct basic_string;

		uint8_t front() const
		{
			sassert(!empty());
			return *cur;
		}

		void pop_front()
		{
			sassert(!empty());
			++cur;
		}

		uint8_t back() const
		{
			sassert(!empty());
			return end[-1];
		}

		void pop_back()
		{
			sassert(!empty());
			--end;
		}

		bool empty() const
		{ return cur >= end; }

		void put(uint8_t c)
		{
			sassert(!empty());
			if(Cow)
			{
				uint8_t* prev_begin = self.begin_();
				if(self.cow_())
					cur = self.begin_() + (cur - prev_begin);
			}

			*cur++ = c;
		}
	private:
		range(basic_string& self, uint8_t* cur_init, uint8_t* end_init)
		: self(self), cur(cur_init), end(end_init)
		{
		}

		basic_string& self;
		uint8_t* cur;
		uint8_t* end;
	};

	basic_string()
	{
		reset_next_limit_();
	}

	basic_string(basic_string const& b)
	{
		base::operator=(b);
		if(!is_inline_())
		{
			if(Cow)
				this->d.data->add_ref();
			else
			{
				// bucket_data_mem allocates 1 byte extra for the zero-terminator
				this->d.data = this->d.data->clone(capacity(), size());
			}
		}
	}

	basic_string(char const* b_)
	{
		uint8_t const* b = reinterpret_cast<uint8_t const*>(b_);
		init_from_(b, std::strlen(b_));
	}

	basic_string(uint8_t const* b, std::size_t len)
	{
		init_from_(b, len);
	}

	basic_string(
		shared_ptr<bucket_data_mem> data_init,
		std::size_t size_init,
		std::size_t cap_init,
		take_data_tag)
	{
		// We can't take over the data pointer unless we're doing
		// Cow or the ref_count() is 1.
		if(!Cow && data_init->ref_count() > 1)
			init_from_(data_init->data, size_init); // Copy
		else
			init_from_(data_init.get(), size_init, cap_init, take_data_tag());
	}

	basic_string(
		shared_ptr<bucket_data_mem> data_init,
		take_data_tag)
	{
		// We can't take over the data pointer unless we're doing
		// Cow or the ref_count() is 1.
		if(!Cow && data_init->ref_count() > 1)
			init_from_(data_init->data, data_init->size()); // Copy
		else
			init_from_(data_init.get(), data_init->size(), data_init->size(), take_data_tag());
	}

	template<std::size_t InlineSize2, bool Cow2>
	basic_string(basic_string<InlineSize2, Cow2> const& b)
	{
		init_from_(b);
	}

	template<std::size_t InlineSize2, bool Cow2>
	basic_string(basic_string<InlineSize2, Cow2> const& b, take_data_tag)
	{
		if(b.is_inline_())
		{
			init_from_(b.d.inline_data, b.size_);
			b.reset_next_limit_();
		}
		else
		{
			init_from_(b.d.data, b.size_, b.cap_, take_data_tag());
			b.reset_next_limit_();
		}
	}

	~basic_string()
	{
		release_();
	}


	void clear()
	{
		if(Cow && !is_inline_() && this->d.data->ref_count() > 1)
		{
			// No point to retain the bucket_data_mem, release
			// and reset.
			this->d.data->release();
			reset_next_limit_();
		}
		else
		{
			this->size_ = 0;
		}
	}

	basic_string& operator=(basic_string b)
	{
		b.swap(*this);
		return *this;
	}

	template<std::size_t InlineSize2, bool Cow2>
	basic_string& operator=(basic_string<InlineSize2, Cow2> b)
	{
		b.swap(*this);
		return *this;
	}

	template<std::size_t InlineSize2, bool Cow2>
	void assign(basic_string<InlineSize2, Cow2> b)
	{
		b.swap(*this);
	}

	void assign(shared_ptr<bucket_data_mem> data_new, std::size_t size_new, std::size_t cap_new)
	{
		basic_string b(data_new, size_new, cap_new, take_data_tag());
		b.swap(*this);
	}

	void assign(shared_ptr<bucket_data_mem> data_new)
	{
		basic_string b(data_new, data_new->size(), data_new->size(), take_data_tag());
		b.swap(*this);
	}

	void assign(uint8_t const* begin, std::size_t len)
	{
		basic_string b(begin, len);
		b.swap(*this);
	}

	void push_back(uint8_t x)
	{
		cow_();

		if(this->size_ == this->cap_)
			reserve(size() * 2);

		begin_()[this->size_++] = x;
	}

	template<std::size_t InlineSize2, bool Cow2>
	void append(basic_string<InlineSize2, Cow2> const& b)
	{
		// cow_ called by append below
		append(b.begin_(), b.size_);
	}

	// NOTE: [b, e) cannot be inside *this
	void append(uint8_t const* b, std::size_t len)
	{
		sassert(!(b >= begin_() && b + len < end_()));

		cow_();
		if(capacity_left_() < len)
			reserve(size() + len);
		std::memcpy(next_(), b, len);
		this->size_ += len;
	}

	void append(char const* b)
	{
		append(reinterpret_cast<uint8_t const*>(b), std::strlen(b));
	}

	void reserve(std::size_t s)
	{
		if(InlineSize > 1 && s <= InlineSize)
			return;

		if(is_inline_())
		{
			std::size_t cur_size = this->size_;
			std::size_t cap_new = std::max(s, InlineSize * 2); // At least double the capacity
			// bucket_data_mem allocates 1 byte extra for the zero-terminator
			bucket_data_mem* data_init = bucket_data_mem::create(cap_new, cap_new);
			std::memcpy(data_init->data, this->d.inline_data, cur_size);

			this->d.data = data_init;
			this->size_ = cur_size;
			this->cap_ = cap_new;
		}
		else if(this->d.data->size_ < s)
		{
			std::size_t cur_size = this->size_;
			std::size_t cap_new = std::max(s, capacity() * 2); // At least double the capacity
			bucket_data_mem* old_data = this->d.data;
			// bucket_data_mem allocates 1 byte extra for the zero-terminator
			this->d.data = old_data->enlarge(cap_new);
			old_data->release();
			this->size_ = cur_size;
			this->cap_ = cap_new;
		}
	}

	basic_string<InlineSize, true> release_as_cow()
	{
		if(Cow)
		{
			basic_string<InlineSize, true> ret(*this);
			clear();
			return ret;
		}
		else
		{
			basic_string<InlineSize, true> ret(*this, take_data_tag());
			return ret;
		}
	}

	char const* c_str() const
	{
		// One additional byte is always allocated for the 0-terminator
		*this->end_() = 0;
		return reinterpret_cast<char const*>(begin_());
	}

	char const* data() const
	{
		return reinterpret_cast<char const*>(begin_());
	}

	std::size_t capacity() const
	{
		return this->cap_;
	}

	std::size_t size() const
	{
		return this->size_;
	}

	bool empty() const
	{ return this->size_ == 0; }

	range all()
	{
		return range(*this, begin_(), end_());
	}

	void swap(basic_string& rhs)
	{
		std::swap(static_cast<base&>(*this), static_cast<base&>(rhs));
	}

	uint8_t operator[](std::size_t i)
	{
		sassert(i < size());
		return begin_()[i];
	}

	void set(std::size_t i, uint8_t c)
	{
		sassert(i < size());
		cow_();
		begin_()[i] = c;
	}

private:

	std::size_t capacity_left_() const
	{
		return std::size_t(this->cap_ - this->size_);
	}

	void release_()
	{
		if(!is_inline_())
		{
			if(Cow)
				this->d.data->release();
			else
				delete this->d.data;
		}
	}

	void reset_next_limit_()
	{
		this->size_ = 0;
		if(InlineSize > 1)
		{
			this->cap_ = InlineSize;
			// We don't reset d.data, because it will not be
			// accessed when cap_ <= InlineSize
		}
		else
		{
			this->cap_ = 0;
			this->d.data = 0;
		}
	}

	uint8_t* begin_() const
	{
		return (is_inline_() ? this->d.inline_data : this->d.data->data);
	}

	uint8_t* end_() const
	{
		return begin_() + this->size_;
	}

	uint8_t* next_() const
	{
		return begin_() + this->size_;
	}

	bool is_inline_() const
	{
		return InlineSize > 1 && (this->cap_ <= InlineSize);
	}

	bool cow_()
	{
		if(Cow && !is_inline_() && this->d.data->ref_count() > 1)
		{
			// bucket_data_mem allocates 1 byte extra for the zero-terminator
			this->d.data = this->d.data->clone(capacity(), size());
			return true;
		}
		return false;
	}


	template<std::size_t InlineSize2, bool Cow2>
	void init_from_(basic_string<InlineSize2, Cow2> const& b)
	{
		if(b.is_inline_())
			init_from_(b.d.inline_data, b.size_);
		else if(Cow && Cow2)
			init_from_(b.d.data, b.size_, b.cap_);
		else
			init_from_(b.d.data->data, b.size_);
	}

	void init_from_(uint8_t const* b, std::size_t size_init)
	{
		this->size_ = size_init;
		if(InlineSize > 1 && size_init <= InlineSize)
		{
			this->cap_ = InlineSize;
			std::memcpy(this->d.inline_data, b, size_init);
		}
		else
		{
			// bucket_data_mem allocates 1 byte extra for the zero-terminator
			this->d.data = bucket_data_mem::create_from(b, b + size_init, size_init);
			this->cap_ = size_init;
		}
	}

	void init_from_(bucket_data_mem* data_init, std::size_t size_init, std::size_t cap_init)
	{
		sassert(Cow);

		this->size_ = size_init;
		if(InlineSize > 1 && size_init <= InlineSize)
		{
			this->cap_ = InlineSize;
			std::memcpy(this->d.inline_data, data_init->data, size_init); // TODO: memcpy isn't very fast for very small strings
		}
		else
		{
			this->d.data = data_init;
			this->d.data->add_ref();
			this->cap_ = cap_init;
		}
	}

	void init_from_(bucket_data_mem* data_init, std::size_t size_init, std::size_t cap_init, take_data_tag)
	{
		this->size_ = size_init;
		if(InlineSize > 1 && size_init <= InlineSize)
		{
			this->cap_ = InlineSize;
			std::memcpy(this->d.inline_data, data_init->data, size_init);
			data_init->release();
		}
		else
		{
			this->d.data = data_init;
			this->cap_ = cap_init;
		}
	}
};

typedef basic_string<> string;
typedef basic_string<7, true> string_cow;

}

#endif // GVL_STRING_HPP
