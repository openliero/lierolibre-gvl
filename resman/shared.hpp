#ifndef UUID_5F7548A068F9485B6759368B5BAE8157
#define UUID_5F7548A068F9485B6759368B5BAE8157

namespace gvl
{

struct weak_ptr_common;

struct shared
{
	friend struct weak_ptr_common;
	
	shared()
	: _ref_count(1), _first(0)
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
	
private:
	void _delete();
	
	int _ref_count;
	weak_ptr_common* _first;
};

} // namespace gvl

#endif // UUID_5F7548A068F9485B6759368B5BAE8157
