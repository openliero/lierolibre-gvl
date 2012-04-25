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

#ifndef GVL_SERIALIZATION_CONTEXT_HPP
#define GVL_SERIALIZATION_CONTEXT_HPP

#include <map>
#include "../support/type_info.hpp"

namespace gvl
{

template<typename T>
struct new_creator
{
	template<typename Context>
	T* operator()(Context&)
	{
		return new T;
	}
};

template<typename DerivedT>
struct serialization_context
{
	struct type
	{
		type()
		: next_id(1)
		{
		}

		void add(uint32_t id, void* ptr)
		{
			id2ptr.insert(std::make_pair(id, ptr));
			ptr2id.insert(std::make_pair(ptr, id));
		}

		void remove(void* ptr)
		{
			std::map<void*, uint32_t>::iterator i1 = ptr2id.find(ptr);
			if(i1 != ptr2id.end())
			{
				std::map<uint32_t, void*>::iterator i2 = id2ptr.find(i1->second);

				ptr2id.erase(i1);
				if(i2 != id2ptr.end())
					id2ptr.erase(i2);
			}
		}

		bool try_get(uint32_t id, void*& ptr)
		{
			std::map<uint32_t, void*>::iterator i = id2ptr.find(id);

			if(i == id2ptr.end())
				return false;
			ptr = i->second;
			return true;
		}

		bool try_get(void* ptr, uint32_t& id)
		{
			std::map<void*, uint32_t>::iterator i = ptr2id.find(ptr);

			if(i == ptr2id.end())
				return false;
			id = i->second;
			return true;
		}

		int pop_id()
		{
			return next_id++;
		}

		std::map<uint32_t, void*> id2ptr;
		std::map<void*, uint32_t> ptr2id;
		uint32_t next_id;
	};

	serialization_context()
	{
	}

	DerivedT& derived()
	{ return *static_cast<DerivedT*>(this); }

	type& get_type(gvl::type_info ti)
	{
		typename std::map<gvl::type_info, type*>::iterator i = types.find(ti);
		if(i == types.end())
		{
			i = types.insert(std::make_pair(ti, new type)).first;
		}

		return *i->second;
	}

	template<typename T>
	std::pair<bool, uint32_t> write(T*& v)
	{
		if(v == 0)
			return std::make_pair(false, 0);
		type& t = get_type(gvl::type_id<T>());

		uint32_t id;
		void* ptr = v;
		if(!t.try_get(ptr, id))
		{
			id = t.pop_id();
			t.add(id, ptr);
			return std::make_pair(true, id);
		}
		else
		{
			return std::make_pair(false, id);
		}
	}

	template<typename T>
	bool read(T*& v, uint32_t id)
	{
		return read(v, id, new_creator<T>());
	}

	template<typename T, typename Creator>
	bool read(T*& v, uint32_t id, Creator creator)
	{
		if(id == 0)
		{
			v = 0;
			return false;
		}
		type& t = get_type(gvl::type_id<T>());

		void* ptr;
		if(t.try_get(id, ptr))
		{
			v = static_cast<T*>(ptr);
			return false;
		}
		else
		{
			v = creator(derived());
			ptr = v;
			t.add(id, ptr);
			return true;
		}
	}

	// Forget about object v.
	// NOTE: This must be called in the same place for the same object when serializing and deserializing!
	template<typename T>
	void unregister(T* v)
	{
		type& t = get_type(gvl::type_id<T>());
		t.remove(v);
	}

	~serialization_context()
	{
		for(typename std::map<gvl::type_info, type*>::iterator i = types.begin(); i != types.end(); ++i)
		{
			delete i->second;
		}
	}

private:
	// Non-copyable
	serialization_context(serialization_context const&);
	serialization_context& operator=(serialization_context const&);

	std::map<gvl::type_info, type*> types;
};

struct default_serialization_context : serialization_context<default_serialization_context>
{
};

} // namespace gvl

#endif // GVL_SERIALIZATION_CONTEXT_HPP
