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

	void        set_nonblocking(bool no_blocking = true);
	int         set_nodelay(bool no_delay = true);
	int         bind(int port);
	int         listen();
	socket      accept(internet_addr& addr);
	int         connect(internet_addr& addr);

	/* Communication */
	int send(void const* msg, std::size_t len);
	int recv(void* msg, std::size_t len);
	int sendto(void const* msg, std::size_t len, internet_addr const& dest);
	int recvfrom(void* msg, std::size_t len, internet_addr& src);

	int opt_error();

	void bind_any()
	{ bind(0); }
};

int const SS_MAXSIZE = 128;                 // Maximum size
#ifdef GVL_MSVCPP
int const SS_ALIGNSIZE = (sizeof(__int64)); // Desired alignment
#elif GVL_GCC
int const SS_ALIGNSIZE = (sizeof(int64_t)); // Desired alignment
#endif
int const SS_PAD1SIZE = (SS_ALIGNSIZE - sizeof(short));
int const SS_PAD2SIZE = (SS_MAXSIZE - (sizeof(short) + SS_PAD1SIZE + SS_ALIGNSIZE));

struct sockaddr_storage
{
    short ss_family;               // Address family.

    char _ss_pad1[SS_PAD1SIZE];  // 6 byte pad, this is to make
                                   //   implementation specific pad up to
                                   //   alignment field that follows explicit
                                   //   in the data structure
    int64_t _ss_align;            // Field to force desired structure
    char _ss_pad2[SS_PAD2SIZE];  // 112 byte pad to achieve desired size;
                                   //   _SS_MAXSIZE value minus size of
                                   //   ss_family, __ss_pad1, and
                                   //   __ss_align fields is 112
};

struct internet_addr
{
	internet_addr(); // any
	internet_addr(char const* name, int port = 0);

	// NOTE: This may not get the address unless I/O has occured
	// on the socket or (if applicable) a connect or accept has
	// been done.
	internet_addr(socket s);
	internet_addr(uint32_t ip, int port);

	bool valid();

    int  port() const;
    void port(int port_new);

    uint32_t ip() const;
    void     ip(uint32_t ip_new);

    void reset();

    friend bool operator==(internet_addr const&, internet_addr const&);

	bool operator!=(internet_addr const& other) const
	{ return !(*this == other); }

	sockaddr_storage storage_;
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
