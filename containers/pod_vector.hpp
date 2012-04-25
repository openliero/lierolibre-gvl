#ifndef UUID_CA651223F7B545D1CC76BEB52DC2A9D3
#define UUID_CA651223F7B545D1CC76BEB52DC2A9D3

#include <cstddef>

namespace gvl
{

template<typename T>
struct pod_vector
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

	pod_vector()
	: data_(0), end_(0), limit_(0)
	{
	}

	explicit pod_vector(size_type s)
	: data_(0), end_(0), limit_(0)
	{
		data_ = new T[s];
		limit_ = end_ = data_ + s;
	}

	pod_vector(T const* p, size_type s)
	: data_(0), end_(0), limit_(0)
	{
		data_ = new T[s];
		limit_ = end_ = data_ + s;
		std::memcpy(data_, p, s * sizeof(T));
	}

	T*       data()       { return data_; }
	T const* data() const { return data_; }

	bool full() const
	{ return end_ == limit_; }

	/// Precondition: !full()
	void unsafe_push_back(T const& v)
	{
		passert(!full(), "Vector full");
		*end_ = v;
		++end_;
	}

	void push_back(T const& v)
	{
		if(full())
		{
			realloc_(1 + (size() * 3 / 2));
			unsafe_push_back(v);
			return;
		}
		// Repeated to possibly help compiler with aliasing information
		unsafe_push_back(v);
	}

	void reserve(size_type n)
	{
		if(capacity() < n)
			realloc_(n);
	}

	~pod_vector()
	{
		dealloc_();
	}

	void dealloc_()
	{
/*
		for(T* p = data_; p != end_; ++p)
		{
			src->~T();
		}
		*/
		delete[] data_;
	}

	void realloc_(size_type new_capacity)
	{
	/*
		vector<T> tmp;

		size_type new_size = std::min(new_capacity, size()); // Number of elements to copy

		tmp.data_ = reinterpret_cast<T*>(new char[new_capacity * sizeof(T)]);
		tmp.end_ = tmp.data;
		tmp.limit_ = tmp.data_ + new_capacity;

		for(pointer p = data_, e = data_ + new_size; p != e; ++p)
		{
			tmp.unsafe_push_back(*p);
		}

		swap(tmp);*/
		size_type new_size = std::min(new_capacity, size());

		T* new_data = new T[new_capacity];

		std::memcpy(new_data, data_, new_size * sizeof(T));
		delete[] data_;
		data_ = new_data;
		end_ = new_data + new_size;
		limit_ = new_data + new_capacity;
	}


	void swap(pod_vector& b)
	{
		std::swap(_data, b._data);
		std::swap(_end, b._end);
		std::swap(_limit, b._limit);
	}

	size_type size() const
	{ return end_ - data_; }

	size_type capacity() const
	{ return limit_ - data_; }

	bool valid(size_type n) const
	{ return n < size(); }

	iterator begin() { return data_; }
	iterator end() { return end_; }
	const_iterator begin() const { return data_; }
	const_iterator end() const { return end_; }

	reference operator[](size_type n)
	{
		passert(valid(n), "Out of bounds");
		return data_[n];
	}

private:
	T* data_;
	T* end_;
	T* limit_;
};

} // namespace gvl

#endif // UUID_CA651223F7B545D1CC76BEB52DC2A9D3
