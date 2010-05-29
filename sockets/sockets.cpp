#include "sockets.hpp"
#include "sockets_sys_impl.hpp"
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace gvl
{

socket socket::invalid()
{
	return socket();
}

sockaddr_in* get_sockaddr_in(internet_addr& self)
{
	return reinterpret_cast<sockaddr_in*>(&self.storage_);
}

sockaddr_in const* get_sockaddr_in(internet_addr const& self)
{
	return reinterpret_cast<sockaddr_in const*>(&self.storage_);
}

sockaddr* get_sockaddr(internet_addr& self)
{
	return reinterpret_cast<sockaddr*>(&self.storage_);
}

void clear_initialized(internet_addr& self)
{
	memset(&self.storage_, 0, sizeof(self.storage_));
}

#if GVL_WIN32 || GVL_WIN64

inline int error() { return WSAGetLastError(); }
int const sckerr_in_progress = WSAEINPROGRESS;
int const sckerr_would_block = WSAEWOULDBLOCK;
int const sckerr_conn_reset = WSAECONNRESET;

socket::socket()
: _voidp((void*)INVALID_SOCKET)
{

}

bool socket::is_valid()
{
	return native_socket(*this) != INVALID_SOCKET;
}

void socket::close()
{
	if(is_valid())
	{
		closesocket(native_socket(*this));
	}
}

int const error_ret = SOCKET_ERROR;

#else

inline int error() { return errno; }
int const sckerr_in_progress = EINPROGRESS;
int const sckerr_would_block = EWOULDBLOCK;
int const sckerr_conn_reset = ECONNRESET;

socket invalid_socket()
{
	socket s;
	s._int = -1;
	return s;
}

socket::socket()
: _int(-1)
{

}

bool socket::is_valid()
{
	return native_socket(*this) != -1;
}

void socket::close()
{
	if(is_valid())
	{
		::close(native_socket(*this));
	}
}

int const error_ret = -1;

#endif



#if 0
static char* copy_string(char const* p)
{
	size_t len = std::strlen(p) + 1;
	char* s = new char[len];
	std::memcpy(s, p, len);
	return s;
}

static char** copy_list(char** p)
{
	int i;
	int l = 0;
	char** n;
	for(; p[l]; ++l)
		/* nothing */;
	
	n = new char*[l + 1];
	for(i = 0; i < l; ++i)
	{
		n[i] = copy_string(p[i]);
	}
	
	n[l] = 0;
	
	return n;
}

static char** copy_list_l(char** p, size_t len)
{
	int i;
	int l = 0;
	char** n;
	for(; p[l]; ++l)
		/* nothing */;
	
	n = new char*[l + 1];
	for(i = 0; i < l; ++i)
	{
		n[i] = new char[len];
		std::memcpy(n[i], p[i], len);
	}
	
	n[l] = 0;
	
	return n;
}

static void free_list(char** p)
{
	int i;
	for(i = 0; p[i] != NULL; ++i)
		delete [] p[i];
	delete [] p;
}

struct host_entry_storage : host_entry_impl
{
	hostent v;
	
	~host_entry_storage();
	
	void* storage()
	{ return &v; }
};

static host_entry_storage* create_host_entry(hostent const* p)
{
	host_entry_storage* self = new host_entry_storage;
	self->v.h_name = copy_string(p->h_name);
	self->v.h_aliases = copy_list(p->h_aliases);
	self->v.h_addrtype = p->h_addrtype;
	self->v.h_length = p->h_length;
	self->v.h_addr_list = copy_list_l(p->h_addr_list, p->h_length);
	return self;
}

host_entry_storage::~host_entry_storage()
{
	delete [] v.h_name;
	free_list(v.h_aliases);
	free_list(v.h_addr_list);
}

host_entry* resolve_host(char const* name)
{
	hostent* p = gethostbyname( name ); // TODO: This is deprecated IIRC
	
	if(!p)
		return 0; // ERROR
	
	return create_host_entry(p);
}

host_entry::host_entry(char const* name)
: ptr(resolve_host(name))
{
}
#endif

socket tcp_socket()
{
	socket s = make_socket(::socket(PF_INET, SOCK_STREAM, 0));

	if(!s.is_valid())
		return s;

	s.set_nonblocking();
	return s;
}

socket udp_socket()
{
	socket s = make_socket(::socket(PF_INET, SOCK_DGRAM, 0));

	if(!s.is_valid())
		return s;

	s.set_nonblocking();
	return s;
}

void socket::set_nonblocking(bool no_blocking)
{
#if GVL_WIN32==1
	unsigned long no_blocking_int = 1;
	ioctlsocket(native_socket(*this), FIONBIO, &no_blocking_int);
#else
	fcntl(native_socket(*this), F_SETFL, no_blocking ? O_NONBLOCK : 0);
#endif
}

int socket::set_nodelay(bool no_delay)
{
	char no_delay_int = no_delay;
#if !defined(BEOS_NET_SERVER)
	return setsockopt(
		native_socket(*this),
		IPPROTO_TCP,
		TCP_NODELAY,
		reinterpret_cast<char*>(&no_delay_int),
		sizeof(no_delay_int)) == 0;
#else
	return 1;
#endif
}

int socket::bind(int port)
{
	sockaddr_in addr;
	int ret;
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)port);
	addr.sin_addr.s_addr = INADDR_ANY;
	std::memset(&(addr.sin_zero), '\0', 8);

	ret = ::bind(native_socket(*this), reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr_in)); // TODO: Some way to get size from addr
	return (ret != error_ret);
}

int socket::listen()
{
	int ret = ::listen(native_socket(*this), 5);
	return (ret != error_ret);
}

socket socket::accept(internet_addr& addr)
{
	socklen_t sin_size = sizeof(sockaddr_in);

	// TODO: Check for errors
	return make_socket(::accept(native_socket(*this), get_sockaddr(addr), &sin_size));
}

int socket::connect(internet_addr& addr)
{
	int r = ::connect(native_socket(*this), get_sockaddr(addr), sizeof(sockaddr_in)); // TODO: Some way to get size from addr
	
	if(r == error_ret)
	{
		int err = error();
		
		#if GVL_WIN32==1
			if(err != sckerr_would_block)
		#else
			if(err != sckerr_in_progress)
		#endif
				return 0; // ERROR
	}
		
	return 1;
}

int translate_comm_ret(int ret)
{
	if(ret == 0)
	{
		return socket::disconnected;
	}
	else if(ret == error_ret || ret < 0)
	{
		int err = error();
		
#if 0
		if(err != sckerr_would_block)
			printf("Sockerr: %d\n", err);
#endif
		switch(err)
		{
		case sckerr_conn_reset: return socket::conn_reset;
		case sckerr_would_block: return socket::would_block;
		default: return socket::failure;
		}
	}
	
	return ret;
}

int socket::send(void const* msg, size_t len)
{
	int ret = ::send(native_socket(*this), reinterpret_cast<char const*>(msg), (int)len, 0);
	
	return translate_comm_ret(ret);
}

int socket::recv(void* msg, size_t len)
{
	int ret = ::recv(native_socket(*this), reinterpret_cast<char*>(msg), (int)len, 0);
	
	return translate_comm_ret(ret);
}

int socket::sendto(void const* msg, size_t len, internet_addr& dest)
{
	int ret = ::sendto(
		native_socket(*this),
		reinterpret_cast<char const*>(msg),
		(int)len, 0,
		get_sockaddr(dest),
		sizeof(sockaddr));
	
	return translate_comm_ret(ret);
}

int socket::recvfrom(void* msg, size_t len, internet_addr& src)
{
	socklen_t fromlen = sizeof(sockaddr);
	int ret = ::recvfrom(
		native_socket(*this),
		reinterpret_cast<char*>(msg),
		(int)len, 0,
		get_sockaddr(src),
		&fromlen);
	
	return translate_comm_ret(ret);
}

int socket::opt_error()
{
	int status;
	socklen_t len = sizeof(status);
	getsockopt(native_socket(*this), SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&status), &len);
	return status;
}

/*
typedef uint64_t sckimpl_sa_align_t;

std::size_t const sckimpl_sa_maxsize = 32; // IPv6 needs 28 bytes
*/

int internet_addr::port() const
{
	sockaddr_in const* s = get_sockaddr_in(*this);
	return ntohs(s->sin_port);
}

uint32_t internet_addr::ip() const
{
	sockaddr_in const* s = get_sockaddr_in(*this);
#if GVL_WIN32 || GVL_WIN64
	return ntohl(s->sin_addr.S_un.S_addr);
#else
	return ntohl(s->sin_addr.s_addr);
#endif
}

internet_addr::internet_addr(uint32_t addr, int port)
{
	clear_initialized(*this);

	sockaddr_in* s = get_sockaddr_in(*this);
	
	s->sin_family = AF_INET;
	s->sin_port = htons( (u_short)port );
	s->sin_addr.s_addr = htonl( addr );
}

internet_addr::internet_addr(char const* name, int port)
{
	clear_initialized(*this);

	hostent* p = gethostbyname(name);
	if(p)
	{
		sockaddr_in* s = get_sockaddr_in(*this);
		
		memmove(&s->sin_addr, p->h_addr_list[0], p->h_length);
		s->sin_family = p->h_addrtype;
		s->sin_port = htons( (u_short)port );
	}
}

internet_addr::internet_addr(socket s)
{
	clear_initialized(*this);

	sockaddr_in addr;
	socklen_t t = sizeof(sockaddr_in);
	
	if(getsockname(native_socket(s), reinterpret_cast<sockaddr*>(&addr), &t) != error_ret)
	{
		*get_sockaddr_in(*this) = addr;
	}
}

internet_addr::internet_addr()
{
	clear_initialized(*this);

	sockaddr_in* s = get_sockaddr_in(*this);
	
	s->sin_family = AF_INET;
	s->sin_port = htons( 0 );
	s->sin_addr.s_addr = htonl( INADDR_ANY );
}

void internet_addr::reset()
{
	clear_initialized(*this);
}

bool internet_addr::valid()
{
	return get_sockaddr(*this)->sa_family != 0;
}

bool operator==(internet_addr const& a, internet_addr const& b)
{
	return 0 == std::memcmp(&a.storage_, &b.storage_, sizeof(a.storage_));
}

void internet_addr::port(int port)
{
	sockaddr_in* s = get_sockaddr_in(*this);
	s->sin_port = htons( (u_short)port );
}

void internet_addr::ip(uint32_t addr)
{
	sockaddr_in* s = get_sockaddr_in(*this);
	s->sin_addr.s_addr = htonl( addr );
}


#if GVL_WIN32==1

static int winsock_ref = 0; // TODO: NOTE: Not thread-safe

void init_sockets()
{
	if(++winsock_ref == 1)
	{
		WSADATA wsaData;
		int res;

		res = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (res != 0)
		{
			return;
		}
	}
}

void deinit_sockets()
{
	if(--winsock_ref == 0)
	{
		WSACleanup();
	}
}

#endif

} // namespace gvl



