/*
 * Copyright (c) 2010, Erik Lindroos <gliptic@gmail.com>
 * This software is released under the The BSD-2-Clause License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UUID_A0E64B040F4F41B4EC933B89A48C42C7
#define UUID_A0E64B040F4F41B4EC933B89A48C42C7

#include "../support/cstdint.hpp"

/* NOTE: Keep this usable from C */

#ifdef __cplusplus
extern "C" {
#endif

// A timer incrementing once per millisecond
uint32_t gvl_get_ticks();

// A timer with higher precision, incrementing hires_ticks_per_sec() ticks
// per second.
// NOTE: The precision isn't necessarily hires_ticks_per_sec()
// per second.
uint64_t gvl_get_hires_ticks();
uint64_t gvl_hires_ticks_per_sec();

void gvl_sleep(uint32_t ms);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
namespace gvl
{

// A timer incrementing once per millisecond
GVL_INLINE uint32_t get_ticks() { return gvl_get_ticks(); }

// A timer with higher precision, incrementing hires_ticks_per_sec() ticks
// per second.
// NOTE: The precision isn't necessarily hires_ticks_per_sec()
// per second.
GVL_INLINE uint64_t get_hires_ticks() { return gvl_get_hires_ticks(); }
GVL_INLINE uint64_t hires_ticks_per_sec() { return gvl_hires_ticks_per_sec(); }

GVL_INLINE void sleep(uint32_t ms) { return gvl_sleep(ms); }

}
#endif

#endif // UUID_A0E64B040F4F41B4EC933B89A48C42C7

