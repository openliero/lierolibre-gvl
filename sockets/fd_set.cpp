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

#include "fd_set.hpp"

#include "sockets_sys_impl.hpp"

namespace gvl
{

#define SELF (static_cast<select_fd_set_impl&>(*this))

struct select_fd_set_impl : select_fd_set
{
	select_fd_set_impl()
	{
		zero();
	}
	
	::fd_set impl;
};

select_fd_set* new_select_fd_set()
{
	return new select_fd_set_impl;
}

void select_fd_set::zero()
{
	FD_ZERO(&SELF.impl);
}

void select_fd_set::set(socket s)
{
	FD_SET(native_socket(s), &SELF.impl);
}

bool select_fd_set::is_set(socket s)
{
	return FD_ISSET(native_socket(s), &SELF.impl) != 0;
}

#undef SELF

inline fd_set* to_impl(select_fd_set* p)
{
	return !p ? 0 : &static_cast<select_fd_set_impl*>(p)->impl;
}

int select(select_fd_set* readfds, select_fd_set* writefds, select_fd_set* errorfds)
{
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

#if GVL_WIN32==1
	return ::select(0,
		to_impl(readfds),
		to_impl(writefds),
		to_impl(errorfds),
		&tv);
#else
	return ::select(FD_SETSIZE,
		to_impl(readfds),
		to_impl(writefds),
		to_impl(errorfds),
		&tv);
#endif
}

} // namespace gvl
