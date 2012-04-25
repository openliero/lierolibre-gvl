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

#ifndef UUID_1F5ADE6AD4944C662DFB9D86C2563186
#define UUID_1F5ADE6AD4944C662DFB9D86C2563186

#include <map>
#include <string>
#include "debug.hpp"

namespace gvl
{

struct string_pool_state
{
	string_pool_state()
	: string_pool_next_id(0)
	{
	}

	std::map<int, std::string> string_pool_id_to_str;
	std::map<std::string, int> string_pool_str_to_id;
	std::map<char const*, int> string_pool_strconst_to_id;
	int string_pool_next_id;

	static string_pool_state& instance()
	{
		static string_pool_state self;
		return self;
	}
};

struct pooled_string
{
	// NOTE! Assumes str is alive throughout the whole program
	pooled_string(char const* str)
	{
		string_pool_state& state = string_pool_state::instance();

		std::map<char const*, int>::iterator i = state.string_pool_strconst_to_id.find(str);
		if(i != state.string_pool_strconst_to_id.end())
		{
			id = i->second;
		}
		else
		{
			init(str);
			state.string_pool_strconst_to_id[str] = id;
		}
	}

	pooled_string(std::string const& str)
	{
		init(str);
	}

	void init(std::string const& str)
	{
		string_pool_state& state = string_pool_state::instance();

		std::map<std::string, int>::iterator i = state.string_pool_str_to_id.find(str);
		if(i != state.string_pool_str_to_id.end())
		{
			id = i->second;
		}
		else
		{
			id = state.string_pool_next_id++;
			state.string_pool_id_to_str[id] = str;
			state.string_pool_str_to_id[str] = id;
		}
	}

	std::string const& get()
	{
		string_pool_state& state = string_pool_state::instance();

		std::map<int, std::string>::iterator i = state.string_pool_id_to_str.find(id);
		sassert(i != state.string_pool_id_to_str.end());
		return i->second;
	}

	bool operator==(pooled_string b) const
	{ return id == b.id; }

	bool operator!=(pooled_string b) const
	{ return id != b.id; }

	bool operator<(pooled_string b) const
	{ return id < b.id;	}

private:
	int id;
};

}

#endif // UUID_1F5ADE6AD4944C662DFB9D86C2563186
