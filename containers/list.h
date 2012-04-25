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

#ifndef UUID_BFFA58059B0C49630DD657937FF53E6C
#define UUID_BFFA58059B0C49630DD657937FF53E6C

#include "../support/platform.hpp"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gvl_list_node_
{
	struct gvl_list_node_* next;
	struct gvl_list_node_* prev;
} gvl_list_node;

typedef struct gvl_list_
{
	gvl_list_node sentinel;
} gvl_list;

GVL_INLINE void gvl_list_link_after(gvl_list_node* self, gvl_list_node* new_node)
{
	gvl_list_node* old_self_next = self->next;

	new_node->next = old_self_next;
	new_node->prev = self;
	old_self_next->prev = new_node;
	self->next = new_node;
}

GVL_INLINE void gvl_list_link_before(gvl_list_node* self, gvl_list_node* new_node)
{
	gvl_list_node* old_self_prev = self->prev;

	new_node->next = self;
	new_node->prev = old_self_prev;
	old_self_prev->next = new_node;
	self->prev = new_node;
}

GVL_INLINE void gvl_list_unlink(gvl_list_node* self)
{
	gvl_list_node* self_next = self->next;
	gvl_list_node* self_prev = self->prev;

	self_prev->next = self_next;
	self_next->prev = self_prev;
}

#define gvl_list_first(self) ((self)->sentinel.next)
#define gvl_list_last (self) ((self)->sentinel.prev)

#ifdef __cplusplus
}
#endif

#endif // UUID_BFFA58059B0C49630DD657937FF53E6C
