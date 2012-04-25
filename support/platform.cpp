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
	
	// Do this last since it may crash the process
#if GVL_UNALIGNED_ACCESS
	uint8_t volatile x[150] = {};
	
	for(int i = 0; i < 100; ++i)
		*(uint32_t volatile*)(x + i) = 1;

	uint32_t sum = 0;
	for(int i = 0; i < 100; ++i)
		sum += *(uint32_t volatile*)(x + i);
#endif
}
