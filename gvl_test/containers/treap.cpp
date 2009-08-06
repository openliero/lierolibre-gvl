#include <gvl/tut/tut.hpp>

#include <gvl/containers/treap.hpp>
#include <gvl/containers/treap2.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <gvl/support/profile.hpp>
#include <functional>
#include <cmath>
#include <set>
#include <ctime>
#include <iostream>

namespace tut
{

struct treap_integer : gvl::treap_node<>
{
	treap_integer(int v)
	: v(v)
	{
	}
	
	bool operator<(treap_integer const& b) const
	{
		return v < b.v;
	}
	
	int v;
};

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
		printf("Time: %f\n", (stop - start) / (double)(CLOCKS_PER_SEC));
	}
	
	std::clock_t start;
};

template<>
template<>
void object::test<1>()
{
	typedef gvl::treap<treap_integer> l1_t;
	typedef std::set<int> l2_t;
	typedef gvl::treap2<treap_integer2> l3_t;
	l1_t l1;
	l2_t l2;
	l3_t l3;
	
#if 1
	gvl::tt800 r(1234);
	
	int const iter = 900000/*000*/;
	int const limit = iter;
	
	printf("\n\n");
	
#if 0
	{
		timer _;
		
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			l1.insert(new treap_integer(r.range(0, limit)));
		}
	}

	// 4.447000
#endif
	{
		timer _;
		
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			uint32_t v = r.range(0, limit);
			l2.insert(v);
			l2.insert(v + 1);
			l2.insert(v + 3);
		}
	}


	{
		timer _;
		
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			uint32_t v = r.range(0, limit);
			l3.insert(new treap_integer2(v));
			l3.insert(new treap_integer2(v + 1));
			l3.insert(new treap_integer2(v + 3));
		}
	}
	
	{
		timer _;
		
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			int v = r.range(0, limit);
			
			l2.find(v);
		}
	}
	
	{
		timer _;
		
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			int v = r.range(0, limit);
			
			l3.find(v);
		}
	}
	
	printf("%f, %f\n", l3.depth() / log2((double)l3.size()), l3.average_depth() / log2((double)l3.size()));
	printf("%f\n", l3.rotations / (double)l3.size());
	
	gvl::present_profile(std::cout);
	
#if 0
	for(int repeat = 0; repeat < 3000000; ++repeat)
	{
		l1.insert(new treap_integer(r.range(0, 100000000)));
		
		/*
		for(l1_t::range i = l1.all(); !i.empty(); i.pop_front())
		{
			printf("%d ", i.front().v);
		}
		
		printf(" at %d\n", repeat);
		*/
		
		l1.integrity_check();
		
		if((repeat & 0xfff) == 0)
			printf("%f\n", l1.depth() / log2((double)l1.size()));
	}
#endif
	
#endif
}

} // namespace tut

