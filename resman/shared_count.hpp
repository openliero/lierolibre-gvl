#ifndef UUID_731CFA4A66D3444BB8D7D6BF25814DEE
#define UUID_731CFA4A66D3444BB8D7D6BF25814DEE

namespace gvl
{

#error "Untested"

/// Shared reference counter optimized for
/// 1 reference.
struct shared_count
{
	shared_count()
	: p(0)
	{
	}
	
	shared_count(shared_count const& b)
	: p(b.inc())
	{
	}
	
	shared_count& operator=(shared_count const& b)
	{
		shared_count n(b);
		swap(n);
	}
	
	~shared_count()
	{
		dec();
	}
	
	void dec()
	{
		if(p)
		{
			if(--(*p) == 0)
				delete p;
		}
		else
			delete p;
	}
	
	void swap(shared_count& b)
	{
		std::swap(p, b.p);
	}
	
	int ref_count() const
	{ return !p ? 1 : *p; }
	
	bool unique() const
	{ return !p || *p == 1; }
	
private:
	int* inc()
	{
		if(!p)
			return new int(2);
		else
		{
			++(*p);
			return p;
		}
	}
	
	int* p; // 0 means ref count of 1
};

} // namespace gvl

#endif // UUID_731CFA4A66D3444BB8D7D6BF25814DEE
