#ifndef UUID_49300C6029444349B888CE8010F07482
#define UUID_49300C6029444349B888CE8010F07482

#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <new>
#include "../support/debug.hpp"
#include "../support/functional.hpp"
#include "../support/platform.hpp"

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

	~deque()
	{
		clear();
	}

#if GVL_CPP0X
	deque(deque&& other)
	: Deleter(std::forward<Deleter>(other))
	, len(other.len)
	, buf(other.buf)
	, begin(other.begin)
	, end(other.end)
	, len_mask(other.len_mask)
	{
		// Minimal necessary to make destruction safe
		other.buf = 0;
		other.begin = 0;
		other.end = 0;
	}

	deque& operator=(deque&& other)
	{
		clear();
		len = other.len;
		buf = other.buf;
		begin = other.begin;
		end = other.end;
		len_mask = other.len_mask;

		// Minimal necessary to make destruction safe
		other.buf = 0;
		other.begin = 0;
		other.end = 0;
		return *this;
	}

#endif
	
#if 0 // TODO
	T extract(std::size_t i)
	{
		T& r = buf[(begin + i) & len_mask];
		T v(r);
		r = T();
		return v;
	}
#endif
	
	T& operator[](std::size_t i) { return buf[(begin + i) & len_mask]; }
	
	/*
	void set(std::size_t i, T const& c)
	{ buf[(begin + i) & len_mask] = c; }
	*/

#if GVL_CPP0X
	void push_back(T&& c)
	{
		cons_(buf + end, std::move(c));

		end = (end + 1) & len_mask;
		if(end == begin)
			expand();
	}

	void push_front(T&& c)
	{
		// Nothing is updated before the construction is done
		// for exception safety.
		std::size_t newbegin = (begin - 1) & len_mask;
		cons_(buf + newbegin, std::move(c));

		begin = newbegin;
		if(end == begin)
			expand();
	}
#endif
	
	void push_back(T const& c)
	{
		// Nothing is updated before the construction is done
		// for exception safety.
		cons_(buf + end, c);

		end = (end + 1) & len_mask;
		if(end == begin)
			expand();
	}
	
	void push_front(T const& c)
	{
		// Nothing is updated before the construction is done
		// for exception safety.
		std::size_t newbegin = (begin - 1) & len_mask;
		cons_(buf + newbegin, c);

		begin = newbegin;
		if(end == begin)
			expand();
	}
	
	void pop_front()
	{
		passert(begin != end, "Empty deque");
		std::size_t oldbegin = begin;
		begin = (begin + 1) & len_mask;
		dest_(buf + oldbegin);
	}
	
	void pop_back()
	{
		passert(begin != end, "Empty deque");
		end = (end - 1) & len_mask;
		dest_(buf + end);
	}
	
#if 0 // TODO
	T extract_front()
	{
		passert(begin != end, "Empty deque");
		T* p = buf + begin;
		begin = (begin + 1) & len_mask;
		return GVL_MOVE(*p);
	}
	
	T extract_back()
	{
		passert(begin != end, "Empty deque");
		end = (end - 1) & len_mask;
		T* p = buf + end;
		return GVL_MOVE(*p);
	}
#endif

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

	bool empty() const { return begin == end; }
	
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

	std::size_t size() const
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
		// TODO: This isn't exception safe at all

		passert(begin == end, "expand can only be called when begin == end");
		
		std::size_t count = len;
		std::size_t new_len = len * 2;
		T* new_buf = talloc<T>(new_len);

		size_t part1_len = (count - begin);
		size_t part2_len = end;

		T* p;
		T* e;
		T* d;

		for(p = buf + begin, e = p + part1_len, d = new_buf; p != e; ++p, ++d)
			cons_(d, GVL_MOVE(*p));

		for(p = buf, e = p + part2_len, d = new_buf + part1_len; p != e; ++p, ++d)
			cons_(d, GVL_MOVE(*p));

		dest_(buf, buf + count);

		begin = 0;
		end = count;
		len = new_len;
		len_mask = len - 1;
		std::free(buf);
		buf = new_buf;
	}

#if GVL_CPP0X
	void cons_(T* p, T&& v)
	{
		new (p) T(std::move(v));
	}
#endif
	
	void cons_(T* p, T const& v = T())
	{
		new (p) T(v);
	}
	
	void dest_(T* p)
	{
		Deleter::operator()(*p);
		p->~T();
	}

	void dest_(T* b, T* e)
	{
		for(; b != e; ++b)
			dest_(b);
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
