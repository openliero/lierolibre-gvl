#include <tut/tut.hpp>

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

int32_t reference_saturate0(int32_t x)
{
	return x < 0 ? 0 : x;
}

uint32_t reference_bswap(uint32_t x)
{
	return (x >> 24)
	| ((x >> 8) & 0xff00)
	| ((x << 8) & 0xff0000)
	| ((x << 24) & 0xff000000);
}

uint64_t reference_bswap(uint64_t x)
{
	return reference_bswap(uint32_t(x >> 32)) | (uint64_t(reference_bswap(uint32_t(x))) << 32);
}

template<>
template<>
void object::test<1>()
{
	gvl::mwc rand;
	for(int i = 0; i < 1000000; ++i)
	{
		uint32_t x = rand();
		uint32_t x64 = (uint64_t(rand()) << 32) | x;
		ensure("trailing_zeroes", reference_trailing_zeroes(x) == gvl::trailing_zeroes(x));
		ensure("bottom_bit", reference_bottom_bit(x) == gvl::bottom_bit(x));
		ensure("log2", reference_log2(x) == gvl::log2(x));
		ensure("top_bit", reference_top_bit(x) == gvl::top_bit(x));
		ensure("saturate0", reference_saturate0(int32_t(x)) == gvl::saturate0(int32_t(x)));
		ensure("bswap", reference_bswap(x) == gvl::bswap(x));
		ensure("bswap 64-bit", reference_bswap(x64) == gvl::bswap(x64));
	}
}

}
