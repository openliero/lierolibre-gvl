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
	typedef std::map<int, profile_counter*> counter_line_map_t;
	typedef std::map<int, profile_timer*> timer_line_map_t;
	
	struct function_def
	{
		counter_line_map_t counters;
		timer_line_map_t timers;
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
	
	void register_timer(profile_timer* c)
	{
		function_map[c->func].timers[c->line] = c;
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

profile_timer::profile_timer(char const* desc, char const* func, int line)
: total_time(0), desc(desc), func(func), line(line), count(0)
{
	profile_manager::instance().register_timer(this);
}

void profile_manager::present(std::ostream& str)
{
	FOREACH(function_map_t, f, function_map)
	{
		str << "== Function " << f->first << "\n";
		str << "Count Description\n";
		FOREACH(counter_line_map_t, l, f->second.counters)
		{
			str << std::setw(5) << l->second->count << " " << l->second->desc << ", line " << l->first << "\n";
		}
		str << "Timer Description\n";
		FOREACH(timer_line_map_t, l, f->second.timers)
		{
			double time = l->second->total_time / 1000.0;
			str << std::setw(5) << time << " s " << l->second->desc << ", line " << l->first << " (average time: " << (time / l->second->count) * 1000.0 << " ms)\n";
		}
		str << "=======\n\n";
	}
}

} // namespace gvl
