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
