#ifndef UUID_9F0036A7B22E453BA9D2A6810D1B70DD
#define UUID_9F0036A7B22E453BA9D2A6810D1B70DD

namespace gvl
{

template<typename T>
struct basic_flags
{
	basic_flags(T f)
	: flags_(f)
	{
	}

	void replace(T f, T mask)
	{
		flags_ ^= ((flags_ ^ f) & mask);
	}
	
	void set(T f)
	{
		flags_ |= f;
	}
	
	void reset(T f)
	{
		flags_ &= ~f;
	}

	void replace(T f)
	{
		flags_ = f;
	}
	
	bool any(T f) const
	{
		return (flags_ & f) != 0;
	}
	
	bool all(T f) const
	{
		return (flags_ & f) == f;
	}
	
	bool no(T f) const
	{
		return (flags_ & f) == 0;
	}
	
	void toggle(T f)
	{
		flags_ ^= f;
	}
	
	T as_integer()
	{
		return flags_;
	}
	
private:
	T flags_;
};

typedef basic_flags<unsigned int> flags;

} // namespace gvl

#endif // UUID_9F0036A7B22E453BA9D2A6810D1B70DD
