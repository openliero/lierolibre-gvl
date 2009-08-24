#include <gvl/tut/tut.hpp>

#include <gvl/containers/treap2.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <functional>
#include <cmath>
#include <set>
#include <ctime>
#include <cstdio>

namespace tut
{

struct treap_integer2 : gvl::treap_node2<>
{
	treap_integer2(int v)
	: v(v)
	{
	}
	
	bool operator<(treap_integer2 const& b) const
	{
		return v < b.v;
	}
	
	int v;
};

struct treap_data
{
	
};

typedef test_group<treap_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::treap");
} // namespace

namespace tut
{

template<typename T>
struct greater : std::binary_function<T const&, T const&, bool>
{
	bool operator()(T const& a, T const& b) const
	{
		return b < a;
	}
};

double log2(double x)
{
	return std::log(x) / std::log(2.0);
}

struct timer
{
	timer()
	: start(std::clock())
	{
	}
	
	~timer()
	{
		std::clock_t stop = std::clock();
		std::printf("Time: %f\n", (stop - start) / (double)(CLOCKS_PER_SEC));
	}
	
	std::clock_t start;
};

template<>
template<>
void object::test<1>()
{
	//typedef gvl::treap<treap_integer> l1_t;
	typedef std::set<int> l2_t;
	typedef gvl::treap2<treap_integer2> l3_t;
	//l1_t l1;
	/*l2_t l2;*/
	l3_t l3;
	
	gvl::tt800 r(1234);
	
	int const iter = 50000;
	int const limit = iter;
	

	{
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			l3.insert(new treap_integer2(/*repeat*/r.range(0, limit)));
		}
	}
	
	
	int sum = 0;
	
	{
		for(int repeat = 0; repeat < 30; ++repeat)
		{
			int v = r.range(0, limit);
			
			for(l3_t::range iter = l3.all(); !iter.empty(); iter.pop_front())
			{
				sum += iter.front().v;
			}
		}
	}
	
	{
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			int v = r.range(0, limit);
			
			l3_t::range iter = l3.find(v);
			if(!iter.empty())
				l3.erase_front(iter);
				
			//l3.integrity_check();
		}
	}
		
	{
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			int v = r.range(0, limit);
			
			l3.find(v);
		}
	}
	
}

template<>
template<>
void object::test<2>()
{
	typedef gvl::treap2<treap_integer2> l1_t;
	l1_t l1;
	
	gvl::tt800 r(1234);
	
	int const iter = 1500000;
	
	
}

} // namespace tut

