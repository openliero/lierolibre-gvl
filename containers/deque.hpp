#ifndef UUID_49300C6029444349B888CE8010F07482
#define UUID_49300C6029444349B888CE8010F07482

#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <new>
#include "../support/debug.hpp"
//#include "memory.hpp"
#include "../support/functional.hpp"
//#include "common.hpp"

namespace gvl
{

template<typename T>
T* talloc(std::size_t count) { return static_cast<T*>(std::malloc(sizeof(T) * count)); }
template<typename T>
T* trealloc(T* p, std::size_t count) { return static_cast<T*>(std::realloc(p, sizeof(T) * count)); }

template<typename T, typename Deleter = dummy_delete>
struct deque : Deleter
{
	deque()
	: len(32), buf(talloc<T>(len))
	, begin(0), end(0), len_mask(len - 1)
	{
	}
	
	T extract(std::size_t i)
	{
		T& r = buf[(begin + i) & len_mask];
		T v(r);
		r = T();
		return v;
	}
	
	T& operator[](std::size_t i) { return buf[(begin + i) & len_mask]; }
	
	/*
	void set(std::size_t i, T const& c)
	{ buf[(begin + i) & len_mask] = c; }
	*/
	
	void push_back(T const& c)
	{
		cons_(buf + end, c);
		end = (end + 1) & len_mask;
		if(end == begin)
			expand();
	}
	
	void push_front(T const& c)
	{
		begin = (begin - 1) & len_mask;
		cons_(buf + begin, c);
		if(end == begin)
			expand();
	}
	
	/*
	simple_construct<T> push_back_inplace()
	{
		void* mem = buf + end;
		end = (end + 1) & len_mask;
		if(end == begin)
			expand();
		return mem;
	}*/
	
	void pop_front()
	{
		passert(begin != end, "Empty deque");
		dest_(buf + begin);
		begin = (begin + 1) & len_mask;
	}
	
	void pop_back()
	{
		passert(begin != end, "Empty deque");
		end = (end - 1) & len_mask;
		dest_(buf + end);
	}
	
	T const& extract_front()
	{
		passert(begin != end, "Empty deque");
		T* p = buf + begin;
		begin = (begin + 1) & len_mask;
		return *p;
	}
	
	T const& extract_back()
	{
		passert(begin != end, "Empty deque");
		end = (end - 1) & len_mask;
		T* p = buf + end;
		return *p;
	}

	void pop_front_n(size_t n)
	{
		//passert(size() <= n, "Not enough items to pop");
		for(std::size_t i = 0; i < n; ++i)
		{
			pop_front();
		}
	}
	
	void pop_back_n(size_t n)
	{
		//passert(size() <= n, "Not enough items to pop");
		for(std::size_t i = 0; i < n; ++i)
		{
			pop_back();
		}
	}

	void clear()
	{
		pop_front_n(size());
		//begin = end = 0;
	}

	bool empty() { return begin == end; }
	
	T& back()
	{
		passert(!empty(), "Empty deque");
		return buf[(end - 1) & len_mask];
	}
	
	T& front()
	{
		passert(!empty(), "Empty deque");
		return buf[(begin) & len_mask];
	}

	std::size_t size()
	{
		return (end - begin) & len_mask;
	}
	
	void swap(deque& b)
	{
		std::swap(len, b.len);
		std::swap(buf, b.buf);
		std::swap(begin, b.begin);
		std::swap(end, b.end);
		std::swap(len_mask, b.len_mask);
	}
	
private:
	std::size_t len;
	T* buf;
	std::size_t begin;
	std::size_t end;
	std::size_t len_mask;

	void expand()
	{
		passert(begin == end, "expand can only be called when begin == end");
		
		std::size_t count = len;
		std::size_t new_len = len * 2;
		T* new_buf = talloc<T>(new_len);
		
		size_t part1_len = (count - begin);
		size_t part2_len = end;
		std::memcpy(new_buf, buf + begin, part1_len * sizeof(T));
		std::memcpy(new_buf + part1_len, buf, part2_len * sizeof(T));
		
		begin = 0;
		end = count;
		len = new_len;
		len_mask = len - 1;
		std::free(buf);
		buf = new_buf;
	}
	
	void cons_(T* p, T const& v = T())
	{
		new (p) T(v);
	}
	
	void dest_(T* p)
	{
		Deleter::operator()(*p);
		p->~T();
	}

	// Disallow copying
	deque(deque const&);
	void operator=(deque const&);
};

template<typename T, typename Deleter = default_delete>
struct pdeque : deque<T*, Deleter>
{
	// set, get, push_back, pop_front, pop_front_n, reset are fine
	// back, front, empty, size are fine
};

} // namespace gvl

#endif // UUID_49300C6029444349B888CE8010F07482
