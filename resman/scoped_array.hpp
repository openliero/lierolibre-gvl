#ifndef UUID_168AFD3798A24A328C6FD4A998C80B4A
#define UUID_168AFD3798A24A328C6FD4A998C80B4A

#include <cstddef>

namespace gvl
{

template<typename T>
struct scoped_array
{
	scoped_array(T* init_arr)
	: arr(init_arr)
	{
	}
	
	~scoped_array()
	{
		delete[] arr;
	}
	
	T& operator[](std::size_t idx)
	{
		return arr[idx];
	}
	
	T const& operator[](std::size_t idx) const
	{
		return arr[idx];
	}
	
private:
	// Non-copyable
	scoped_array(scoped_array const&);
	scoped_array& operator=(scoped_array const&);
	
	T* arr;
};

}

#endif // UUID_168AFD3798A24A328C6FD4A998C80B4A
