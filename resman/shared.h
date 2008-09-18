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
		++_ref_count;
	}
	
	void release()
	{
		--_ref_count;
		if(_ref_count == 0)
		{
			_delete();
		}
	}
	
	int ref_count() const
	{ return _ref_count; }

	virtual ~shared()
	{
	}
	
	int ref_count;
	gvl_weak_ptr* first;
};

#endif // UUID_C2F0561FF8D54EC1F56F98B82F02D1C0
