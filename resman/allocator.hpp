#ifndef GVL_ALLOCATOR_HPP
#define GVL_ALLOCATOR_HPP

#include <memory>
#include <cstdlib>

namespace gvl
{

template<class T>
struct malloc_allocator
{
	typedef T                 value_type;
	typedef value_type*       pointer;
	typedef value_type const* const_pointer;
	typedef value_type&       reference;
	typedef value_type const& const_reference;
	typedef std::size_t       size_type;
	typedef std::ptrdiff_t    difference_type;

	malloc_allocator() {}
	malloc_allocator(malloc_allocator const&) {}
	~malloc_allocator() {}

	pointer address(reference x) const
	{ return &x; }
	
	const_pointer address(const_reference x) const
	{ return &x; }

	pointer allocate(size_type n, const_pointer = 0)
	{
		void* p = std::malloc(n * sizeof(T));
		if (!p)
			throw std::bad_alloc();
		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type)
	{
		std::free(p);
	}

	size_type max_size() const
	{ 
		return static_cast<size_type>(-1) / sizeof(value_type);
	}

	void construct(pointer p, value_type const& x)
	{
		new(p) value_type(x);
	}
	
	void destroy(pointer p) { p->~value_type(); }
private:
	void operator=(malloc_allocator const&);
};

template<class T>
inline bool operator==(malloc_allocator<T> const&,
                       malloc_allocator<T> const&)
{
  return true;
}

template<class T>
inline bool operator!=(malloc_allocator<T> const&,
                       malloc_allocator<T> const&)
{
  return false;
}


}

#endif // GVL_ALLOCATOR_HPP
