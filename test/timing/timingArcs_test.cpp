#include "timingArcs_test.h"
#include <catch.hpp>

#include <ophidian/timing/TimingArcs.h>
#include <ophidian/standard_cell/StandardCells.h>

using namespace ophidian;

TEST_CASE("TimingArcs: Brand-new TimingArcs must be empty.", "[Timing][TimingArcs]")
{
    timing::TimingArcs arcs;
    REQUIRE(arcs.size() == 0);
    REQUIRE(arcs.begin() == arcs.end());
}

TEST_CASE("TimingArcs: Add Arc.", "[Timing][TimingArcs]")
{
    timing::TimingArcs arcs;
    auto arc = arcs.add("arc");
    REQUIRE(arcs.size() == 1);
}

TEST_CASE("TimingArcs: Erase Arc.", "[Timing][TimingArcs]")
{
    timing::TimingArcs arcs;
    auto arc = arcs.add("arc");
    arcs.erase(arc);
    REQUIRE(arcs.size() == 0);
}

TEST_CASE("TimingArcs: Arc mapping.", "[Timing][TimingArcs]")
{
    timing::TimingArcs arcs;
    arcs.add("arc");
    auto arc = arcs.find("arc");
    arcs.erase(arc);
    REQUIRE(arcs.size() == 0);
}

TEST_CASE("TimingArcs: Try add the same arc twice.", "[Timing][TimingArcs]")
{
    timing::TimingArcs arcs;
    arcs.add("arc");
    arcs.add("arc");
    REQUIRE(arcs.size() == 1);
}

TEST_CASE("TimingArcs: Finding a Arc.", "[Timing][TimingArcs]")
{
    timing::TimingArcs arcs;
    auto arc1 = arcs.add("arc1");
    auto arc2 = arcs.add("arc2");
    REQUIRE(arcs.size() == 2);
    REQUIRE(arcs.find("arc1") == arc1);
    REQUIRE(arcs.find("arc2") == arc2);
}

TEST_CASE("TimingArcs: Add from and to of an arc.", "[Timing][TimingArcs]")
{
    timing::TimingArcs arcs;
    standard_cell::StandardCells stdCells;
    auto from = stdCells.add(standard_cell::Pin(), "from", standard_cell::PinDirection::INPUT);
    auto to = stdCells.add(standard_cell::Pin(), "to", standard_cell::PinDirection::OUTPUT);
    auto arc = arcs.add("arc");
    arcs.from(arc, from);
    arcs.to(arc, to);
    arcs.from(arc, from);
    arcs.to(arc, to);
}

TEST_CASE("TimingArcs: Make Arc Property.", "[Timing][TimingArcs]")
{
    timing::TimingArcs arcs;
    auto arc1 = arcs.add("arc1");
    auto arc2 = arcs.add("arc2");
    auto delay = arcs.makeProperty<double>();
    delay[arc1] = 1.1;
    delay[arc2] = 2.2;
    REQUIRE(delay[arc1] == 1.1);
    REQUIRE(delay[arc2] == 2.2);
}
