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

#include "log.hpp"

#include <cstring>
#include <fstream>

namespace
{

struct cstr_comp
{
	bool operator()(char const* a, char const* b) const
	{
		return strcmp(a, b) < 0;
	}
};


struct log_streams
{
	typedef std::map<char const*, gvl::octet_stream_writer*, cstr_comp> stream_map;

	log_streams()
	{
	}

	~log_streams()
	{
		FOREACH(stream_map, i, streams)
		{
			delete i->second;
		}
	}

	gvl::octet_stream_writer& get(char const* name, char const* path)
	{
		stream_map::iterator i = streams.find(name);
		if(i == streams.end())
		{
			gvl::octet_stream_writer* str = new gvl::octet_stream_writer(
				gvl::stream_ptr(new gvl::fstream(path, "wb")));
			streams[name] = str;
			return *str;
		}
		else
			return *i->second;
	}

	static log_streams& instance()
	{
		static log_streams inst;
		return inst;
	}

	stream_map streams;
};

} // namespace

namespace gvl
{

log_options global_log_options;

log_options::log_options()
: debug(true), level(LOG_WARNINGS)
{
}

void location::print(std::string const& msg) const
{
	if(file)
		gvl::cerr() << *file << ':' << line << ": " << msg << gvl::endl;
}

gvl::octet_stream_writer& get_named_stream(char const* name, char const* path)
{
	return log_streams::instance().get(name, path);
}

}
