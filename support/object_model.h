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

#ifndef UUID_D13E73786A7C4F75E2481BB7E79A5B37
#define UUID_D13E73786A7C4F75E2481BB7E79A5B37

#define GVL_INTERFACE(name, members) \
typedef struct name##_vtable_ members name##_vtable; \
typedef struct name##_ { void* ptr; name##_vtable* vtable; }; \
INLINE name make_##name(void* ptr, name##_vtable* vtable) { \
	name ret; \
	ret.ptr = ptr; \
	ret.vtable = vtable; \
	return ret; }

#define GVL_INTERFACE1(name, member) \
typedef struct name##_ { void* ptr; member; };

#define GVL_DECL_INTERFACE(interf, vtable_name) \
extern interf##_table vtable_name;

#define GVL_DEF_INTERFACE(interf, vtable_name, vtable_funcs) \
interf##_table vtable_name = vtable_funcs;

#define GVL_REF_INTERFACE(self, member) ((self)->vtable.member)
#define GVL_REF_INTERFACE1(self, member) ((self)->member)


GVL_INTERFACE(printable, { void (*print)(void*); })

GVL_DECL_INTERFACE(printable, printable_int)

void print_int(void* self)
{
	printf("%d", (int)self);
}

GVL_DEF_INTERFACE(printable, printable_int, { print_int })

#define PRINTABLE_FROM_INT(i) make_printable((void*)(i), printable_int)

void test()
{
	int i = 10;
	printable ip = PRINTABLE_FROM_INT(i);
	GVL_REF_INTERFACE(ip, print)(ip.ptr);
}

#endif // UUID_D13E73786A7C4F75E2481BB7E79A5B37
