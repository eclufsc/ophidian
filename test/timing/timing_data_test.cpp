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
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/timing/Library.h>
#include <ophidian/timing/TimingData.h>
#include <ophidian/parser/SDCParser.h>
#include <ophidian/timing/TimingGraphBuilder.h>

using namespace ophidian;

using netlist_type        = circuit::Netlist;
using standard_cells_type = standard_cell::StandardCells;
using timing_graph_type   = timing::TimingGraph;
using timing_arcs_type    = timing::TimingArcs;
using timing_library_type = timing::Library;
using node_type           = timing_graph_type::node_type;
using arc_type            = timing_graph_type::arc_type;

namespace
{
class TimingDataFixture
{
public:
    design::ICCAD2015ContestDesignBuilder mBuilder;
    design::Design & mDesign;

    std::shared_ptr<ophidian::parser::Liberty> mLiberty;
    timing::TimingArcs mTimingArcs;
    timing::Library mTimingLibrary;
    std::shared_ptr<parser::DesignConstraints> mDC;
    std::unique_ptr<parser::Lef> mLef;

    std::shared_ptr<timing::TimingGraph> mGraph;

    timing::TimingGraph::node_type mFrom, mTo;
    timing::TimingGraph::arc_type mArc;

    TimingDataFixture() :
        mBuilder("./input_files/simple/simple.lef", "./input_files/simple/simple.def", "./input_files/simple/simple.v"),
        mDesign(mBuilder.build()),
        mLiberty(parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        mTimingArcs(mDesign.standardCells()),
        mTimingLibrary(*mLiberty, mDesign.standardCells(), mTimingArcs, true),
        mDC(parser::SDCSimple().constraints()),
        mGraph(timing::TimingGraphBuilder().build(mDesign.netlist(),
                                                  mDesign.standardCells(),
                                                  mDesign.libraryMapping(),
                                                  mTimingArcs,
                                                  mTimingLibrary,
                                                  *mDC))
    {
        mFrom = mGraph->riseNode(mDesign.netlist().find(circuit::Pin(), "u1:o"));
        mTo = mGraph->riseNode(mDesign.netlist().find(circuit::Pin(), "u2:a"));

        mArc = mGraph->outArc(mFrom);
    }
};
} // namespace

TEST_CASE_METHOD(TimingDataFixture, "TimingGraph Condensation","[timing][sta][condensation]")
{
    SECTION("TimingGraph Condensation: Values of nodes after creation","[timing][sta][condensation]")
    {
        using time_unit_type = timing::TimingData::time_unit_type;
        using capacitance_unit_type = timing::TimingData::capacitance_unit_type;

        timing::TimingData data(mTimingLibrary, *mGraph);
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

        timing::TimingData data(mTimingLibrary, *mGraph);
        data.delay(mArc, time_unit_type(10.0));
        data.slew(mArc, time_unit_type(12.0));

        REQUIRE(data.delay(mArc) == time_unit_type(10.0));
        REQUIRE(data.slew(mArc) == time_unit_type(12.0));
    }
}
