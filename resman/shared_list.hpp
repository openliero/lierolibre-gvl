#ifndef UUID_2780CFD020A54BE4F3120E87E0415D3D
#define UUID_2780CFD020A54BE4F3120E87E0415D3D

#include "shared.hpp"

namespace gvl
{

struct shared_releaser
{
	void operator()(shared* p)
	{
		p->release();
	}
};

struct shared_ownership
{
	static shared* take(shared* p)
	{
		return p;
	}
};

template<typename T, typename Tag = default_list_tag>
struct shared_list : list<T, Tag, shared_releaser, shared_ownership>
{
	shared_list()
	{
	}
};

} // namespace gvl

#endif // UUID_2780CFD020A54BE4F3120E87E0415D3D
