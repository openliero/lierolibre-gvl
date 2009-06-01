#ifndef UUID_E256FC00B8C44EBE85C8738284C064F6
#define UUID_E256FC00B8C44EBE85C8738284C064F6

// NOTE: Keep this usable from C

#if !defined(GVL_WIN64)
#if defined(WIN64) || defined(_WIN64) // TODO: Check for Cygwin
#define GVL_WIN64 1
#else
#define GVL_WIN64 0
#endif
#endif

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
#if defined(__linux__) || defined(linux)
#define GVL_LINUX 1
#else
#define GVL_LINUX 0
#endif
#endif

#define GVL_X86 1
#define GVL_X86_64 2

#define GVL_LITTLE_ENDIAN 1
#define GVL_BIG_ENDIAN 2

#if !defined(GVL_ARCH)
#if defined(__i386__) || defined(_M_IX86) || defined(i386) || defined(i486) || defined(intel) || defined(x86) || defined(i86pc)
#define GVL_ARCH GVL_X86
#elif defined(_M_X64) || defined(__x86_64__)
#define GVL_ARCH GVL_X86_64
#else
#error "Unknown architecture, please add it"
#endif
#endif

#if !defined(GVL_ENDIAN)
#if GVL_ARCH==GVL_X86 || GVL_ARCH==GVL_X86_64
#define GVL_ENDIAN GVL_LITTLE_ENDIAN
#else
#define GVL_ENDIAN GVL_BIG_ENDIAN
#endif
#endif

#if defined(__cplusplus)
#define GVL_INLINE inline
#elif GVL_MSVCPP
#define GVL_INLINE __inline
#else
#define GVL_INLINE static inline
#endif

// Whether or not the compiler may generate x87 code for floating point calculations.
// GVL_X87 == 1 means the gvl IEEE support functions will take measures to work-around
// x87 issues that make results non-reproducible.
#if !defined(GVL_X87)
#if GVL_ARCH == GVL_X86
#define GVL_X87 1 // Assume the compiler generates x87 code on x86 unless otherwise stated
#endif
#endif

// TODO: This function should try to check whether the above inferred
// characteristics are correct.
// Returns: true if they are correct, false otherwise.
int gvl_test_platform();

#endif // UUID_E256FC00B8C44EBE85C8738284C064F6
