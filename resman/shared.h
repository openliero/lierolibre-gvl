#ifndef UUID_C2F0561FF8D54EC1F56F98B82F02D1C0
#define UUID_C2F0561FF8D54EC1F56F98B82F02D1C0

struct gvl_weak_ptr;

struct gvl_shared
{
	shared()
	: _ref_count(0), _first(0)
	{

	}

	void add_ref()
	{
#if GVL_THREADSAFE
		#error "Not finished"
		// TODO: Interlocked add
		++_ref_count;
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
			// TODO: CAS decrement the counter and check
			cas(
		}
#else
		--_ref_count;
		if(_ref_count == 0)
		{
			_delete();
		}
#endif
	}

	int ref_count() const
	{ return _ref_count; }

	virtual ~shared()
	{
	}

	int _ref_count;
	gvl_weak_ptr* first;
};

#endif // UUID_C2F0561FF8D54EC1F56F98B82F02D1C0
