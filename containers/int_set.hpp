#ifndef UUID_C27C476E17CF4899A94FCBB265827A0F
#define UUID_C27C476E17CF4899A94FCBB265827A0F

#include <cstddef>
#include <stdexcept>

namespace gvl
{

template<typename T>
struct int_set
{
	typedef std::size_t size_type;

	typedef T* iterator;

	int_set(size_type max)
	: n(0)
	, dense(new T[max])
	, sparse(new size_type[max])
	, max(max)
	{

	}

	bool contains(T v)
	{
		if(v >= sparse.size())
			return false;
		if(sparse[v] < n && dense[sparse[v]] == v)
			return true;
		return false;
	}

	void insert(T v)
	{
		if(contains(v))
			return;
		if(v >= max)
			throw std::out_of_range("integer is out of range");
		dense[n] = v;
		sparse[v] = n;
		++n;
	}

	void remove(T v)
	{
		if(!contains(v))
			return;
		size_type dense_pos = sparse[v];
		--n;
		T last = dense[n];
		dense[dense_pos] = last;
		sparse[last] = dense_pos;
	}

	iterator begin()
	{ return dense; }

	iterator end()
	{ return dense + n; }

private:
	size_t n;
	T* dense;
	size_type* sparse;
	size_type max;
};

}

#endif // UUID_C27C476E17CF4899A94FCBB265827A0F
