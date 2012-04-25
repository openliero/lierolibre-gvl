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

#ifndef UUID_54B4E13DC6CC41B073E60A941244DFBB
#define UUID_54B4E13DC6CC41B073E60A941244DFBB

// Adapted from PortAudio

#if defined(__APPLE__)
#   include <libkern/OSAtomic.h>
    /* Here are the memory barrier functions. Mac OS X only provides
       full memory barriers, so the three types of barriers are the same,
       however, these barriers are superior to compiler-based ones. */
#   define GVL_FULL_SYNC()  OSMemoryBarrier()
#   define GVL_READ_SYNC()  OSMemoryBarrier()
#   define GVL_WRITE_SYNC() OSMemoryBarrier()
#elif defined(__GNUC__)
    /* GCC >= 4.1 has built-in intrinsics. We'll use those */
#   if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)
#      define GVL_FULL_SYNC()  __sync_synchronize()
#      define GVL_READ_SYNC()  __sync_synchronize()
#      define GVL_WRITE_SYNC() __sync_synchronize()
    /* as a fallback, GCC understands volatile asm and "memory" to mean it
     * should not reorder memory read/writes */
    /* Note that it is not clear that any compiler actually defines __PPC__,
     * it can probably be safely removed. */
#   elif defined( __ppc__ ) || defined( __powerpc__) || defined( __PPC__ )
#      define GVL_FULL_SYNC()  asm volatile("sync":::"memory")
#      define GVL_READ_SYNC()  asm volatile("sync":::"memory")
#      define GVL_WRITE_SYNC() asm volatile("sync":::"memory")
#   elif defined( __i386__ ) || defined( __i486__ ) || defined( __i586__ ) || \
         defined( __i686__ ) || defined( __x86_64__ )
#      define GVL_FULL_SYNC()  asm volatile("mfence":::"memory")
#      define GVL_READ_SYNC()  asm volatile("lfence":::"memory")
#      define GVL_WRITE_SYNC() asm volatile("sfence":::"memory")
#   else
#      ifdef ALLOW_SMP_DANGERS
#         warning Memory barriers not defined on this system or system unknown
#         warning For SMP safety, you should fix this.
#         define GVL_FULL_SYNC()
#         define GVL_READ_SYNC()
#         define GVL_WRITE_SYNC()
#      else
#         error Memory barriers are not defined on this system. You can still compile by defining ALLOW_SMP_DANGERS, but SMP safety will not be guaranteed.
#      endif
#   endif
#elif (_MSC_VER >= 1400) && !defined(_WIN32_WCE)
#   include <intrin.h>
#   pragma intrinsic(_ReadWriteBarrier)
#   pragma intrinsic(_ReadBarrier)
#   pragma intrinsic(_WriteBarrier)
#   define GVL_FULL_SYNC()  _ReadWriteBarrier()
#   define GVL_READ_SYNC()  _ReadBarrier()
#   define GVL_WRITE_SYNC() _WriteBarrier()
#elif defined(_WIN32_WCE)
#   define GVL_FULL_SYNC()
#   define GVL_READ_SYNC()
#   define GVL_WRITE_SYNC()
#elif defined(_MSC_VER) || defined(__BORLANDC__)
#   define GVL_FULL_SYNC()  _asm { lock add    [esp], 0 }
#   define GVL_READ_SYNC()  _asm { lock add    [esp], 0 }
#   define GVL_WRITE_SYNC() _asm { lock add    [esp], 0 }
#else
#   ifdef ALLOW_SMP_DANGERS
#      warning Memory barriers not defined on this system or system unknown
#      warning For SMP safety, you should fix this.
#      define GVL_FULL_SYNC()
#      define GVL_READ_SYNC()
#      define GVL_WRITE_SYNC()
#   else
#      error Memory barriers are not defined on this system. You can still compile by defining ALLOW_SMP_DANGERS, but SMP safety will not be guaranteed.
#   endif
#endif

#endif // UUID_54B4E13DC6CC41B073E60A941244DFBB
