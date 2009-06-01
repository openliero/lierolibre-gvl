#include "uthread.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include "../support/log.hpp"
#include "../support/debug.hpp"
#include "uthread_config.hpp"

#if defined(CO_USE_FIBERS)
#define _WIN32_WINNT 0x0400
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _MSC_VER
#pragma comment(lib, "kernel32.lib")
#endif

#elif defined(CO_USE_UCONEXT)
#include <ucontext.h>
#else
#include <setjmp.h>
#endif

#if defined(CO_USE_SIGCONTEXT)
#include <signal.h>
#endif

namespace gvl
{

// Implementation details
namespace
{

typedef size_t integerptr_t;

#if defined(CO_USE_FIBERS)
typedef void* ctx_t;
#elif defined(CO_USE_UCONEXT)
typedef ucontext_t ctx_t;
#else
typedef jmp_buf ctx_t;
#endif

std::size_t const min_stack_size = 1024 * 4;


struct uthread_impl : public uthread
{
	void (*entry)(void* user);
	void (*yielder)(void* user);
	void* yielder_user;
	
	ctx_t ctx;
#if !defined(CO_USE_FIBERS)
	char* stack;
#else
	static VOID CALLBACK fiber_runner(PVOID lpParameter);
	static void switch_back(void* user);
	
	uthread_impl()
	: ctx(0)
	{
	}
	
	uthread_impl(ctx_t ctx)
	: ctx(ctx)
	{
	}
	
	uthread_impl(void (*entry)(void*), void* entry_user, std::size_t stack_size)
	: entry(entry)
	{
		if(stack_size < min_stack_size)
		{
			stack_size = min_stack_size;
		}
	
		ctx = CreateFiber(stack_size, fiber_runner, entry_user);
	}
#endif // #if !defined(CO_USE_FIBERS)

	void switch_to();

	~uthread_impl();
};

uthread_impl* impl(uthread* p)
{ return static_cast<uthread_impl*>(p); }

// TODO: Make thread locals
//coro main((coro::main_tag()));
uthread* main = new uthread_impl;
uthread* curr = main;

uthread_impl::~uthread_impl()
{
	if(this == main)
		throw error("Attempt to delete main coro");
		
	if(this == curr) // It's not safe to clean up itself
		throw error("Attempt to delete current coro");
	
#	if defined(CO_USE_FIBERS)
	if(ctx != GetCurrentFiber())
		DeleteFiber(ctx);
#	else
	std::free(stack);
#	endif
}

//uthread* dhelper = 0;

#if defined(CO_USE_SIGCONTEXT)

volatile int ctx_called;
ctx_t* ctx_creating;
void* ctx_creating_func;
sigset_t ctx_creating_sigs;
ctx_t ctx_trampoline;
ctx_t ctx_caller;

#endif /* #if defined(CO_USE_SIGCONTEXT) */


int ctx_sdir(integerptr_t psp)
{
	int nav = 0;
	integerptr_t csp = integerptr_t(&nav);

	return psp > csp ? -1: +1;
}


static int ctx_stackdir(void)
{
	int cav = 0;

	return ctx_sdir(integerptr_t(&cav));
}

#if defined(CO_USE_FIBERS)

void switch_context(ctx_t& octx, ctx_t& nctx)
{
	if(!main)
	{
		main = new uthread_impl(ConvertThreadToFiber(0));
	}
	else if(PVOID(0x1e00) == GetCurrentFiber())
	{
		// Somehow we're not in a fiber, convert to a fiber so we can switch
		impl(main)->ctx = ConvertThreadToFiber(0);
	}
	
	SwitchToFiber(nctx);
}
#endif


#if defined(CO_USE_FIBERS)

VOID CALLBACK uthread_impl::fiber_runner(PVOID lpParameter)
{
	uthread* co = curr;
	
	//co->restarget = co->caller;
	
	if(!co->is_done()) // Check if the coroutine was already flagged as done
	{
		try
		{
			impl(co)->entry(lpParameter);
		}
		catch(std::exception& e)
		{
			ILOG("uthread exited from exception: " << e.what());
		}
		catch(...)
		{
			
		}
		
		impl(co)->flags |= uthread::done;
	}
	
	
	// Wait for the destruction
	for(;;)
	{
		try
		{
			yield();
		}
		catch(...)
		{
		}
	}
	
}
#endif

#if 0 // TODO
struct del_helper : coro
{
	del_helper()
	: coro(min_stack_size)
	{
	}
	
	void go()
	{
		for(;;)
		{
			coro* cdh = dhelper;
			dhelper = 0;
			delete curr->caller;
			cdh->call();
			
			if(!dhelper)
			{
				throw error("Resume to delete helper coroutine");
			}
		}
	}
};
#endif

void uthread_impl::switch_back(void* user)
{
	impl(static_cast<uthread*>(user))->switch_to();
}

} // namespace

uthread* uthread::create(void (*entry)(void*), void* entry_user, std::size_t stack_size)
{
	return new uthread_impl(entry, entry_user, stack_size);
}

void uthread::clean_up()
{
	if(!(flags & done))
	{
		flags |= done;
		call();
	}
}

uthread::~uthread()
{
	
}

void uthread_impl::switch_to()
{
	uthread_impl* oldco = impl(curr);
	
	sassert(oldco != impl(this));
	
	curr = this;

	switch_context(oldco->ctx, impl(this)->ctx);

	if(curr->is_done())
		throw is_done();
}

void uthread::call()
{
	impl(this)->yielder = uthread_impl::switch_back;
	impl(this)->yielder_user = curr;
	impl(this)->switch_to();
}

void uthread::switch_to(void (*yielder)(void*), void* yielder_user)
{
	impl(this)->yielder = yielder;
	impl(this)->yielder_user = yielder_user;
	impl(this)->switch_to();
}

void yield()
{
	impl(curr)->yielder(impl(curr)->yielder_user);
}

#if 0 // TODO
void uthread::exit_to()
{
	static coro* dchelper = 0;

	if(!dchelper)
	{
		dchelper = new del_helper();
	}

	dhelper = this;
 
	dchelper->call();
	
	throw error("Stale coroutine called");
}

void exit()
{
	curr->restarget->exit_to();
}
#endif

} // namespace gvl
