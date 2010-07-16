#ifndef UUID_4800C387FBD44D146E8D33AE59C4A598
#define UUID_4800C387FBD44D146E8D33AE59C4A598

#include <iterator>
#include <stdexcept>
#include "../support/cstdint.hpp"
#include "../support/debug.hpp"

namespace gvl
{

// Functor -> infinite range adaptor
template<typename Functor>
struct functor_range
{
	typedef typename Functor::value_type value_type;
	
	functor_range(Functor func = Functor())
	: cached_(false), func_(func)
	{
	}
	
	Functor& functor();
	
	bool empty() const
	{
		return false;
	}
	
	value_type& front()
	{
		if(!cached_)
		{
			val = func_();
			cached_ = true;
		}
		return val;
	}
	
	void pop_front()
	{
		if(!cached_)
			func_(); // Skip
		else
			cached_ = false;
	}
	
private:
	Functor func_;
	bool cached_;
	value_type val;
};

struct delimited_iterator_range_overflow : std::exception
{
	delimited_iterator_range_overflow()
	{
	}
};

template<typename It, bool Except = false>
struct delimited_iterator_range
{
	typedef typename std::iterator_traits<It>::value_type value_type;
	typedef typename std::iterator_traits<It>::reference reference;
	
	delimited_iterator_range()
	: i(), e()
	{
	}

	delimited_iterator_range(It i_init, It e_init)
	: i(i_init), e(e_init)
	{
	}
	
	void put(value_type const& x)
	{
		if(i != e)
			*i++ = x;
		else if(Except)
			throw delimited_iterator_range_overflow();
	}
	
	void put(value_type const* x, uint32_t count)
	{
		while(count-- > 0)
		{
			put(*x++);
		}
	}

	bool empty() const
	{
		return i == e;
	}

	std::size_t size() const
	{
		return std::distance(i, e);
	}
	
	reference front()
	{
		sassert(i != e);
		return *i;
	}
	
	void pop_front()
	{
		sassert(i != e);
		++i;
	}
	
	It i, e;
};

template<typename It>
struct unsafe_delimited_iterator_range
{
	typedef typename std::iterator_traits<It>::value_type value_type;
	
	unsafe_delimited_iterator_range(It i_init, It e_init)
	: i(i_init), e(e_init)
	{
	}
	
	void put(value_type const& x)
	{
		*i++ = x;
	}
	
	void put(value_type const* x, uint32_t count)
	{
		while(count-- > 0)
		{
			put(*x++);
		}
	}
	
	bool empty() const
	{
		return i == e;
	}

	std::size_t size() const
	{
		return std::distance(i, e);
	}
	
	value_type& front()
	{
		sassert(i != e);
		return *i;
	}
	
	void pop_front()
	{
		sassert(i != e);
		++i;
	}
	
	It i, e;
};

template<typename Cont>
struct container_output_range
{
	typedef typename Cont::value_type value_type;
	typedef typename Cont::reference reference;
	
	void put(value_type const& x)
	{
		cont.push_back(x);
	}
	
	void put(value_type const* x, uint32_t count)
	{
		while(count-- > 0)
		{
			put(*x++);
		}
	}

	Cont cont;
};

template<typename ForwardIterator>
unsafe_delimited_iterator_range<ForwardIterator> iter_range(ForwardIterator const& b, ForwardIterator const& e)
{
	return unsafe_delimited_iterator_range<ForwardIterator>(b, e);
}

template<typename Container>
unsafe_delimited_iterator_range<typename Container::iterator> iter_range(Container& cont)
{
	return unsafe_delimited_iterator_range<typename Container::iterator>(cont.begin(), cont.end());
}

template<typename T, typename F>
void for_range(T range, F func)
{
	for(; !range.empty(); range.pop_front())
		func(range.front());
}

}

#endif // UUID_4800C387FBD44D146E8D33AE59C4A598
