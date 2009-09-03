#ifndef UUID_0B359775B44F45D98A1A938A7401BAD6
#define UUID_0B359775B44F45D98A1A938A7401BAD6

#include "bits.hpp"
#include "cstdint.hpp"
#include "platform.hpp"
#include <stdexcept>
#include <utility>

#if GVL_MSVCPP
#include <intrin.h>
#pragma intrinsic(__emul)
#pragma intrinsic(__emulu)
#endif

namespace gvl
{

inline uint64_t emulu(uint32_t x, uint32_t y)
{
#if GVL_MSVCPP
	return __emulu(x, y);
#else
	return uint64_t(x) * y;
#endif
}

struct prepared_division
{
	prepared_division(uint32_t divisor_init)
	: divisor(divisor_init)
	{
		if(divisor == 0)
			throw std::domain_error("Prepared division by zero");
		else if(divisor == 1)
			throw std::invalid_argument("prepared_division cannot divide by 1");
			
		int b = top_bit(divisor);
		
		rshift = b;
		
		if(is_power_of_two(divisor))
		{
			offset = 0;
			multiplier = 0x80000000;
			// The multiplier will cause a right-shift by 1, so decrease rshift
			--rshift;
			return;
		}
		
		uint64_t dividend = (0x100000000ull << rshift);
		uint32_t f = uint32_t(dividend / divisor);
		uint64_t remainder = dividend - uint64_t(f) * divisor;
		if(remainder * 2 < divisor) // remainder * 2 < divisor <=> remainder / divisor < 1/2
		{
			multiplier = f;
			offset = multiplier;
		}
		else
		{
			multiplier = f + 1; // Round up
			offset = 0;
		}
	}
	
	uint32_t multiplier;
	uint32_t offset;
	uint32_t divisor;
	int rshift;
	
	uint32_t quot(uint32_t dividend)
	{
		return uint32_t((emulu(dividend, multiplier) + offset) >> 32) >> rshift;
	}
	
	std::pair<uint32_t, uint32_t> quot_rem(uint32_t dividend)
	{
		uint32_t quotient = quot(dividend);
		uint32_t remainder = dividend - quotient * divisor;
		
		return std::make_pair(quotient, remainder);
	}
};


}

#endif // UUID_0B359775B44F45D98A1A938A7401BAD6
