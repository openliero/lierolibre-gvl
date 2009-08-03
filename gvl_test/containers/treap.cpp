#include <gvl/tut/tut.hpp>

#include <gvl/containers/treap2.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <functional>
#include <cmath>
#include <set>
#include <ctime>

namespace tut
{

/*
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
};*/

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
#if 1
	//typedef gvl::treap<treap_integer> l1_t;
	typedef std::set<int> l2_t;
	typedef gvl::treap2<treap_integer2> l3_t;
	//l1_t l1;
	l2_t l2;
	l3_t l3;
	
#if 1
	gvl::tt800 r(1234);
	
	int const iter = 1500000/*0000*/;
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
			l2.insert(/*repeat*/r.range(0, limit));
		}
	}


	{
		timer _;
		
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			l3.insert(new treap_integer2(/*repeat*/r.range(0, limit)));
		}
	}
	
	printf("%f, %f\n", l3.depth() / log2((double)l3.size()), l3.average_depth() / log2((double)l3.size()));
	printf("%f\n", l3.rotations / (double)l3.size());
	
	
	int sum = 0;
	
	{
		timer _;
		
		
		
		for(int repeat = 0; repeat < 30; ++repeat)
		{
			for(l2_t::iterator iter = l2.begin(); iter != l2.end(); ++iter)
			{
				sum += *iter;
			}
		}
	}
	
	{
		timer _;
		
		for(int repeat = 0; repeat < 30; ++repeat)
		{
			int v = r.range(0, limit);
			
			for(l3_t::range iter = l3.all(); !iter.empty(); iter.pop_front())
			{
				sum += iter.front().v;
			}
		}
	}
	
	printf("%d\n", sum);
	
	
	{
		timer _;
		
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			int v = r.range(0, limit);
			
			l2_t::iterator iter = l2.find(v);
			if(iter != l2.end())
				l2.erase(iter);
		}
	}
	
	{
		timer _;
		
		for(int repeat = 0; repeat < iter; ++repeat)
		{
			int v = r.range(0, limit);
			
			l3_t::range iter = l3.find(v);
			if(!iter.empty())
				l3.erase_front(iter);
				
			//l3.integrity_check();
		}
	}
	
#if 0
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
#endif
	
	
	
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

#endif
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

