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
