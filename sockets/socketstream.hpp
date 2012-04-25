#ifndef UUID_A55DFD76DE724F2E2A89CE91B9EDB4DA
#define UUID_A55DFD76DE724F2E2A89CE91B9EDB4DA

#include "sockets.hpp"
#include "fd_set.hpp"
#include "../io/stream.hpp"
#include "../support/flags.hpp"
#include <memory>
//#include <iostream>

namespace gvl
{

/*
struct socket_state : bucket_sink, flags
{

};*/

/*
struct socketstream;

struct socket_bucket : bucket
{
	socket_bucket(socketstream* state)
	: state(state)
	{
	}

	socketstream* state;
};*/

struct socketstream : stream, flags
{
	friend struct socket_bucket;

	socketstream(char const* addr, int port)
	: flags(0)
	{
		//open(new socket_bucket(this));

		connect(addr, port);
	}

	// sock_init must be connected
	socketstream(socket sock_init)
	: flags(connected)
	, sock(sock_init)
	{

	}

	/*
	socketstream(socket sck)
	: stream(new socket_bucket(sck), new socket_sink(sck))
	, sck(sck)
	{
	}
	*/

	enum socket_flags
	{
		connected = (1 << 0),
		connecting = (1 << 1),
		error_occured = (1 << 2)
	};


	void process()
	{
		if(flags::all(connecting) && flags::no(error_occured))
		{
			std::auto_ptr<select_fd_set> fdset(new_select_fd_set());

			fdset->set(sock);

			select(0, fdset.get(), 0);

			if(fdset->is_set(sock))
			{
				int status = sock.opt_error();
				if(status != 0)
				{
					flags::set(error_occured);
					return;
				}
				flags::replace(connected, connected | connecting);
			}
		}
	}

	void connect(char const* addr, int port)
	{
		init_sockets();

/*
		std::auto_ptr<host_entry> hp(resolve_host( addr ));

		if(!hp.get())
			return;
		*/

		internet_addr server(addr, port);
		if(!server.valid())
			return;

		socket s = tcp_socket();
		if(!s.is_valid())
			return;
		s.set_nonblocking();

		if(!s.connect(server))
			return;

		sock = s;
		flags::replace(connecting);
	}

	socket underlying_socket()
	{
		return sock;
	}

protected:
	/*override*/ read_result read_bucket(size_type amount = 0, bucket* dest = 0);

	/*override*/ write_result write_bucket(bucket* b)
	{
		process();

		if(flags::no(connected))
			return write_result(write_would_block, false);

		size_type size = b->size();
		int r = sock.send(b->get_ptr(), size);

		if(r == (int)size)
		{
			//std::cout << "Full write: " << size << "b" << std::endl;

			gvl::unlink(b);
			delete b;
			return write_result(write_ok, true);
		}
		else
		{
			if(r > 0)
			{
				//std::cout << "Partial write: " << r << "b" << std::endl;
				b->cut_front(r);
				return write_result(write_part, false);
			}
			else if(r == socket::would_block)
				return write_result(write_would_block, false);
		}
		return write_result(write_error, false);
	}

	/*override*/ write_status propagate_close()
	{
		if(flags::any(connected | connecting))
		{
			sock.close();
			flags::reset(connected | connecting | error_occured);
		}
		return write_ok;
	}

	socket sock;
};

} // namespace gvl

#endif // UUID_A55DFD76DE724F2E2A89CE91B9EDB4DA
