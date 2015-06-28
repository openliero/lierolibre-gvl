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

#ifndef UUID_7D332649F1E24EAC587F0386AD08B2CC
#define UUID_7D332649F1E24EAC587F0386AD08B2CC

/* NOTE: Keep this usable from C */

#include <stddef.h>

#if !defined(GVL_CPP)
#if defined(__cplusplus)
#define GVL_CPP 1
#else
#define GVL_CPP 0
#endif
#endif

#if !defined(GVL_WIN64)
# if defined(WIN64) || defined(_WIN64) /* TODO: Check for Cygwin */
#  define GVL_WIN64 1
# else
#  define GVL_WIN64 0
# endif
#endif

#if !defined(GVL_WIN32)
# if defined(WIN32) || defined(_WIN32) /* TODO: Check for Cygwin */
#  define GVL_WIN32 1
# else
#  define GVL_WIN32 0
# endif
#endif

#if !defined(GVL_WINDOWS)
# define GVL_WINDOWS (GVL_WIN32 || GVL_WIN64)
#endif

#if !defined(GVL_MSVCPP)
# if defined(_MSC_VER)
#  define GVL_MSVCPP _MSC_VER
# else
#  define GVL_MSVCPP 0
# endif
#endif

#if !defined(GVL_GCC)
# if defined(__GNUC__)
#  define GVL_GCC 1
# else
#  define GVL_GCC 0
# endif
#endif

#if !defined(GVL_LINUX)
# if defined(__linux__) || defined(linux) || defined(__GLIBC__)
#  define GVL_LINUX 1
# else
#  define GVL_LINUX 0
# endif
#endif

#if !GVL_X86 && !GVL_X86_64
# if defined(_M_X64) || defined(__x86_64__) || GVL_WIN64
#  define GVL_X86_64 1
# elif defined(__i386__) || defined(_M_IX86) || defined(i386) || defined(i486) || defined(intel) || defined(x86) || defined(i86pc)
#  define GVL_X86 1
# endif
#endif

#if !GVL_LITTLE_ENDIAN && !GVL_BIG_ENDIAN
# if GVL_X86 || GVL_X86_64 || __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN
#  define GVL_LITTLE_ENDIAN 1
# else
#  define GVL_BIG_ENDIAN 1
# endif
#endif

#if defined(__cplusplus) // We don't test GVL_CPP here, because we assume we can always use C++ inline in C++
# define GVL_INLINE inline
# if defined(GVL_GCC)
#   define GVL_FORCE_INLINE inline
# elif defined(GVL_MSVCPP)
#   define GVL_FORCE_INLINE __forceinline
# else
#   define GVL_FORCE_INLINE inline
# endif
#elif GVL_GCC
# define GVL_INLINE static inline
# define GVL_FORCE_INLINE static inline
#elif GVL_MSVCPP
# define GVL_INLINE __inline
# define GVL_FORCE_INLINE __inline
#else
# define GVL_INLINE static
# define GVL_FORCE_INLINE static
#endif

#if !defined(GVL_CPP0X)
# if GVL_MSVCPP >= 1600
#  define GVL_CPP0X 1 // C++0x level 1
# else
#  define GVL_CPP0X 0
# endif
#endif

/* Whether or not the compiler may generate x87 code for floating point calculations.
** GVL_X87 == 1 means the gvl IEEE support functions will take measures to work-around
** x87 issues that make results non-reproducible. */
#if !defined(GVL_X87)
# if GVL_X86
#  define GVL_X87 1 // Assume the compiler generates x87 code on x86 unless otherwise stated
# elif GVL_X86_64
#  define GVL_X87 0 // SSE2 is typically used on GVL_X86_64
# endif
#endif

/* Whether or not types can be read from unaligned addresses */
#if !defined(GVL_UNALIGNED_ACCESS)
# if GVL_X86 || GVL_X86_64
#  define GVL_UNALIGNED_ACCESS 1
# else
#  define GVL_UNALIGNED_ACCESS 0
# endif
#endif

/* At least x86 and x86_64 provide efficient masked shift counts in shifts */
#if !defined(GVL_MASKED_SHIFT_COUNT)
# if GVL_X86 || GVL_X86_64
#  define GVL_MASKED_SHIFT_COUNT 1
# else
#  define GVL_MASKED_SHIFT_COUNT 0
# endif
#endif

#ifndef GVL_DIV_ROUNDS_TOWARD_ZERO
#define GVL_DIV_ROUNDS_TOWARD_ZERO ((-1/2)==0)
#endif

#ifndef GVL_SIGN_EXTENDING_RIGHT_SHIFT
#define GVL_SIGN_EXTENDING_RIGHT_SHIFT (((-1)>>15)==-1)
#endif

#ifndef GVL_TWOS_COMPLEMENT
#define GVL_TWOS_COMPLEMENT (~(-1)==0)
#endif


#if !defined(GVL_WINDOWS)
#define GVL_WINDOWS (GVL_WIN64 || GVL_WIN32)
#endif

#if !defined(GVL_PTR64)
#if GVL_WIN64
#define GVL_PTR64 1
#else
#define GVL_PTR64 0
#endif
#endif

/* This function checks whether the above inferred
** characteristics are correct. It will throw gvl::assert_failure if not. */
void gvl_test_platform();

#endif // UUID_7D332649F1E24EAC587F0386AD08B2CC
