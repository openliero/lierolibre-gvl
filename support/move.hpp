#ifndef UUID_3EEC2EFDDD1945483B3E30B8FE895AA9
#define UUID_3EEC2EFDDD1945483B3E30B8FE895AA9

namespace gvl
{

// NOTE: One can't bind rvalues to move_holder. Instead
// one has to explicitly use rvalue() and make sure it's only
// used for rvalues.

template<typename T>
struct move_holder
{
	move_holder(T& v)
	: v(v)
	{
	}

	operator T&()
	{ return v; }

	T& operator*()
	{ return v; }

	T* operator->()
	{ return &v; }

private:
	T& v;
};

template<typename T>
inline move_holder<T> move(T& v)
{
	return move_holder<T>(v);
}

template<typename T>
inline move_holder<T> rvalue(T const& v)
{
	return move_holder<T>(const_cast<T&>(v));
}

} // namespace gvl

#endif // UUID_3EEC2EFDDD1945483B3E30B8FE895AA9
