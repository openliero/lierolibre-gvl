#ifndef UUID_D13E73786A7C4F75E2481BB7E79A5B37
#define UUID_D13E73786A7C4F75E2481BB7E79A5B37

#define GVL_INTERFACE(name, members) \
typedef struct name##_vtable_ members name##_vtable; \
typedef struct name##_ { void* ptr; name##_vtable* vtable; }; \
INLINE name make_##name(void* ptr, name##_vtable* vtable) { \
	name ret; \
	ret.ptr = ptr; \
	ret.vtable = vtable; \
	return ret; }

#define GVL_INTERFACE1(name, member) \
typedef struct name##_ { void* ptr; member; };

#define GVL_DECL_INTERFACE(interf, vtable_name) \
extern interf##_table vtable_name;

#define GVL_DEF_INTERFACE(interf, vtable_name, vtable_funcs) \
interf##_table vtable_name = vtable_funcs;

#define GVL_REF_INTERFACE(self, member) ((self)->vtable.member)
#define GVL_REF_INTERFACE1(self, member) ((self)->member)


GVL_INTERFACE(printable, { void (*print)(void*); })

GVL_DECL_INTERFACE(printable, printable_int)

void print_int(void* self)
{
	printf("%d", (int)self);
}

GVL_DEF_INTERFACE(printable, printable_int, { print_int })

#define PRINTABLE_FROM_INT(i) make_printable((void*)(i), printable_int)

void test()
{
	int i = 10;
	printable ip = PRINTABLE_FROM_INT(i);
	GVL_REF_INTERFACE(ip, print)(ip.ptr);
}

#endif // UUID_D13E73786A7C4F75E2481BB7E79A5B37
