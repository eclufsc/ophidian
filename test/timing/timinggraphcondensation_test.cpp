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
#include <ophidian/timing/TimingGraphCondensation.h>

namespace
{
class TGCondensationFixture
{
public:
    ophidian::circuit::Netlist              mNetlist;
    ophidian::timing::TimingGraph           mGraph;
    ophidian::timing::TimingGraph::NodeType mFrom, mTo;
    ophidian::timing::TimingGraph::ArcType  mArc;

    TGCondensationFixture() :
        mGraph(mNetlist)
    {
        auto from = mNetlist.add(ophidian::circuit::Pin(), "from");
        auto to = mNetlist.add(ophidian::circuit::Pin(), "to");

        mFrom = mGraph.riseNodeCreate(from);
        mTo = mGraph.riseNodeCreate(to);
        mArc = mGraph.arcCreate(mFrom, mTo, ophidian::timing::TimingArc());
    }
};
} // namespace

TEST_CASE_METHOD(TGCondensationFixture, "TimingGraph Condensation: Values of nodes after creation","[timing][sta][condensation]")
{
    using namespace ophidian;

    timing::GraphNodesTiming nodes(mGraph);
    nodes.arrival(mFrom, util::second_t(1.0));
    nodes.slew(mFrom, util::second_t(3.0));
    nodes.required(mFrom, util::second_t(5.0));
    nodes.load(mFrom, util::farad_t(6.0));

    REQUIRE(nodes.arrival(mFrom) == util::second_t(1.0));
    REQUIRE(nodes.slew(mFrom) == util::second_t(3.0));
    REQUIRE(nodes.required(mFrom) == util::second_t(5.0));
    REQUIRE(nodes.load(mFrom) == util::farad_t(6.0));
}

TEST_CASE_METHOD(TGCondensationFixture, "TimingGraph Condensation: Values of arcs after creation", "[timing][sta][condensation]")
{
    using namespace ophidian;

    timing::GraphArcsTiming arcs(mGraph);
    arcs.delay(mArc, util::second_t(10.0));
    arcs.slew(mArc, util::second_t(12.0));

    REQUIRE(arcs.delay(mArc) == util::second_t(10.0));
    REQUIRE(arcs.slew(mArc) == util::second_t(12.0));
}
