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

#ifndef UUID_C9885A5B4D7F4D849D6DC18BE63A0131
#define UUID_C9885A5B4D7F4D849D6DC18BE63A0131

#include "../resman/linked_object.hpp"
#include "../support/move.hpp"

namespace gvl
{

/// NOTE: Only really usable with PODs
template<typename T>
struct linked_vector : linked_object<linked_vector<T> >
{
	typedef std::size_t size_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;
	typedef pointer iterator;
	typedef const_pointer const_iterator;

	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	typedef linked_object<linked_vector<T> > base;

	struct take_ownership_tag {};
	struct borrow_tag {};

	linked_vector()
	: data_(0), begin_(0), end_(0), limit_(0)
	{
	}

	explicit linked_vector(size_type s)
	: data_(0), begin_(0), end_(0), limit_(0)
	{
		alloc_(s);
	}

	linked_vector(T const* p, size_type s)
	: data_(0), begin_(0), end_(0), limit_(0)
	{
		begin_ = data_ = new T[s];
		limit_ = end_ = data_ + s;
		std::memcpy(data_, p, s * sizeof(T));
	}

	linked_vector(T* p, size_type s, take_ownership_tag)
	: data_(p), begin_(p), end_(p + s), limit_(p + s)
	{
	}

	linked_vector(T* p, size_type s, borrow_tag)
	: data_(0), begin_(p), end_(p + s), limit_(p + s)
	{
	}

	linked_vector(move_holder<linked_vector> m)
	: base(move<base>(m))
	, data_(m->data_), begin_(m->begin_), end_(m->end_), limit_(m->limit_)
	{
		m->data_ = m->begin_ = m->end_ = m->limit_ = 0;
	}

	// Default copy-ctor is fine, op= is not
	linked_vector& operator=(linked_vector const& b)
	{
		delete_();
		linked_object<linked_vector<T> >::operator=(b);
		data_ = b.data_;
		begin_ = b.begin_;
		end_ = b.end_;
		limit_ = b.limit_;
		return *this;
	}

	~linked_vector()
	{
		if(this->unique())
			delete[] this->data_;
	}

/* TODO: COW
	T*       data()       { return begin_; }*/
	T const* data() const { return begin_; }

	/// Returned pointer is only valid until any other non-const member function
	/// is called. NOTE: Returned pointer can also be invalidated if this
	/// vector refers to some transient storage that is destroyed.
	T* mut_data()
	{
		if(!this->unique())
			realloc_(size());
		return begin_;
	}

	bool full() const
	{ return end_ == limit_; }

	bool empty() const
	{ return begin_ == end_; }

	/// Precondition: !full() && this->unique()
	/// WARNING: Use with extreme care.
	void unsafe_unique_push_back(T const& v)
	{
		passert(!full(), "Vector full");
		passert(this->unique(), "Vector is not unique");
		*end_ = v;
		++end_;
	}

	/// Precondition: this->unique()
	/// WARNING: Use with extreme care.
	void unique_push_back(T const& v)
	{
		passert(this->unique(), "Vector is not unique");

		if (full())
			realloc_((size() + 1) * 3 / 2);
		*end_ = v;
		++end_;
	}

	void push_back(T const& v)
	{
		*space1_() = v;
	/*
		if(full())
		{
			realloc_(1 + (size() * 3 / 2));
			unsafe_push_back(v);
			return;
		}
		// Repeated to possibly help compiler with aliasing information
		unsafe_push_back(v);*/
	}

	/// Resizes the vector to n elements.
	/// The contents is undefined.
	/// The pointer returned is the same as
	/// would be returned from mut_data().
	T* assign(size_type n)
	{
		if(capacity() < n)
		{
			delete_();
			alloc_(n);
		}
		else
		{
			end_ = begin_ + n;
		}

		return begin_;
	}

	void reserve(size_type n)
	{
		if(capacity() < n)
			realloc_(n);
	}

#if 0 // Incorrect
	void swap(linked_vector& b)
	{
		std::swap(_data, b._data);
		std::swap(_begin, b._begin);
		std::swap(_end, b._end);
		std::swap(_limit, b._limit);
	}
#endif

	size_type size() const
	{ return end_ - begin_; }

	size_type capacity() const
	{ return limit_ - begin_; }

	bool valid(size_type n) const
	{ return n < size(); }

	void cut_front(size_type n)
	{
		sassert(n <= size());
		begin_ += n;
	}

/* // TODO: COW
	iterator begin() { return data_; }
	iterator end() { return end_; }*/

	const_iterator begin() const { return begin_; }
	const_iterator end() const { return end_; }

// TODO: COW for mutable version
	const_reference operator[](size_type n) const
	{
		passert(valid(n), "Out of bounds");
		return data_[n];
	}

	/// NOTE: Invalidates iterators to this object
	/// Precondition: not heap allocated
	void heap_allocate()
	{
		passert(!data_, "Already heap allocated");
		realloc_(size());
	}

protected:

	T* space_(size_type amount)
	{
		if(!this->unique()
		|| size() + amount >= capacity())
		{
			// Make unique
			realloc_((size() + amount) * 3 / 2);
		}

		T* ret = end_;
		end_ += amount;
		return ret;
	}

	T* space1_()
	{
		if(!this->unique()
		|| end_ == limit_)
		{
			// Make unique
			realloc_((size() + 1) * 3 / 2);
		}

		T* ret = end_;
		++end_;
		return ret;
	}

	void alloc_(size_type s)
	{
		begin_ = data_ = new T[s];
		limit_ = end_ = data_ + s;
	}

	void realloc_(size_type new_capacity)
	{
		size_type new_size = std::min(new_capacity, size());

		T* new_data = new T[new_capacity];

		std::memcpy(new_data, begin_, new_size * sizeof(T));
		delete_();
		begin_ = data_ = new_data;
		end_ = new_data + new_size;
		limit_ = new_data + new_capacity;
	}

	void delete_()
	{
		if(this->unique())
			delete[] this->data_;
		else
			this->make_unique();
	}

	T* data_; // If non-zero, data_ is allocated with new[]
	T* begin_;
	T* end_;
	T* limit_;
};

/*
template<typename T>
struct linked_vector_heap : linked_vector<T>
{
	linked_vector()
	: data_(0), begin_(0), end_(0), limit_(0)
	{
	}

	explicit linked_vector(size_type s)
	: data_(0), begin_(0), end_(0), limit_(0)
	{
		begin_ = data_ = new T[s];
		limit_ = end_ = data_ + s;
	}

	linked_vector(T const* p, size_type s)
	: data_(0), begin_(0), end_(0), limit_(0)
	{
		begin_ = data_ = new T[s];
		limit_ = end_ = data_ + s;
		std::memcpy(data_, p, s * sizeof(T));
	}

	linked_vector(T* p, size_type s, ownership_tag)
	: data_(p), begin_(p), end_(p + s), limit_(p + s)
	{
	}

	~linked_vector_heap()
	{
		if(this->unique())
			delete[] this->data_;
	}
};
*/

template<typename T, unsigned int N>
struct linked_vector_temp : linked_vector<T>
{
	typedef linked_vector<T> base;

	linked_vector_temp()
	: base(temp_data_, N, typename base::borrow_tag())
	{
	}

	~linked_vector_temp()
	{
		if(!this->unique())
		{
			// Go through all
			// NOTE: This will invalidate iterators for all
			// the containers.
			typedef typename base::linked_iterator iter;
			iter i = this->linked_begin(), e = this->linked_end();
			for(; i != e; )
			{
				// heap_allocate will invalidate the iterator
				iter next = i; ++next;
				i->heap_allocate();
				i = next;
			}
		}
	}

/* Not safe, use mut_data()
	// Only to be used before this is copied
	T* data() { return temp_data_; }
	*/
private:
	// Can't copy to other linked_vector_temp as there can
	// only be one owner.
	linked_vector_temp(linked_vector_temp const&);
	linked_vector_temp& operator=(linked_vector_temp const&);

	T temp_data_[N];
};

} // namespace gvl

#endif // UUID_C9885A5B4D7F4D849D6DC18BE63A0131
