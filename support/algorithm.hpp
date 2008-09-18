#ifndef UUID_6DD5291E8D2F459FA7A8469E4A95E300
#define UUID_6DD5291E8D2F459FA7A8469E4A95E300

#include <iterator>

namespace gvl
{

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
