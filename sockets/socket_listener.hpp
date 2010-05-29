#ifndef UUID_5EA45A24CF29403A5EA528B39D964A3A
#define UUID_5EA45A24CF29403A5EA528B39D964A3A

#include "sockets.hpp"

namespace gvl
{

struct socketstream;

struct socket_listener
{
	socket_listener();	
	socket_listener(int port);
	~socket_listener();
	
	bool listen(int port);
	gvl::shared_ptr<gvl::socketstream> accept();
	
	void close();
	
	socket sock;
};

}

#endif // UUID_5EA45A24CF29403A5EA528B39D964A3A
