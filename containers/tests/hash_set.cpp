#include "../../test/test.hpp"

#include "../../hash_set/hash_set.hpp"

GVLTEST_SUITE(gvl, hash_set_new)
GVLTEST_SUITE(gvl, hash_set_unboxed)

GVLTEST(gvl, hash_set_new, insert_find)
{
	gvl::hash_set_new<int> h;

	for(int i = 0; i < 100; ++i)
		h.insert(i);

	for(int i = 0; i < 100; ++i)
		GVLTEST_ASSERT("exists", h.has(i) && h.lookup(i)->key() == i);
	for(int i = 100; i < 200; ++i)
		GVLTEST_ASSERT("not exists", !h.has(i));

	
}

GVLTEST(gvl, hash_set_unboxed, insert_find)
{
	gvl::hash_set_unboxed<int> h;

	// 0 will work as empty element
	for(int i = 1; i < 100; ++i)
		h.insert(i);

	for(int i = 1; i < 100; ++i)
		GVLTEST_ASSERT("exists", h.has(i) && *h.get(i) == i);
	for(int i = 100; i < 200; ++i)
		GVLTEST_ASSERT("not exists", !h.has(i));

	for(int i = 100; i < 100000; ++i)
		h.insert(i);

	for(int i = 100; i < 100000; ++i)
		GVLTEST_ASSERT("exists 2", h.has(i) && h.lookup(i)->key() == i);

	for(int i = 500; i < 5000; ++i)
		h.erase(i);

	for(int i = 100; i < 500; ++i)
		GVLTEST_ASSERT("exists 3", h.has(i));
	for(int i = 500; i < 5000; ++i)
		GVLTEST_ASSERT("not exists 2", !h.has(i));
	for(int i = 5000; i < 100000; ++i)
		GVLTEST_ASSERT("exists 4", h.has(i));
}