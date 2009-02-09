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
	
	tut::runner.get().run_tests();
}