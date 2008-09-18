#ifndef UUID_6071590BDDFE4E7683887E92F8CDD4FA
#define UUID_6071590BDDFE4E7683887E92F8CDD4FA

#include "debug.hpp"

namespace gvl
{

struct borrow_tag {};

struct lendable
{
#ifndef NDEBUG
	lendable()
	: borrow_count(0)
	{
	}
	
	lendable(lendable const& b, borrow_tag)
	: borrow_count(-1) // Not lendable
	{
	}
	
	~lendable()
	{
		passert(borrow_count == 0, "Object still borrowed");
	}
	
	mutable int borrow_count; // Need to be able to modify via pointers to const
#endif
};

template<typename T>
struct borrowed : T
{
	borrowed(T const& b)
	: T(b, borrow_tag())
#ifndef NDEBUG
	, source(&b)
	, old_borrow_count(b.lendable::borrow_count)
#endif
	{
#ifndef NDEBUG
		passert(b.lendable::borrow_count != -1, "Object is a borrowed instance and cannot be borrowed");
		passert(b.lendable::borrow_count == 0, "Object borrowed already");
		++b.lendable::borrow_count;
		
#endif
	}
	
	borrowed(borrowed const& b)
	: T(b, borrow_tag())
#ifndef NDEBUG
	, source(b.source)
	, old_borrow_count(b.lendable::borrow_count)
#endif
	{
#ifndef NDEBUG
		++b.lendable::borrow_count;
#endif
	}
	
	~borrowed()
	{
#ifndef NDEBUG
		--source->lendable::borrow_count;
		if(source->lendable::borrow_count > old_borrow_count)
			passert(false, "Borrowed object not returned");
		else if(source->lendable::borrow_count < old_borrow_count)
			passert(false, "Borrowed object returned too late"); // This should never be reached
#endif
	}
	
#ifndef NDEBUG
	T const* source;
	int old_borrow_count;
#endif
};

} // namespace gvl

#endif // UUID_6071590BDDFE4E7683887E92F8CDD4FA
