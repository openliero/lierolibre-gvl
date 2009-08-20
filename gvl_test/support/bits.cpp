#include <gvl/tut/tut.hpp>

#include <gvl/math/cmwc.hpp>
#include <gvl/support/bits.hpp>

namespace tut
{

struct bits_data
{
	
};

typedef test_group<bits_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("bit operations");
} // namespace

namespace tut
{

int reference_trailing_zeroes(uint32_t v)
{
	int c = 0;
	if(!v) return 0;
	while(!((v >> c) & 1))
	{
		++c;
	}
	return c;
}

int reference_bottom_bit(uint32_t v)
{
	if(!v) return -1;
	return reference_trailing_zeroes(v);
}

int reference_log2(uint32_t v)
{
	int c = 0;
	while((v >>= 1))
		++c;
	return c;
}

int reference_top_bit(uint32_t v)
{
	if(!v) return -1;
	return reference_log2(v);
}

template<>
template<>
void object::test<1>()
{
	gvl::mwc rand;
	for(int i = 0; i < 1000000; ++i)
	{
		uint32_t x = rand();
		ensure("trailing_zeroes", reference_trailing_zeroes(x) == gvl::trailing_zeroes(x));
		ensure("bottom_bit", reference_bottom_bit(x) == gvl::bottom_bit(x));
		ensure("log2", reference_log2(x) == gvl::log2(x));
		ensure("top_bit", reference_top_bit(x) == gvl::top_bit(x));
	}
}

}
