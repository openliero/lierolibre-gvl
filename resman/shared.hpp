#ifndef UUID_5F7548A068F9485B6759368B5BAE8157
#define UUID_5F7548A068F9485B6759368B5BAE8157

namespace gvl
{

struct weak_ptr_common;

struct shared
{
	friend struct weak_ptr_common;
	
	shared()
	: _ref_count(0), _first(0)
	{

	}
	
	void add_ref()
	{
#if GVL_THREADSAFE
		#error "Not finished"
		// TODO: Interlocked increment
#else
		++_ref_count;
#endif
	}
	
	void release()
	{
#if GVL_THREADSAFE
		#error "Not finished"
		if(_ref_count == 1) // 1 means it has to become 0, nobody can increment it after this read
			_delete();
		else
		{
			// TODO: Implement CAS
			int read_ref_count;
			do
			{
				read_ref_count = _ref_count;
			}
			while(!cas(&_ref_count, read_ref_count, read_ref_count - 1));
			
			if(read_ref_count - 1 == 0)
			{
				_clear_weak_ptrs();
				_delete();
			}
		}
#else
		--_ref_count;
		if(_ref_count == 0)
		{
			_clear_weak_ptrs();
			_delete();
		}
#endif
	}
	
	int ref_count() const
	{ return _ref_count; }

	virtual ~shared()
	{
	}
	
private:
	void _delete()
	{
		delete this;
	}
	
	void _clear_weak_ptrs();
	
	
	int _ref_count;
	weak_ptr_common* _first;
};

} // namespace gvl

#endif // UUID_5F7548A068F9485B6759368B5BAE8157
