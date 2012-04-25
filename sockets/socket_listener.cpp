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
