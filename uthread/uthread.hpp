#ifndef UUID_24B2BD5A1EFA448F69228FB9A9AD84D4
#define UUID_24B2BD5A1EFA448F69228FB9A9AD84D4

#include <cstddef>
#include <stdexcept>

namespace gvl
{

struct uthread
{
	friend void yield();
	
	struct error : std::runtime_error
	{
		error(std::string const& text)
		: std::runtime_error(text)
		{
		}
	};
	
	struct is_done // Exception that is thrown when a coroutine that is flagged as done is resumed
	{};

	enum
	{
		done = (1<<0)
	};
	
	uthread* create(void (*entry)(void*), void* entry_user = 0, std::size_t stack_size = 0);

	virtual ~uthread();
		
	void switch_to(void (*yielder)(void*), void* yielder_user);
	void call();
	
	//void exit_to();
	
	void clean_up();
	
	bool is_done()
	{
		return (flags & done) != 0;
	}

protected:
	int flags;

	uthread()
	: flags(0)
	{
	}
};

void yield();

}

#endif // UUID_24B2BD5A1EFA448F69228FB9A9AD84D4
