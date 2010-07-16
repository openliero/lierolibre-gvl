#include <gvl/tut/tut.hpp>

#include <gvl/containers/deque.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <gvl/resman/allocator.hpp>
#include <gvl/support/platform.hpp>
#include <functional>
#include <cmath>
#include <set>
#include <ctime>
#include <cstdio>

namespace tut
{

struct track_obj;

std::set<track_obj*> track_obj_live;
int track_obj_copies = 0;

#define MOVABLE CPP0X

struct track_obj
{
	track_obj(int v)
	: v(v)
	{
		track_obj_live.insert(this);
	}

#if !MOVABLE
	track_obj(track_obj const& other)
	: v(other.v)
	{
		++track_obj_copies;
		track_obj_live.insert(this);
	}

	track_obj& operator=(track_obj const& other)
	{
		++track_obj_copies;
		v = other.v;
		return *this;
	}
#endif

	~track_obj()
	{
		if(track_obj_live.count(this) == 0)
			throw std::runtime_error("non-live track_obj was destructed");
		track_obj_live.erase(this);
	}

	int v;

#if MOVABLE
	track_obj(track_obj&& other)
	: v(other.v)
	{
		track_obj_live.insert(this);
	}

	track_obj& operator=(track_obj&& other)
	{
		v = other.v;
		return *this;
	}

private:
	track_obj(track_obj const& other);
	track_obj& operator=(track_obj const& other);
#endif

};

struct deque_data
{
	
};

typedef test_group<deque_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::deque");
} // namespace

namespace tut
{


template<>
template<>
void object::test<1>()
{
	typedef gvl::deque<track_obj> l_t;
	
	l_t l;
	
	gvl::tt800 r(1234);
	
	int const iter = 1000;
	int const limit = iter;
	
	for(int i = 0; i < iter; ++i)
	{
		l.push_back(track_obj(i));
	}

	for(int i = 0; i < iter; ++i)
	{
		track_obj& v = l.front();
		ensure("popped in the right order", v.v == i);
		l.pop_front();
	}

	ensure(l.empty());

	ensure("all track_obj were destroyed", track_obj_live.empty());
	
#if MOVABLE
	ensure("no copied were made in C++0x", track_obj_copies == 0);
#endif
}

} // namespace tut

