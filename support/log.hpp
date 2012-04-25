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

#ifndef UUID_D58C3D2694ED41DD5D5AEA8CEECD46B2
#define UUID_D58C3D2694ED41DD5D5AEA8CEECD46B2

//#include <iostream>
//#include <ostream>
#include "../io/iostream.hpp"
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

gvl::octet_stream_writer& get_named_stream(char const* name, char const* path);

#define LOG_NONE -1
#define LOG_ERRORS 0
#define LOG_INFO 1
#define LOG_WARNINGS 2
#define LOG_TRACE 3

#define LOG(x_) (gvl::cout() << x_ << gvl::endl)

#define FLOG(f_, x_) (gvl::get_named_stream(#f_, #f_ ".log") << x_ << std::endl)

#define WLOG_ONCE(x_) do { static bool warned = false; if(!warned) { WLOG(x_); warned = true; } } while(0)

#ifdef LOG_RUNTIME
# define DLOG(x_)	if(global_log_options.debug) { (gvl::cout() << __FILE__ ":" << __LINE__ << ": " << x_ << gvl::endl); } else (void)0
# define DLOGL(l_, x_) if(global_log_options.debug) { l_.print(x_); } else (void)0
# define TLOG(x_) if(global_log_options.level >= LOG_TRACE) { (gvl::cout() << __FILE__ ":" << __LINE__ << ": " << x_ << gvl::endl); } else (void)0
# define WLOG(x_) if(global_log_options.level >= LOG_WARNINGS) { (gvl::cerr() << __FILE__ ":" << __LINE__ << ": " << x_ << gvl::endl); } else (void)0
# define ILOG(x_) if(global_log_options.level >= LOG_INFO) { (gvl::cerr() << x_ << gvl::endl); } else (void)0
# define ELOG(x_) if(global_log_options.level >= LOG_ERRORS) { (gvl::cerr() << __FILE__ ":" << __LINE__ << ": " << x_ << gvl::endl); } else (void)0
#else
# ifndef LOG_LEVEL
#  define LOG_LEVEL LOG_WARNINGS
# endif
# ifdef LOG_DEBUG
#  define DLOG(x_) (gvl::cout() << __FILE__ ":" << __LINE__ << ": " << x_ << gvl::endl)
#  define DLOGL(l_, x_) l_.print(x_)
# else
#  define DLOG(x_) (void)0
#  define DLOGL(l_, x_) (void)0
# endif
# if LOG_LEVEL >= LOG_TRACE
#  define TLOG(x_) (gvl::cout() << __FILE__ ":" << __LINE__ << ": " << x_ << gvl::endl)
# else
#  define TLOG(x_) (void)0
# endif
# if LOG_LEVEL >= LOG_INFO
#  define ILOG(x_) (gvl::cout() << x_ << gvl::endl)
# else
#  define ILOG(x_) (void)0
# endif
# if LOG_LEVEL >= LOG_WARNINGS
#  define WLOG(x_) (gvl::cerr() << __FILE__ ":" << __LINE__ << ": " << x_ << gvl::endl)
# else
#  define WLOG(x_) (void)0
# endif
# if LOG_LEVEL >= LOG_ERRORS
#  define ELOG(x_) (gvl::cerr() << __FILE__ ":" << __LINE__ << ": " << x_ << gvl::endl)
# else
#  define ELOG(x_) (void)0
# endif
#endif

} // namespace gvl

#endif // UUID_D58C3D2694ED41DD5D5AEA8CEECD46B2
