#include "platform.hpp"

#include "cstdint.hpp"
#include "debug.hpp"

void gvl_test_platform()
{
	sassert(sizeof(uint64_t)*CHAR_BIT == 64);
	sassert(sizeof(uint32_t)*CHAR_BIT == 32);
	sassert(sizeof(uint16_t)*CHAR_BIT == 16);
	sassert(sizeof(uint8_t)*CHAR_BIT == 8);
	sassert(sizeof(int64_t)*CHAR_BIT == 64);
	sassert(sizeof(int32_t)*CHAR_BIT == 32);
	sassert(sizeof(int16_t)*CHAR_BIT == 16);
	sassert(sizeof(int8_t)*CHAR_BIT == 8);
	
	// Test endianness
	uint32_t v = 0xAABBCCDD;
	uint8_t first = reinterpret_cast<uint8_t*>(&v)[0];
	uint8_t second = reinterpret_cast<uint8_t*>(&v)[1];
#if GVL_ENDIAN == GVL_BIG_ENDIAN
	sassert(first == 0xAA && second == 0xBB);
#else
	sassert(first == 0xDD && second == 0xCC);
#endif

	// Test integer assumptions
	passert((-1>>31) == -1, "Signed right-shift must duplicate sign bit");
	passert((-1/2) == 0, "Division must round towards 0");
}
