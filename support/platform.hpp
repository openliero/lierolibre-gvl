#ifndef UUID_E256FC00B8C44EBE85C8738284C064F6
#define UUID_E256FC00B8C44EBE85C8738284C064F6

// NOTE: Keep this usable from C

#if !defined(GVL_WIN32)
#if defined(WIN32) || defined(_WIN32) // TODO: Check for Cygwin
#define GVL_WIN32 1
#else
#define GVL_WIN32 0
#endif
#endif

#if !defined(GVL_MSVCPP)
#if defined(_MSC_VER)
#define GVL_MSVCPP 1
#else
#define GVL_MSVCPP 0
#endif
#endif

#if !defined(GVL_GCC)
#if defined(__GNUC__)
#define GVL_GCC 1
#else
#define GVL_GCC 0
#endif
#endif

#if !defined(GVL_LINUX)
#if defined(__linux__)
#define GVL_LINUX 1
#else
#define GVL_LINUX 0
#endif
#endif

#if !defined(GVL_X86)
#if defined(__i386__) || defined(GVL_WIN32)
#define GVL_X86 1
#else
#define GVL_X86 0
#endif
#endif

#endif // UUID_E256FC00B8C44EBE85C8738284C064F6
