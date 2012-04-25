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

#include "profile.hpp"

#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <ostream>
#include <iomanip>
#include <sstream>
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

void format_time(std::ostream& out, double seconds)
{
	std::ostringstream ss;
	if(seconds < 1.0)
		ss << (1000.0 * seconds) << "ms";
	else if(seconds < 60.0)
		ss << int(seconds) << "s " << int(std::fmod(seconds, 1)*1000.0) << "ms";
	else if(seconds < 60.0*60.0)
		ss << int(seconds / 60.0) << "m " << int(std::fmod(seconds, 60)) << "s";
	else
		ss << int(seconds / 3600.0) << "h " << int(std::fmod(seconds, 3600.0) / 60.0) << "m " << int(std::fmod(seconds, 60)) << "s";
	out << ss.str();
}

void profile_manager::present(std::ostream& str)
{
	FOREACH(function_map_t, f, function_map)
	{
		str << "== Function " << f->first << "\n";
		if(!f->second.counters.empty())
		{
			str << "Count Description\n";
			FOREACH(counter_line_map_t, l, f->second.counters)
			{
				str << std::setw(5) << l->second->count << " " << l->second->desc << ", line " << l->first << "\n";
			}
		}
		str << "      Time Description\n";
		FOREACH(timer_line_map_t, l, f->second.timers)
		{
			if(l->second->count > 0)
			{
				double time = l->second->total_time / 1000.0;
				str << std::setw(10);
				
				format_time(str, time);
				
				str << " " << l->second->desc; // << ":" << l->first;
				if(l->second->count > 1)
				{
					str << " (average time: ";
					format_time(str, (time / l->second->count));
					str << ")";
				}
				str << "\n";
			}
			else
			{
				str << l->second->desc << " not hit\n";
			}
		}
		str << "=======\n\n";
	}
}



} // namespace gvl
