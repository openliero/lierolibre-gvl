#ifndef UUID_42BBB6C432494D09B3881EA0734FB3E1
#define UUID_42BBB6C432494D09B3881EA0734FB3E1

#if defined(__GNUC__)
#define FD_INLINE static inline
#elif defined(_MSC_VER)
/*
#pragma fp_contract(off)
#pragma fenv_access(on)*/
#define FD_INLINE __inline
#endif

#endif // UUID_42BBB6C432494D09B3881EA0734FB3E1
