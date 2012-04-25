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

#ifndef GVL_LINKED_HASH_SET_HPP
#define GVL_LINKED_HASH_SET_HPP

#include "../list.hpp"
#include "../support/functional.hpp"
#include "../support/hash.hpp"

namespace gvl
{

template<typename Tag>
struct hash_head
{
	hash_head()
	: next(0)
	{
	}

	hash_head<Tag>* next;
}

template<typename Tag>
struct hash_node : hash_head<Tag>
{
	hash_head<Tag>* prev;
	hash_head<Tag>* next;
};

template<typename SubTag = void>
struct linked_hash_list_tag;

template<typename SubTag = void>
struct linked_hash_insert_order_tag;

template<bool InsertOrder, typename Tag = void>
struct linked_hash_set_node
{
};

template<typename T, typename Tag>
struct linked_hash_set_node<true, Tag>
	: hash_node<T, linked_hash_tag<Tag> >
	, list_node<linked_hash_insert_order_tag<Tag> >
{
};

template<typename Tag>
struct linked_hash_set_node<false, Tag>
	: hash_node<T, linked_hash_tag<Tag> >
{
};

template<typename T, bool InsertOrder = true, typename Tag = void>
struct linked_hash_set_common
{

};

template<typename T, typename Tag>
struct linked_hash_set_common<T, true, Tag>
{
	void common_unlink(T* x)
	{
		insert_order_list.unlink(x);
	}

	void common_insert(T* x)
	{
		insert_order_list.push_back(x);
	}

	list<T, insert_order_tag, dummy_delete> insert_order_list;
};

template<typename T, typename Tag>
struct linked_hash_set_common<T, true, Tag>
{
	void common_unlink(T*)
	{
	}

	void common_insert(T*)
	{
	}
};

template<typename T, typename Hash = hash_functor, bool InsertOrder = true, typename Tag = void>
struct linked_hash_set : linked_hash_set_common<T, InsertOrder, Tag>, Hash
{
	linked_hash_set()
	: tshift(3)
	, tsize(1 << tshift)
	, tmask(tsize - 1)
	, table(tsize)
	{
	}

	static std::size_t index_from_hash_(std::size_t h, std::size_t tshift, std::size_t tmask)
	{
		return (h ^ (h >> (32-tshift))) & tmask;
	}

	void insert(T* x)
	{
		std::size_t idx = index_from_hash_(Hash::operator()(*x), tshift, tmask);
		hash_head<Tag>& head = table[tmask];
		insert_into_bucket_(head, x);

		this->common_insert(x);
	}

	void unlink(T* x)
	{
		hash_node<Tag>* node = upcast_(x);
		node->prev->next = node->next;
		hash_head<Tag>* next = node->next;
		if(next)
			downcast_(next)->prev = node->prev;
	}

	static T* next_of_(hash_head<Tag> x)
	{
		return static_cast<T*>(x.next);
	}

	static T* next_of_(T* x)
	{
		return static_cast<T*>(x->template hash_node<Tag>::next);
	}

	static T* prev_of_(T* x)
	{
		return static_cast<T*>(x->template hash_node<Tag>::prev);
	}

	static hash_node<Tag>* upcast_(T* x)
	{
		return static_cast<hash_node<T, Tag>*>(x);
	}

	static hash_node<Tag>* downcast_(hash_head<Tag>* x)
	{
		return static_cast<T*>(x);
	}

	void rehash_(std::size_t tshift_new)
	{
		std::vector<hash_head<Tag> > table_new(tsize_new);
		std::size_t tsize_new = (1 << tshift_new);
		std::size_t tmask_new = tsize_new - 1;

		for(std::size_t i = 0; i < tsize; ++i)
		{
			T* cur = next_of_(table[i]);

			while(cur)
			{
				T* next = next_of_(cur);
				std::size_t idx = index_from_hash_(Hash::operator()(*cur), tshift_new, tmask_new);

				insert_into_bucket_(table_new[idx], cur);

				cur = next;
			}
		}

		table.swap(table_new);
		tshift = tshift_new;
		tsize = tsize_new;
		tmask = tmask_new;
	}

	void insert_into_bucket_(hash_head<Tag>& bucket, T* x)
	{
		hash_node<Tag>* node = upcast_(x);
		node->next = bucket.next;
		node->prev = &bucket;
		bucket.next = node;
	}

	std::size_t tshift;
	std::size_t tsize;
	std::size_t tmask;
	std::vector<hash_head<Tag> > table;
};

}

#endif // GVL_LINKED_HASH_SET_HPP
