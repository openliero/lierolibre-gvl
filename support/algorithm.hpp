#ifndef UUID_6DD5291E8D2F459FA7A8469E4A95E300
#define UUID_6DD5291E8D2F459FA7A8469E4A95E300

#include <iterator>

namespace gvl
{

template<typename T, void (T::*)(T&)>
struct check_swap
{ typedef T type; };

template<typename T>
void swap(typename check_swap<T, &T::swap>::type& x, T& y)
{ x.swap(y); }

template<typename T>
struct ref_wrapper
{
	ref_wrapper(T& v)
	: v(v)
	{ }
	T& v;
};

template<typename T>
void swap(ref_wrapper<T> x, T& y)
{
	T temp(x.v);
	x.v = y;
	y = temp;
}

template<typename InputIterator, typename StrictWeakOrdering>
bool is_sorted(InputIterator b, InputIterator e, StrictWeakOrdering comp)
{
	typedef std::iterator_traits<InputIterator> traits;
	typedef typename traits::value_type value_type;
	typedef typename traits::pointer pointer;
	
	pointer prev = &*b;
	
	for(++b; b != e; ++b)
	{
		pointer cur = &*b;
		if(comp(*cur, *prev))
			return false;
	}
	
	return true;
}

template<typename InputIterator>
bool is_sorted(InputIterator b, InputIterator e)
{
	return is_sorted(b, e, std::less<
		typename std::iterator_traits<InputIterator>::value_type>());
}

} // namespace gvl

#endif // UUID_6DD5291E8D2F459FA7A8469E4A95E300
