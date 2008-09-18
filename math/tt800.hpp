#ifndef UUID_6D485E50691B483866E152A1029D5226
#define UUID_6D485E50691B483866E152A1029D5226

#include "../support/cstdint.hpp"

namespace gvl
{

struct tt800
{
	static unsigned int const N = 25;
	static unsigned int const M = 7;
	
	tt800(uint32_t new_seed)
	: k(0)
	{
		seed(new_seed);
	}
	
	void seed(uint32_t new_seed);

	uint32_t operator()()
	{
		if(k == N)
			update();
		
		uint32_t y = x[k++];
		y ^= (y << 7) & 0x2b5b2500; /* s and b, magic vectors */
		y ^= (y << 15) & 0xdb8b0000; /* t and c, magic vectors */
		
		return y;
	}
	
	int32_t range(int32_t a, int32_t b)
	{
		return a + int32_t((b - a) * uint64_t(operator()()) >> 32);
	}
	
	template<typename Archive>
	void serialize(Archive& arch);
	
private:
	void update();
	
	uint32_t x[N];
	uint32_t k;
};

template<typename Archive>
void tt800::serialize(Archive& arch)
{
	for(int i = 0; i < N; ++i)
		arch.uint(32, x[i]);
	arch.uint(32, k);
}

tt800& global_rand();

} // namespace gvl

#endif // UUID_6D485E50691B483866E152A1029D5226
