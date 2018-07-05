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
#include <ophidian/parser/LibertyParser.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/timing/TimingGraphBuilder.h>

using namespace ophidian;

namespace
{
class TimingGraphBuilderFixture
{
public:
    design::ICCAD2015ContestDesignBuilder mBuilder;
    design::Design & mDesign;

    std::shared_ptr<ophidian::parser::Liberty> mLiberty;
    timing::TimingArcs mTimingArcs;
    timing::Library mTimingLibrary;
    std::shared_ptr<parser::DesignConstraints> mDC;

    TimingGraphBuilderFixture() :
        mBuilder("./input_files/simple/simple.lef", "./input_files/simple/simple.def", "./input_files/simple/simple.v"),
        mDesign(mBuilder.build()),
        mLiberty(ophidian::parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        mTimingArcs(mDesign.standardCells()),
        mTimingLibrary(mDesign.standardCells(), mTimingArcs),
        mDC(parser::SDCSimple().constraints())
    {
        mTimingLibrary.init(*mLiberty, true);
    }
};
} // namespace

TEST_CASE_METHOD(TimingGraphBuilderFixture, "TimingGraphBuilder: init", "[timing][TimingGraphBuilder]")
{
    timing::TimingGraphBuilder builder;
    timing::TimingGraph graph(mDesign.netlist());

    builder.build(mDesign.netlist(), mDesign.standardCells(), mDesign.libraryMapping(), mTimingArcs, mTimingLibrary, *mDC, graph);

    REQUIRE(graph.size(timing::TimingGraph::node_type()) == 42);
    REQUIRE(graph.size(timing::TimingGraph::arc_type()) == 40);

    auto u1_a = mDesign.netlist().find(circuit::Pin(), "u1:a");
    auto u1_o = mDesign.netlist().find(circuit::Pin(), "u1:o");
    auto u2_a = mDesign.netlist().find(circuit::Pin(), "u2:a");

    auto riseU1_a = graph.riseNode(u1_a);
    auto riseU1_o = graph.riseNode(u1_o);
    auto fallU1_o = graph.fallNode(u1_o);
    auto riseU2_a = graph.riseNode(u2_a);

    auto timingArcU1 = mTimingArcs.find("NAND2_X1:a->NAND2_X1:o");
    auto netU1ToU2 = mDesign.netlist().find(circuit::Net(), "n1");

    REQUIRE(graph.graph().id(graph.target(graph.outArc(riseU1_a))) == graph.graph().id(fallU1_o));
    REQUIRE(graph.property(graph.outArc(riseU1_a)) == timing::TimingGraph::ArcProperty::TimingArc);
    REQUIRE(graph.entity(timing::TimingArcs::timing_arc_entity_type(), graph.outArc(riseU1_a)) == timingArcU1);

    REQUIRE(graph.graph().id(graph.target(graph.outArc(riseU1_o))) == graph.graph().id(riseU2_a));
    REQUIRE(graph.property(graph.outArc(riseU1_o)) == timing::TimingGraph::ArcProperty::Net);
    REQUIRE(graph.entity(circuit::Net(), graph.outArc(riseU1_o)) == netU1ToU2);
}
