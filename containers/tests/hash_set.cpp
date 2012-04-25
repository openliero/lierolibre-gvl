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

#include "../../test/test.hpp"

#include "../../hash_set/hash_set.hpp"

GVLTEST_SUITE(gvl, hash_set_new)
GVLTEST_SUITE(gvl, hash_set_unboxed)

GVLTEST(gvl, hash_set_new, insert_find)
{
	gvl::hash_set_new<int> h;

	for(int i = 0; i < 100; ++i)
		h.insert(i);

	for(int i = 0; i < 100; ++i)
		GVLTEST_ASSERT("exists", h.has(i) && h.lookup(i)->key() == i);
	for(int i = 100; i < 200; ++i)
		GVLTEST_ASSERT("not exists", !h.has(i));

	
}

GVLTEST(gvl, hash_set_unboxed, insert_find)
{
	gvl::hash_set_unboxed<int> h;

	// 0 will work as empty element
	for(int i = 1; i < 100; ++i)
		h.insert(i);

	for(int i = 1; i < 100; ++i)
		GVLTEST_ASSERT("exists", h.has(i) && *h.get(i) == i);
	for(int i = 100; i < 200; ++i)
		GVLTEST_ASSERT("not exists", !h.has(i));

	for(int i = 100; i < 100000; ++i)
		h.insert(i);

	for(int i = 100; i < 100000; ++i)
		GVLTEST_ASSERT("exists 2", h.has(i) && h.lookup(i)->key() == i);

	for(int i = 500; i < 5000; ++i)
		h.erase(i);

	for(int i = 100; i < 500; ++i)
		GVLTEST_ASSERT("exists 3", h.has(i));
	for(int i = 500; i < 5000; ++i)
		GVLTEST_ASSERT("not exists 2", !h.has(i));
	for(int i = 5000; i < 100000; ++i)
		GVLTEST_ASSERT("exists 4", h.has(i));
}