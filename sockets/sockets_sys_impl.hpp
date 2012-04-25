#ifndef UUID_79D7290192F84DAEF23EECAB7295F249
#define UUID_79D7290192F84DAEF23EECAB7295F249

#include "sockets.hpp"

#if GVL_WIN32 || GVL_WIN64
#undef  NOGDI
#define NOGDI
#undef  NOMINMAX
#define NOMINMAX
#undef  WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#undef  NONAMELESSUNION
#define NONAMELESSUNION
#undef  NOKERNEL
#define NOKERNEL
#undef  NONLS
#define NONLS

/*
#ifndef POINTER_64
#define POINTER_64 // Needed for bugged headers
#endif*/

#if GVL_WIN32
#define _WIN32_WINDOWS 0x0410
#endif

#define WINVER 0x0410
#include <winsock2.h>
#include <cstdlib>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

namespace gvl
{

inline SOCKET native_socket(socket s) { return (SOCKET)s._voidp; }

inline socket make_socket(SOCKET native)
{
	socket s;
	s._voidp = (void*)native;
	return s;
}

typedef int socklen_t;

} // namespace gvl

#else //if !defined(OSK)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace gvl
{

inline int native_socket(socket s) { return s._int; }

inline socket make_socket(int native)
{
	socket s;
	s._int = native;
	return s;
}

} // namespace gvl

#endif

/*
namespace gvl
{

struct host_entry : hostent
{
};

} // namespace gvl
*/

#endif // UUID_79D7290192F84DAEF23EECAB7295F249
