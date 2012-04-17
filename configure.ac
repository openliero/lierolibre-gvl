AC_PREREQ([2.68])
AC_INIT([gvl], [0.1], [martinerikwerner@gmail.com], [gvl] [https://gitorious.org/gvl])
AC_CONFIG_SRCDIR([list.hpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT
AM_INIT_AUTOMAKE([1.11 -Wall subdir-objects])

AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h fenv.h float.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime floor gethostbyname localeconv memmove memset pow select socket sqrt])

AC_OUTPUT
