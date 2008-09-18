#include "profile.hpp"

#include <vector>
#include <map>
#include <string>
#include <ostream>
#include <iomanip>
#include "macros.hpp"

namespace gvl
{

struct profile_manager
{
	typedef std::map<int, profile_counter*> line_map_t;
	
	struct function_def
	{
		line_map_t counters;
	};
	
	static profile_manager& instance()
	{
		static profile_manager instance_;
		return instance_;
	}
	
	void register_counter(profile_counter* c)
	{
		function_map[c->func].counters[c->line] = c;
	}
	
	void present(std::ostream& str);
	
	typedef std::map<std::string, function_def> function_map_t;
	
	std::vector<profile_counter*> counters;
	function_map_t function_map;
};

void present_profile(std::ostream& str)
{
	profile_manager::instance().present(str);
}

profile_counter::profile_counter(char const* desc, char const* func, int line)
: count(0), desc(desc), func(func), line(line)
{
	profile_manager::instance().register_counter(this);
}

void profile_manager::present(std::ostream& str)
{
	FOREACH(function_map_t, f, function_map)
	{
		str << "== Function " << f->first << "\n";
		str << "Count Description\n";
		FOREACH(line_map_t, l, f->second.counters)
		{
			str << std::setw(5) << l->second->count << " " << l->second->desc << ", line " << l->first << "\n";
		}
		str << "=======\n\n";
	}
}

} // namespace gvl
