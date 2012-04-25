#ifndef UUID_45275E9930944E2D32B8A686DB5E647A
#define UUID_45275E9930944E2D32B8A686DB5E647A

namespace gvl
{

#if 0
struct auto_any_base
{
    operator bool() const
    {
        return false;
    }
};

template<typename T>
struct auto_any : auto_any_base
{
	auto_any(T const& t)
	: item(t)
	{
	}

	mutable T item;
};

typedef auto_any_base const& auto_any_t;

T* type_as_pointer(T const&)
{
	return 0;
}

template<typename T>
inline auto_any<T> to_auto_any(T const& t)
{
    return t;
}

#define GVL_TYPEOF_AS_POINTER(V)    (true ? 0 : type_as_pointer(V))

#define GVL_EVAL_AUTO_ANY(expr) to_auto_any(expr)

if(auto_any_t _iter = GVL_EVAL_AUTO_ANY(iter)) {} else
for(;

#endif

#include <iterator>
#include <cstdlib>

// Deprecated, see range.hpp
template<typename ForwardIterator>
struct iterator_range
{
	typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename std::iterator_traits<ForwardIterator>::difference_type difference_type;
	typedef typename std::iterator_traits<ForwardIterator>::distance_type distance_type;
	typedef typename std::iterator_traits<ForwardIterator>::pointer pointer;
	typedef typename std::iterator_traits<ForwardIterator>::reference reference;
	typedef std::size_t size_type;

	typedef ForwardIterator iterator;

	iterator_range(ForwardIterator begin_init, ForwardIterator end_init)
	: begin_(begin_init)
	, end_(end_init)
	{
	}

	ForwardIterator begin()
	{
		return begin_;
	}

	ForwardIterator end()
	{
		return end_;
	}

	std::size_t size() const
	{
		return end_ - begin_;
	}

private:
	ForwardIterator begin_;
	ForwardIterator end_;
};

} // namespace gvl

#endif // UUID_45275E9930944E2D32B8A686DB5E647A
