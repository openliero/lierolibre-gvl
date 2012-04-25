#ifndef UUID_52AE273D77754004EBD1CFA67FE06693
#define UUID_52AE273D77754004EBD1CFA67FE06693

#include "sockets.hpp"

namespace gvl
{

struct select_fd_set
{
	void zero();
	bool is_set(socket s);
	void set(socket s);

	virtual ~select_fd_set()
	{}

protected:
	select_fd_set() {}
	select_fd_set(select_fd_set const&);
	select_fd_set& operator=(select_fd_set const&);
};

select_fd_set* new_select_fd_set();

int select(select_fd_set* readfds, select_fd_set* writefds, select_fd_set* errorfds);

}

#endif // UUID_52AE273D77754004EBD1CFA67FE06693
