/*
 * Copyright 2017 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at
  http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#include <catch.hpp>

#include <ophidian/timing/TimingArcs.h>
#include <ophidian/standard_cell/StandardCells.h>

using namespace ophidian;

namespace
{
class TimingArcFixture
{
public:
    standard_cell::StandardCells mStdCells;
    standard_cell::Pin mFrom, mTo;

    TimingArcFixture()
    {
        mFrom = mStdCells.add(standard_cell::Pin(), "from", standard_cell::PinDirection::OUTPUT);
        mTo = mStdCells.add(standard_cell::Pin(), "to", standard_cell::PinDirection::INPUT);
    }
};
} // namespace

TEST_CASE_METHOD(TimingArcFixture, "TimingArcs tests", "[Timing][TimingArcs]")
{
    SECTION("TimingArcs: Brand-new TimingArcs must be empty.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);
        REQUIRE(arcs.size() == 0);
        REQUIRE(arcs.begin() == arcs.end());
    }

    SECTION("TimingArcs: Add TimingArc.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);
        auto arc = arcs.add("arc");
        REQUIRE(arcs.size() == 1);
    }

    SECTION("TimingArcs: Erase TimingArc.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);
        auto arc = arcs.add("arc");
        arcs.erase(arc);
        REQUIRE(arcs.size() == 0);
    }

    SECTION("TimingArcs: TimingArc mapping.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);
        arcs.add("arc");
        auto arc = arcs.find("arc");
        arcs.erase(arc);
        REQUIRE(arcs.size() == 0);
    }

    SECTION("TimingArcs: Try add the same arc twice.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);
        arcs.add("arc");
        arcs.add("arc");
        REQUIRE(arcs.size() == 1);
    }

    SECTION("TimingArcs: Finding a TimingArc.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);
        auto arc1 = arcs.add("arc1");
        auto arc2 = arcs.add("arc2");
        REQUIRE(arcs.size() == 2);
        REQUIRE(arcs.find("arc1") == arc1);
        REQUIRE(arcs.find("arc2") == arc2);
    }

    SECTION("TimingArcs: Add from and to of an arc.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);
        auto arc = arcs.add("arc");
        arcs.from(arc, mFrom);
        arcs.to(arc, mTo);

        REQUIRE(arcs.from(arc) == mFrom);
        REQUIRE(arcs.to(arc) == mTo);
    }

    SECTION("TimingArcs: Make TimingArc Property.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);
        auto arc1 = arcs.add("arc1");
        auto arc2 = arcs.add("arc2");
        auto delay = arcs.makeProperty<double>();
        delay[arc1] = 1.1;
        delay[arc2] = 2.2;
        REQUIRE(delay[arc1] == 1.1);
        REQUIRE(delay[arc2] == 2.2);
    }

    SECTION("TimingArcs: Check association between arcs and a pin.", "[Timing][TimingArcs]")
    {
        timing::TimingArcs arcs(mStdCells);

        auto to2 = mStdCells.add(standard_cell::Pin(), "to2", standard_cell::PinDirection::INPUT);
        auto arc1 = arcs.add("arc1");
        auto arc2 = arcs.add("arc2");
        arcs.from(arc1, mFrom);
        arcs.to(arc1, mTo);
        arcs.from(arc2, mTo);
        arcs.to(arc2, to2);

        REQUIRE(arcs.pinArcs(mTo).size() == 2);
        REQUIRE(arcs.pinArcs(mTo).front() == arc1);
        REQUIRE(arcs.pinArcs(mTo).back() == arc2);
    }
}
