#ifndef UUID_A426FCF03B0B485CD32991BFACDF17CD
#define UUID_A426FCF03B0B485CD32991BFACDF17CD

#include <cstddef>
#include "../support/functional.hpp"

namespace gvl
{

struct default_xlist_tag
{};

struct xlist_node_common
{
	xlist_node_common* mirror(xlist_node_common* other)
	{
		return reinterpret_cast<xlist_node_common*>(std::ptrdiff_t(other) ^ prevnext);
	}

	std::ptrdiff_t prevnext;
};

template<typename Tag = default_xlist_tag>
struct xlist_node : xlist_node_common
{
	template<typename T>
	static T* downcast(xlist_node<Tag>* p)
	{
		return static_cast<T*>(static_cast<xlist_node<Tag>*>(p));
	}

	xlist_node()
	{
		prevnext = 0;
	}

	template<typename T>
	static xlist_node<Tag>* upcast(T* p)
	{
		return static_cast<xlist_node<Tag>*>(p);
	}
};

struct xlist_common
{
	xlist_node_common* first;
	xlist_node_common* last;
};

template<
	typename T,
	typename Tag = default_xlist_tag,
	typename Deleter = default_delete>
struct xlist : xlist_common, protected Deleter
{
	template<typename T>
	static T* downcast(xlist_node<Tag>* p)
	{
		return static_cast<T*>(static_cast<xlist_node<Tag>*>(p));
	}

	template<typename T>
	static xlist_node<Tag>* upcast(T* p)
	{
		return static_cast<xlist_node<Tag>*>(p);
	}

	struct range
	{

	};

	void push_back(T* x_)
	{
#if 0
		xlist_node_common* x = upcast(x_);

		if(last)
		{
			last->prevnext ^= reinterpret_cast<std::ptrdiff_t>(x) ^ 0;
			x->prevnext = reinterpret_cast<std::ptrdiff_t>(last) ^ 0;
		}
		else
		{
			x->prevnext = 0 ^ 0;
			first = x;
		}
		last = x;
#else
		xlist_node_common* x = upcast(x_);

		last->prevnext ^= reinterpret_cast<std::ptrdiff_t>(x) ^ 0;
		x->prevnext = reinterpret_cast<std::ptrdiff_t>(last) ^ 0;
		last = x;

		// We start with:
		// xlist_node_common sentinel;
		// xlist_node_common* last = &sentinel;

		// sentinel.prevnext = &sentinel ^ &sentinel == 0

		// last node == last
		// first node == last ^ sentinel.prevnext

		// Push back a node (n):
		// last.prevnext ^= &sentinel ^ n
		// sentinel.prevnext ^= last ^ n // last is no longer prev, but n is
		// n.prevnext = last ^ &sentinel
		// last = n

		// Result:
		// sentinel.prevnext = sentinel.prevnext ^ &sentinel ^ n = (&sentinel ^ &sentinel) ^ &sentinel ^ n = &sentinel ^ n
		// sentinel.prevnext = (&sentinel ^ n) ^ (last ^ n) = (&sentinel ^ n) ^ (&sentinel ^ n) = 0
		// last node == last == n
		// first node == last ^ 0 == n
#endif
	}
};

} // namespace gvl

#endif // UUID_A426FCF03B0B485CD32991BFACDF17CD
