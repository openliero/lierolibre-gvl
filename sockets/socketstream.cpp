#include "socketstream.hpp"

namespace gvl
{

socket_bucket::read_result socket_bucket::read(size_type amount, bucket* dest)
{
	state->process();
	
	if(state->flags::no(socketstream::connected))
		return read_result(bucket_source::blocking);
		
	char buffer[1024];
	int r = state->sock.recv(buffer, 1024);
	if(r > 0)
		return read_result(bucket_source::ok, new bucket_mem(buffer, r));
	else if(r == 0)
		return read_result(bucket_source::eos);
	else if(r == socket::would_block)
		return read_result(bucket_source::blocking);
	return read_result(bucket_source::error);
}

} // namespace gvl
