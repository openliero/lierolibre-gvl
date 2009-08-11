#ifndef UUID_038DE51568F04236FF21B1A4AFF43D3C
#define UUID_038DE51568F04236FF21B1A4AFF43D3C

#include <cassert>
#include <algorithm>
//#include "../containerslist.hpp"
#include "shared.hpp"
#include "../support/debug.hpp"
#include "../support/functional.hpp"

namespace gvl
{

struct weak_ptr_common;

struct no_ownership_transfer {};

struct shared_ptr_common
{
	shared_ptr_common()
	: v(0)
	{
	}
	
	explicit shared_ptr_common(shared* v_init)
	: v(v_init)
	{
		_set(v_init);
		sassert(v);
	}
		
	shared_ptr_common(shared_ptr_common const& b)
	{
		_set(b.v);
	}
		
	~shared_ptr_common()
	{
		_release();
	}

	void reset(shared* v_new)
	{
		shared* old = v; // Handles self-reset.
		_set(v_new);
		if(old)
			old->release();
	}
	
	void _release()
	{
		if(v)
			v->release();
	}
	
	void _set(shared* v_new)
	{
		v = v_new;
		if(v)
			v->add_ref();
	}
	
	mutable shared* v; // This is mutable for the sake of deferred_ptr only, see if we can change that
};

template<typename T>
struct deferred_ptr;

/*
template<typename T>
struct unsafe_weak_ptr;*/

template<typename T>
struct shared_ptr : shared_ptr_common
{
	shared_ptr()
	{ }
	
	explicit shared_ptr(T* v)
	: shared_ptr_common(v)
	{ }
	
	shared_ptr(shared_ptr const& b)
	{
		shared_ptr_common::_set(b.get());
	}

	template<typename SrcT>
	shared_ptr(shared_ptr<SrcT> const& b)
	{
		T* p = b.get();
		shared_ptr_common::_set(p);
	}

	shared_ptr(deferred_ptr<T> const& b);
	shared_ptr& operator=(deferred_ptr<T> const& b);
	/*
	shared_ptr(unsafe_weak_ptr<T> const& b);
	shared_ptr& operator=(unsafe_weak_ptr<T> const& b);
	*/
	shared_ptr& operator=(shared_ptr const& b)
	{
		shared_ptr_common::reset(b.get());
		return *this;
	}

	template<typename SrcT>
	shared_ptr& operator=(shared_ptr<SrcT> const& b)
	{
		T* p = b.get();
		shared_ptr_common::reset(p);
		return *this;
	}
		
	operator void*() const
	{ return v; }
	
	T* operator->() const
	{ sassert(v); return static_cast<T*>(v); }
	
	T& operator*() const
	{ sassert(v); return *static_cast<T*>(v); }
	
	void reset(T* b)
	{ shared_ptr_common::reset(b); }
	
	void reset()
	{ _release(); v = 0; }
	
	shared_ptr release()
	{
		// TODO: We can do this more efficiently
		shared_ptr ret = *this;
		reset();
		return ret;
	}
	
	void swap(shared_ptr& b)
	{ std::swap(v, b.v); }
	
	template<typename DestT>
	shared_ptr<DestT> cast()
	{ return dynamic_cast<DestT*>(get()); }
	
	T* cow()
	{
		sassert(v);
		if(v->ref_count() > 1)
			reset(get()->clone());
		return get();
	}
	
	T* get() const
	{ return static_cast<T*>(v); }
};

// Cheaper, ownership-passing version of shared_ptr
template<typename T>
struct deferred_ptr : shared_ptr_common
{
	deferred_ptr()
	{ }
	
	// takes ownership, v assumed fresh
	explicit deferred_ptr(T* v)
	: shared_ptr_common(v)
	{ }
	
	template<typename SrcT>
	deferred_ptr(shared_ptr<SrcT> const& b)
	{
		T* p = b.get();
		shared_ptr_common::_set(p);
	}
	
	template<typename SrcT>
	deferred_ptr(deferred_ptr<SrcT> const& b)
	{
		v = b.get();
		b.v = 0;
	}
	
	deferred_ptr(deferred_ptr const& b)
	{
		v = b.get();
		b.v = 0;
	}
	
	template<typename SrcT>
	deferred_ptr& operator=(shared_ptr<SrcT> const& b)
	{
		T* p = b.get();
		shared_ptr_common::reset(p);
		return *this;
	}
	
	template<typename SrcT>
	deferred_ptr& operator=(deferred_ptr<SrcT> const& b)
	{
		v = b.get();
		b.v = 0;
		return *this;
	}
	
	deferred_ptr& operator=(deferred_ptr const& b)
	{
		v = b.get();
		b.v = 0;
		return *this;
	}
		
	operator void*() const
	{ return v;	}
	
	T* operator->() const
	{ sassert(v); return v; }
	
	T& operator*() const
	{ assert(v); return *v;	}

	void reset(T* b)
	{ shared_ptr_common::reset(b); }
	
	void reset()
	{ _release(); v = 0; }
	
	void swap(deferred_ptr& b)
	{ std::swap(v, b.v); }
	
	template<typename DestT>
	deferred_ptr<DestT> cast()
	{ return dynamic_cast<DestT*>(get()); }
	
	T* get() const
	{ return static_cast<T*>(v); }
	
};

/*
// Like a normal pointer, but ownership is assumed to belong
// to something else.
template<typename T>
struct unsafe_weak_ptr : shared_ptr_common
{
	unsafe_weak_ptr()
	{ }
	
	// Does not take ownership
	explicit unsafe_weak_ptr(T* v)
	: shared_ptr_common(v)
	{ }
	
	template<typename SrcT>
	unsafe_weak_ptr(shared_ptr<SrcT> const& b)
	: shared_ptr_common(b.get())
	{ }
	
	template<typename SrcT>
	unsafe_weak_ptr(unsafe_weak_ptr<SrcT> const& b)
	: shared_ptr_common(b.get())
	{ }
		
	unsafe_weak_ptr(unsafe_weak_ptr const& b)
	{
		v = b.get();
	}
	
	template<typename SrcT>
	unsafe_weak_ptr& operator=(shared_ptr<SrcT> const& b)
	{
		v = b.get();
		return *this;
	}
	
	template<typename SrcT>
	unsafe_weak_ptr& operator=(unsafe_weak_ptr<SrcT> const& b)
	{
		v = b.get();
		return *this;
	}
	
	unsafe_weak_ptr& operator=(unsafe_weak_ptr const& b)
	{
		v = b.get();
		return *this;
	}
		
	operator void*() const
	{ return v;	}
	
	T* operator->() const
	{ sassert(v); return v; }
	
	T& operator*() const
	{ assert(v); return *v;	}

	void reset()
	{ v = 0; }
	
	void swap(unsafe_weak_ptr& b)
	{ std::swap(v, b.v); }
	
	template<typename DestT>
	unsafe_weak_ptr<DestT> cast()
	{ return dynamic_cast<DestT*>(get()); }
	
	T* get() const
	{ return static_cast<T*>(v); }
};*/

template<typename T>
shared_ptr<T>::shared_ptr(deferred_ptr<T> const& b)
{
	v = b.v;
	b.v = 0;
}
/*
template<typename T>
shared_ptr<T>::shared_ptr(unsafe_weak_ptr<T> const& b)
{
	_set(b.v);
}*/

template<typename T>
shared_ptr<T>& shared_ptr<T>::operator=(deferred_ptr<T> const& b)
{
	_release();
	v = b.v;
	b.v = 0;
	return *this;
}
/*
template<typename T>
shared_ptr<T>& shared_ptr<T>::operator=(unsafe_weak_ptr<T> const& b)
{
	shared_ptr_common::_nonfresh_reset(b.get());
	return *this;
}
*/

} // namespace gvl


#endif // UUID_038DE51568F04236FF21B1A4AFF43D3C
