#include <catch.hpp>

#include <ophidian/parser/TimingParser.h>

using namespace ophidian::parser;
using namespace ophidian::timing;

TEST_CASE("Timing: read simple dot timing.", "[parser][TimingParser]")
{
    TimingParser timingParser;
    Timing* timingPtr = timingParser.readFile("input_files/simple.timing");
    REQUIRE(timingPtr->clockName() == "iccad_clk");
    REQUIRE(timingPtr->clockPeriod() == 80.0);
    REQUIRE(timingPtr->rat("out", EARLY_MODE, RISE_TRANSITION) == 0.0);
    REQUIRE(timingPtr->rat("out", EARLY_MODE, FALL_TRANSITION) == 0.0);
    REQUIRE(timingPtr->rat("out", LATE_MODE, RISE_TRANSITION) == 80.0);
    REQUIRE(timingPtr->rat("out", LATE_MODE, FALL_TRANSITION) == 80.0);
}
