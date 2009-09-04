#ifndef UUID_4800C387FBD44D146E8D33AE59C4A598
#define UUID_4800C387FBD44D146E8D33AE59C4A598

#include <iterator>

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

template<typename It>
struct delimited_iterator_range
{
	typedef typename std::iterator_traits<It>::value_type value_type;
	
	delimited_iterator_range(It i_init, It e_init)
	: i(i_init), e(e_init)
	{
	}
	
	void put(value_type const& x)
	{
		if(i != e)
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

}

#endif // UUID_4800C387FBD44D146E8D33AE59C4A598
