#include <catch.hpp>
#include <ophidian/timing/Timer.h>

TEST_CASE("Timer: test open timer on tau 2015 simple.", "[timing]")
{
    CHECK(ophidian::timing::run_open_timer() == Approx(-204.347f));
}
