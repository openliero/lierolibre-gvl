#ifndef UUID_A0E64B040F4F41B4EC933B89A48C42C7
#define UUID_A0E64B040F4F41B4EC933B89A48C42C7

#include "../support/cstdint.hpp"

namespace gvl
{

// A timer incrementing once per millisecond
uint32_t get_ticks();

// A timer with higher precision, incrementing hires_ticks_per_sec() ticks
// per second.
// NOTE: The precision isn't necessarily hires_ticks_per_sec()
// per second.
uint64_t get_hires_ticks();
uint64_t hires_ticks_per_sec();

void sleep(uint32_t ms);

}

#endif // UUID_A0E64B040F4F41B4EC933B89A48C42C7

