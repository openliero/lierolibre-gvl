#ifndef UUID_4800C387FBD44D146E8D33AE59C4A598
#define UUID_4800C387FBD44D146E8D33AE59C4A598

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
		if(cached_)
			return val;
		val = func_();
		cached_ = true;
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

}

#endif // UUID_4800C387FBD44D146E8D33AE59C4A598
