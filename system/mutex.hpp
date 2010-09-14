#ifndef UUID_84A118B9780C45F80DE92E8BEE587EA6
#define UUID_84A118B9780C45F80DE92E8BEE587EA6

#include "../support/platform.hpp"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gvl_mutex_
{
#if GVL_WINDOWS
	void* handle;
#else
#endif
} gvl_mutex;

void gvl_mutex_init(gvl_mutex* self);
void gvl_mutex_lock(gvl_mutex* self);
void gvl_mutex_unlock(gvl_mutex* self);
void gvl_mutex_destroy(gvl_mutex* self);

#ifdef __cplusplus
}
#endif

#endif // UUID_84A118B9780C45F80DE92E8BEE587EA6
