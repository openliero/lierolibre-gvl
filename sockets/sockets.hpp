#ifndef UUID_2E6087A4B6164BFF4F47018450AFE807
#define UUID_2E6087A4B6164BFF4F47018450AFE807

#include <cstddef>
#include "../support/cstdint.hpp"
#include "../support/platform.hpp"
#include "../resman/shared_ptr.hpp"

namespace gvl
{

//std::size_t const sockaddr_in_size = 16;

struct internet_addr;
struct host_entry;

struct socket
{
	static int const failure = -1;
	static int const would_block = -2;
	static int const conn_reset = -3;
	static int const disconnected = -4;
	
	union 
	{
		int   _int;
		void* _voidp;
	};
	
	static socket invalid();
	
	socket();
	
	bool is_valid();
	void close();
	
	void        set_nonblocking();
	int         set_nodelay(int no_delay);
	int         bind(int port);
	int         listen();
	socket      accept(internet_addr& addr);
	int         connect(internet_addr& addr);
	
	/* Communication */
	int send(void const* msg, std::size_t len);
	int recv(void* msg, std::size_t len);
	int sendto(void const* msg, std::size_t len, internet_addr& dest);
	int recvfrom(void* msg, std::size_t len, internet_addr& src);
	
	int opt_error();
	
	void bind_any()
	{ bind(0); }
};

struct internet_addr_impl : gvl::shared
{
	friend struct internet_addr;
	
	internet_addr_impl(bool is_set_init = false)
	: is_set_(is_set_init)
	{
	}
	
	virtual ~internet_addr_impl()
	{
	}
	
	internet_addr_impl* clone();
	
	virtual void* storage() = 0;
	
	bool is_set_;
};

struct internet_addr // : internet_addr_storage
{
	internet_addr(); // any
	internet_addr(char const* name, int port = 0);
	
	// NOTE: This may not get the address unless I/O has occured
	// on the socket or (if applicable) a connect or accept has
	// been done.
	internet_addr(socket s);
	//internet_addr(host_entry* hp, int port);
	internet_addr(uint32_t ip, int port);
	
	void* storage()
	{ return ptr->storage(); }
	void const* storage() const
	{ return ptr->storage(); }
	
	operator bool()
	{ return ptr->is_set_; }
	
    int  port();
    void port(int port_new);
    
    uint32_t ip();
    void     ip(uint32_t ip_new);
    
    void reset()
    { ptr->is_set_ = false; }

    friend bool operator==(internet_addr const&, internet_addr const&);
    
    gvl::shared_ptr<internet_addr_impl> ptr;
};

/*
struct host_entry_impl : gvl::shared
{
	virtual ~host_entry_impl()
	{
	}
	
	virtual void* storage() = 0;
};

struct host_entry
{
	host_entry(char const* name);
	
	void* storage()
	{ return ptr->storage(); }
	
	gvl::shared_ptr<host_entry_impl> ptr;
};*/

//host_entry* resolve_host(char const* name);
socket      tcp_socket();
socket      udp_socket();

#ifdef GVL_WIN32
void init_sockets();
void deinit_sockets();
#else
inline void init_sockets() {}
inline void deinit_sockets() {}
#endif

} // namespace gvl

#endif // UUID_2E6087A4B6164BFF4F47018450AFE807
