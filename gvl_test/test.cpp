#include <gvl/tut/tut.hpp>
#include <gvl/tut/tut_reporter.hpp>

namespace tut
{
test_runner_singleton runner;
}

int main()
{
	tut::reporter visi;
	tut::runner.get().set_callback(&visi);
#if 1
	tut::runner.get().run_tests("opt.hpp");
	//tut::runner.get().run_test("gvl::recursive_bounded_rectangle_packing", 1);
	//tut::runner.get().run_test("gvl::accurate_rectangle_packing", 1);
#else
	tut::runner.get().run_tests();
#endif
}