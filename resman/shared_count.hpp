#ifndef UUID_731CFA4A66D3444BB8D7D6BF25814DEE
#define UUID_731CFA4A66D3444BB8D7D6BF25814DEE

namespace gvl
{

// TODO: This is not thread-safe at all. If GVL_THREADSAFE is defined
// it should be thread-safe.

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
		if(!p)
		{
			delete p;
		}
		else if(--(*p) == 0)
		{
			delete p;
		}
	}
	
	void swap(shared_count& b)
	{
		std::swap(p, b.p);
	}
	
	// release the current count and make a new one set to 1
	void reset()
	{
		dec();
		p = 0;
	}
		
	int ref_count() const
	{ return !p ? 1 : *p; }
	
	bool unique() const
	{ return !p || *p == 1; }
	
private:
	int* inc() const
	{
		if(!p)
		{
			p = new int(2);
			return p;
		}
		else
		{
			++(*p);
			return p;
		}
	}
	
	mutable int* p; // 0 means ref count of 1
};

} // namespace gvl

#endif // UUID_731CFA4A66D3444BB8D7D6BF25814DEE
