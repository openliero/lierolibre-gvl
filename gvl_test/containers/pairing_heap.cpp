#include <gvl/tut/tut.hpp>

#include <gvl/containers/pairing_heap.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <functional>
#include <cmath>
#include <set>
#include <ctime>

namespace tut
{

struct pairing_heap_integer : gvl::pairing_node<>
{
	pairing_heap_integer(int v)
	: v(v)
	{
	}
	
	bool operator<(pairing_heap_integer const& b) const
	{
		return v < b.v;
	}
	
	int v;
};

struct pairing_heap_data
{
	
};

typedef test_group<pairing_heap_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::pairing_heap");
} // namespace

namespace tut
{

/*
template<typename T>
struct greater : std::binary_function<T const&, T const&, bool>
{
	bool operator()(T const& a, T const& b) const
	{
		return b < a;
	}
};*/

template<>
template<>
void object::test<1>()
{
	typedef gvl::pairing_heap<pairing_heap_integer> h_t;
	
	h_t heap;
	
	int const count = 1000;
	std::vector<int> added;
	gvl::tt800 rand(1);
	
	for(int i = 0; i < count; ++i)
	{
		int v = rand.range(0, 10000);
		
		added.push_back(v);
		heap.insert(new pairing_heap_integer(v));
	}
	
	std::sort(added.begin(), added.end());
	
	for(int i = 0; i < count; ++i)
	{
		ensure("not empty", !heap.empty());
		ensure("all items are present and popped in the right order", added[i] == heap.min().v);
		heap.erase_min();
	}
}

} // namespace tut
