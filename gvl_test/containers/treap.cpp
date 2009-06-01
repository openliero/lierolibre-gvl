#include <gvl/tut/tut.hpp>

#include <gvl/containers/treap.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <functional>

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

template<>
template<>
void object::test<1>()
{
	typedef gvl::treap<treap_integer> l1_t;
	l1_t l1;
	
#if 0
	gvl::tt800 r(1234);
	
	for(int repeat = 0; repeat < 100; ++repeat)
	{
		l1.insert(new treap_integer(r.range(0, 1000)));
	}
	
	l1.integrity_check();
#endif
}

} // namespace tut
