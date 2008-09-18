#include "shared.hpp"

namespace gvl
{

void shared::_delete()
{
#if 0 // TODO
	for(weak_ptr_common* p = _first; p; )
	{
		weak_ptr_common* n = p->next;
		//p->v = 0;
		p->_clear();
		p->next = 0;
		p = n;
	}
#endif
	
	delete this;
}

} // namespace gvl
