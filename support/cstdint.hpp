#ifndef UUID_EA12C28D969947BF0340D69440AE5D30
#define UUID_EA12C28D969947BF0340D69440AE5D30

// NOTE: Keep this useable from C

#include <limits.h>

#include "platform.hpp"

#if CHAR_BIT == 8
typedef unsigned char      uint8_t;
typedef   signed char      int8_t;
#else
#error "Only 8-bit chars supported"
#endif

#if USHRT_MAX == 65535
typedef unsigned short     uint16_t;
typedef          short     int16_t;
#elif UINT_MAX == 65535
typedef unsigned int       uint16_t;
typedef          int       int16_t;
#else
#error "No suitable 16-bit type"
#endif

#if USHRT_MAX == 0xffffffff
typedef unsigned short     uint32_t;
typedef          short     int32_t;
#elif UINT_MAX == 0xffffffff
typedef unsigned int       uint32_t;
typedef          int       int32_t;
#else
#error "No suitable 32-bit type"
#endif

#define IS_64(x) ((x) > 0xffffffff && (x) == 0xffffffffffffffff)

#if IS_64(ULONG_MAX)
typedef unsigned long      uint64_t;
typedef          long      int64_t;
#elif defined(ULLONG_MAX) && IS_64(ULLONG_MAX)
typedef unsigned long long uint64_t;
typedef          long long int64_t;
#else
#error "No suitable 64-bit type"
#endif

#endif // UUID_EA12C28D969947BF0340D69440AE5D30
