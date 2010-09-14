#include "mutex.hpp"

#include "windows.hpp"

void gvl_mutex_init(gvl_mutex* self)
{
#if GVL_WINDOWS
	self->handle = CreateEvent(NULL, FALSE, TRUE, NULL);
#endif
}

void gvl_mutex_lock(gvl_mutex* self)
{
#if GVL_WINDOWS	
	while(WaitForSingleObjectEx(self->handle, INFINITE, TRUE) != WAIT_OBJECT_0)
		/* Nothing */;
#endif
}

void gvl_mutex_unlock(gvl_mutex* self)
{
#if GVL_WINDOWS	
	SetEvent(self->handle);
#endif
}

void gvl_mutex_destroy(gvl_mutex* self)
{
#if GVL_WINDOWS
	CloseHandle(self->handle);
#endif
}
