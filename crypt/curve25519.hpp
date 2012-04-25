#ifndef UUID_A80850D4219545B8CF0EA18AA088876D
#define UUID_A80850D4219545B8CF0EA18AA088876D

#include <cstddef>
#include "../support/cstdint.hpp"

namespace gvl
{

struct int10
{
	int32_t& operator[](std::size_t i)
	{ return limb[i]; }

	int32_t operator[](std::size_t i) const
	{ return limb[i]; }

	int32_t limb[10];
};

struct curve25519
{
	static bool sign(uint8_t* v, uint8_t const* h, uint8_t const* x, uint8_t const* s);
	static void verify(uint8_t* Y, uint8_t const* v, uint8_t const* h, uint8_t const* P);
	static void clamp(uint8_t* k);
	static void keygen(uint8_t* P, uint8_t* s, uint8_t* k);
	static void curve(uint8_t* Z, uint8_t* k, uint8_t* P);
};

}

#endif // UUID_A80850D4219545B8CF0EA18AA088876D
