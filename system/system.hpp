#ifndef UUID_A0E64B040F4F41B4EC933B89A48C42C7
#define UUID_A0E64B040F4F41B4EC933B89A48C42C7

#include "../support/cstdint.hpp"

namespace gvl
{

/* A timer incrementing once per millisecond */
uint32_t get_ticks();

void sleep(uint32_t ms);

}

#endif // UUID_A0E64B040F4F41B4EC933B89A48C42C7

