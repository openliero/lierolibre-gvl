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
