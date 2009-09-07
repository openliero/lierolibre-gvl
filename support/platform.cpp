#include "platform.hpp"

#include "cstdint.hpp"
#include "debug.hpp"

void gvl_test_platform()
{
	GVL_STATIC_ASSERT(sizeof(uint64_t)*CHAR_BIT == 64);
	GVL_STATIC_ASSERT(sizeof(uint32_t)*CHAR_BIT == 32);
	GVL_STATIC_ASSERT(sizeof(uint16_t)*CHAR_BIT == 16);
	GVL_STATIC_ASSERT(sizeof(uint8_t)*CHAR_BIT == 8);
	GVL_STATIC_ASSERT(sizeof(int64_t)*CHAR_BIT == 64);
	GVL_STATIC_ASSERT(sizeof(int32_t)*CHAR_BIT == 32);
	GVL_STATIC_ASSERT(sizeof(int16_t)*CHAR_BIT == 16);
	GVL_STATIC_ASSERT(sizeof(int8_t)*CHAR_BIT == 8);
	
	// Test endianness
	uint32_t v = 0xAABBCCDD;
	uint8_t first = reinterpret_cast<uint8_t*>(&v)[0];
	uint8_t second = reinterpret_cast<uint8_t*>(&v)[1];
#if GVL_BIG_ENDIAN
	sassert(first == 0xAA && second == 0xBB);
#else
	sassert(first == 0xDD && second == 0xCC);
#endif

	// Test integer assumptions
	GVL_STATIC_ASSERT((-1>>31) == -1); // Signed right-shift must duplicate sign bit
	GVL_STATIC_ASSERT((-1/2) == 0); // Division must round towards 0
}
