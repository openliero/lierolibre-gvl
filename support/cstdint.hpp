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

#ifndef UUID_EA12C28D969947BF0340D69440AE5D30
#define UUID_EA12C28D969947BF0340D69440AE5D30

// NOTE: Keep this useable from C

#include <limits.h>

#include "platform.hpp"

#if GVL_GCC || _MSC_VER >= 1600
#include <stdint.h>
#else /* !GVL_GCC */

#if !GVL_EXCLUDE_STDINT

#if CHAR_BIT == 8 && UCHAR_MAX == 0xff
typedef unsigned char      uint8_t;
typedef   signed char      int8_t;
#else
#error "Only 8-bit chars supported"
#endif

typedef unsigned int       uint_fast8_t;
typedef          int       int_fast8_t;

#if USHRT_MAX == 0xffff
typedef unsigned short     uint16_t;
typedef          short     int16_t;
#elif UINT_MAX == 0xffff
typedef unsigned int       uint16_t;
typedef          int       int16_t;
#else
#error "No suitable 16-bit type"
#endif

#if USHRT_MAX == 0xffffffff
typedef unsigned short     uint32_t;
typedef          short     int32_t;
#elif UINT_MAX == 0xffffffff
typedef unsigned int       uint32_t;
typedef          int       int32_t;
#elif ULONG_MAX == 0xffffffff
typedef unsigned long      uint32_t;
typedef          long      int32_t;
#else
#error "No suitable 32-bit type"
#endif

/* We found a 32-bit type above, int should do */
#if GVL_X86 || GVL_X86_64
/* long should match the register size */
typedef unsigned long    uint_fast16_t;
typedef          long    int_fast16_t;
typedef unsigned long    uint_fast32_t;
typedef          long    int_fast32_t;
#elif UINT_MAX < 0xffffffff
/* Have to use long for 32-bit */
typedef unsigned int     uint_fast16_t;
typedef          int     int_fast16_t;
typedef unsigned long    uint_fast32_t;
typedef          long    int_fast32_t;
#else
/* Be conservative with space */
typedef unsigned int     uint_fast16_t;
typedef          int     int_fast16_t;
typedef unsigned int     uint_fast32_t;
typedef          int     int_fast32_t;
#endif

#define IS_64(x) ((x) > 0xffffffff && (x) == 0xffffffffffffffff)

#if IS_64(ULONG_MAX)
typedef unsigned long      uint64_t;
typedef          long      int64_t;
#elif defined(ULLONG_MAX) && IS_64(ULLONG_MAX)
typedef unsigned long long uint64_t;
typedef          long long int64_t;
#else
#error "No suitable 64-bit type"
#endif

#if defined(ULLONG_MAX)
typedef unsigned long long uintmax_t;
typedef          long long intmax_t;
#else
typedef unsigned long      uintmax_t;
typedef          long      intmax_t;
#endif

typedef uint64_t uint_fast64_t;
typedef int64_t int_fast64_t;
typedef ptrdiff_t intptr_t;

#endif

#define GVL_BITS_IN(t) (sizeof(t)*CHAR_BIT)

#endif /* !GVL_GCC */

#endif /* UUID_EA12C28D969947BF0340D69440AE5D30 */
