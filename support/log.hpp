#ifndef UUID_D58C3D2694ED41DD5D5AEA8CEECD46B2
#define UUID_D58C3D2694ED41DD5D5AEA8CEECD46B2

#include <iostream>
#include <ostream>
#include <string>
#include <map>
#include "macros.hpp"

namespace gvl
{

struct location
{
	location()
	: file(0)
	{
	}
	
	location(std::string const& file_, int line_)
	: file(&file_), line(line_)
	{
	}
	
	// Compiler generated op= and copy ctor
		
	void print(std::string const& msg) const;
	
private:
	std::string const* file;
	int line;
};

struct log_options
{
	log_options();
	
	bool debug;
	int  level;
};

extern log_options global_log_options;

std::ostream& get_named_stream(char const* name, char const* path);

#define LOG_NONE -1
#define LOG_ERRORS 0
#define LOG_INFO 1
#define LOG_WARNINGS 2
#define LOG_TRACE 3

#define LOG(x_) (std::cout << x_ << std::endl)

#define FLOG(f_, x_) (gvl::get_named_stream(#f_, #f_ ".log") << x_ << std::endl)

#define WLOG_ONCE(x_) do { static bool warned = false; if(!warned) { WLOG(x_); warned = true; } } while(0)

#ifdef LOG_RUNTIME
#	define DLOG(x_)	if(global_log_options.debug) { (std::cout << __FILE__ ":" << __LINE__ << ": " << x_ << std::endl); } else (void)0
#	define DLOGL(l_, x_) if(global_log_options.debug) { l_.print(x_); } else (void)0
#	define TLOG(x_) if(global_log_options.level >= LOG_TRACE) { (std::cout << __FILE__ ":" << __LINE__ << ": " << x_ << std::endl); } else (void)0
#	define WLOG(x_) if(global_log_options.level >= LOG_WARNINGS) { (std::cerr << __FILE__ ":" << __LINE__ << ": " << x_ << '\n'); } else (void)0
#	define ILOG(x_) if(global_log_options.level >= LOG_INFO) { (std::cerr << x_ << '\n'); } else (void)0
#	define ELOG(x_) if(global_log_options.level >= LOG_ERRORS) { (std::cerr << __FILE__ ":" << __LINE__ << ": " << x_ << '\n'); } else (void)0
#else
#	ifndef LOG_LEVEL
#		define LOG_LEVEL LOG_WARNINGS
#	endif
#	ifdef LOG_DEBUG
#		define DLOG(x_) (std::cout << __FILE__ ":" << __LINE__ << ": " << x_ << std::endl)
#		define DLOGL(l_, x_) l_.print(x_)
#	else
#		define DLOG(x_) (void)0
#		define DLOGL(l_, x_) (void)0
#	endif
#	if LOG_LEVEL >= LOG_TRACE
#		define TLOG(x_) (std::cout << __FILE__ ":" << __LINE__ << ": " << x_ << std::endl)
#	else
#		define TLOG(x_) (void)0
#	endif
#	if LOG_LEVEL >= LOG_INFO
#		define ILOG(x_) (std::cout << x_ << std::endl)
#	else
#		define ILOG(x_) (void)0
#	endif
#	if LOG_LEVEL >= LOG_WARNINGS
#		define WLOG(x_) (std::cerr << __FILE__ ":" << __LINE__ << ": " << x_ << '\n')
#	else
#		define WLOG(x_) (void)0
#	endif
#	if LOG_LEVEL >= LOG_ERRORS
#		define ELOG(x_) (std::cerr << __FILE__ ":" << __LINE__ << ": " << x_ << '\n')
#	else
#		define ELOG(x_) (void)0
#	endif
#endif

} // namespace gvl

#endif // UUID_D58C3D2694ED41DD5D5AEA8CEECD46B2
