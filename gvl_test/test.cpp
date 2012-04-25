#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

#include <gvl/support/profile.hpp>
#include <iostream>
#include <cstdio>

namespace tut
{
test_runner_singleton runner;
}

int main()
{
	tut::reporter visi;
	tut::runner.get().set_callback(&visi);
#if 0
	tut::runner.get().run_tests("gvl::bitstream");
	//tut::runner.get().run_tests("gvl::string");
	//tut::runner.get().run_tests("gvl::formatter");
	//tut::runner.get().run_test("gvl::recursive_bounded_rectangle_packing", 1);
	//tut::runner.get().run_test("gvl::accurate_rectangle_packing", 1);
#else
	tut::runner.get().run_tests();
#endif

	gvl::present_profile(std::cout);

	std::getchar();
}