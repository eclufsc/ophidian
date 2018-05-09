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
#include <ophidian/circuit/Netlist.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/timing/Library.h>
#include <ophidian/timing/TimingData.h>

namespace
{
class TGCondensationFixture
{
public:
    ophidian::circuit::Netlist               mNetlist;
    ophidian::standard_cell::StandardCells   mStdCells;
    ophidian::timing::TimingGraph            mGraph;
    ophidian::timing::TimingGraph::node_type mFrom, mTo;
    ophidian::timing::TimingGraph::arc_type  mArc;
    ophidian::timing::TimingArcs             mTimingArcs;
    ophidian::timing::Library                mLibrary;

    TGCondensationFixture() :
        mGraph(mNetlist),
        mTimingArcs(mStdCells),
        mLibrary(ophidian::parser::Liberty(), mStdCells, mTimingArcs, true)
    {
        auto from = mNetlist.add(ophidian::circuit::Pin(), "from");
        auto to = mNetlist.add(ophidian::circuit::Pin(), "to");

        mFrom = mGraph.riseNodeCreate(from);
        mTo = mGraph.riseNodeCreate(to);
        mArc = mGraph.arcCreate(mFrom, mTo, ophidian::timing::TimingArcs::timing_arc_entity_type());
    }
};
} // namespace

TEST_CASE_METHOD(TGCondensationFixture, "TimingGraph Condensation","[timing][sta][condensation]")
{
    using namespace ophidian;

    SECTION("TimingGraph Condensation: Values of nodes after creation","[timing][sta][condensation]")
    {
        using time_unit_type = timing::TimingData::time_unit_type;
        using capacitance_unit_type = timing::TimingData::capacitance_unit_type;

        timing::TimingData data(mLibrary, mGraph);
        data.arrival(mFrom, time_unit_type(1.0));
        data.slew(mFrom, time_unit_type(3.0));
        data.required(mFrom, time_unit_type(5.0));
        data.load(mFrom, capacitance_unit_type(6.0));

        REQUIRE(data.arrival(mFrom) == time_unit_type(1.0));
        REQUIRE(data.slew(mFrom) == time_unit_type(3.0));
        REQUIRE(data.required(mFrom) == time_unit_type(5.0));
        REQUIRE(data.load(mFrom) == capacitance_unit_type(6.0));
    }

    SECTION("TimingGraph Condensation: Values of arcs after creation", "[timing][sta][condensation]")
    {
        using time_unit_type = timing::TimingData::time_unit_type;

        timing::TimingData data(mLibrary, mGraph);
        data.delay(mArc, time_unit_type(10.0));
        data.slew(mArc, time_unit_type(12.0));

        REQUIRE(data.delay(mArc) == time_unit_type(10.0));
        REQUIRE(data.slew(mArc) == time_unit_type(12.0));
    }
}
