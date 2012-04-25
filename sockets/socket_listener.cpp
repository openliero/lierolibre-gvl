#include "socket_listener.hpp"

#include "socketstream.hpp"

namespace gvl
{

socket_listener::socket_listener()
: sock()
{
	init_sockets();
}

socket_listener::socket_listener(int port)
: sock()
{
	init_sockets();
	listen(port);
}

socket_listener::~socket_listener()
{
	sock.close();
	deinit_sockets();
}

bool socket_listener::listen(int port)
{
	sock.close();

	socket s;
	s = tcp_socket();
	if(!s.is_valid())
		return false;
	s.set_nonblocking();

	s.bind(port);

	s.listen();

	sock = s;
	return true;
}

gvl::shared_ptr<gvl::socketstream> socket_listener::accept()
{
	if(!sock.is_valid())
		return gvl::shared_ptr<gvl::socketstream>();

	while(true)
	{
		internet_addr addr;
		socket news = sock.accept(addr);

		if(news.is_valid())
			return gvl::shared_ptr<gvl::socketstream>(new socketstream(news));

		/*
		if(sock_error() != sockerr_would_block) // Error
		{
			ELOG("Error accepting on " << native_socket(sock));
			sock.close();
			return 0;
		}*/

		return gvl::shared_ptr<gvl::socketstream>();
	}
}

void socket_listener::close()
{
	if(sock.is_valid())
		sock.close();
	sock = socket();
}

}
