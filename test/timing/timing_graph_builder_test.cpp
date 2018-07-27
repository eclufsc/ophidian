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
    design::ICCAD2015ContestDesignBuilder m_builder;
    design::Design & m_design;

    std::shared_ptr<ophidian::parser::Liberty> m_liberty;
    timing::TimingArcs m_timing_arcs;
    timing::Library m_timing_library;
    std::shared_ptr<parser::DesignConstraints> m_dc;

    TimingGraphBuilderFixture() :
        m_builder("./input_files/simple/simple.lef",
                 "./input_files/simple/simple.def",
                 "./input_files/simple/simple.v",
                 "./input_files/simple/simple_Early.lib",
                 "./input_files/simple/simple_Late.lib",
                 "./input_files/simple/simple.sdc"),
        m_design(m_builder.build()),
        m_liberty(ophidian::parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        m_timing_arcs(m_design.standardCells()),
        m_timing_library(m_design.standardCells(), m_timing_arcs),
        m_dc(parser::SDCSimple().constraints())
    {
        m_timing_library.init(*m_liberty, true);
    }
};
} // namespace

TEST_CASE_METHOD(TimingGraphBuilderFixture, "TimingGraphBuilder: init", "[timing][TimingGraphBuilder]")
{
    timing::TimingGraphBuilder builder;
    timing::TimingGraph graph(m_design.netlist());

    builder.build(m_design.netlist(), m_design.standardCells(), m_design.libraryMapping(), m_timing_arcs, m_timing_library, *m_dc, graph);

    REQUIRE(graph.size(timing::TimingGraph::node_type()) == 42);
    REQUIRE(graph.size(timing::TimingGraph::arc_type()) == 40);

    auto u1_a = m_design.netlist().find(circuit::Pin(), "u1:a");
    auto u1_o = m_design.netlist().find(circuit::Pin(), "u1:o");
    auto u2_a = m_design.netlist().find(circuit::Pin(), "u2:a");

    auto riseU1_a = graph.rise_node(u1_a);
    auto riseU1_o = graph.rise_node(u1_o);
    auto fallU1_o = graph.fall_node(u1_o);
    auto riseU2_a = graph.rise_node(u2_a);

    auto timingArcU1 = m_timing_arcs.find("NAND2_X1:a->NAND2_X1:o");
    auto netU1ToU2 = m_design.netlist().find(circuit::Net(), "n1");

    REQUIRE(graph.graph().id(graph.target(graph.out_arc_iterator(riseU1_a))) == graph.graph().id(fallU1_o));
    REQUIRE(graph.property(graph.out_arc_iterator(riseU1_a)) == timing::TimingGraph::ArcProperty::TimingArc);
    REQUIRE(graph.entity(timing::TimingArcs::timing_arc_entity_type(), graph.out_arc_iterator(riseU1_a)) == timingArcU1);

    REQUIRE(graph.graph().id(graph.target(graph.out_arc_iterator(riseU1_o))) == graph.graph().id(riseU2_a));
    REQUIRE(graph.property(graph.out_arc_iterator(riseU1_o)) == timing::TimingGraph::ArcProperty::Net);
    REQUIRE(graph.entity(circuit::Net(), graph.out_arc_iterator(riseU1_o)) == netU1ToU2);
}
