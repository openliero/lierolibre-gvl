#include "socketstream.hpp"

namespace gvl
{

stream::read_result socketstream::read(size_type amount, bucket* dest)
{
	process();
	
	if(flags::no(socketstream::connected))
		return read_result(read_blocking);
		
	char buffer[1024];
	int r = sock.recv(buffer, 1024);
	if(r > 0)
		return read_result(read_ok, new bucket(buffer, r));
	else if(r == 0)
		return read_result(read_eos);
	else if(r == socket::would_block)
		return read_result(read_blocking);
	return read_result(read_error);
}

} // namespace gvl
