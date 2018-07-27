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

using time_unit_type = timing::TimingData::time_unit_type;
using capacitance_unit_type = timing::TimingData::capacitance_unit_type;

namespace
{
class TimingDataFixture
{
public:
    design::ICCAD2015ContestDesignBuilder m_builder;
    design::Design & m_design;

    std::shared_ptr<ophidian::parser::Liberty> m_liberty;
    timing::TimingArcs m_timing_arcs;
    timing::Library m_timing_library;
    std::shared_ptr<parser::DesignConstraints> m_dc;
    std::unique_ptr<parser::Lef> m_lef;

    timing::TimingGraph m_graph;

    timing::TimingGraph::node_type m_from, m_to;
    timing::TimingGraph::arc_type m_arc;

    TimingDataFixture() :
        m_builder("./input_files/simple/simple.lef",
                 "./input_files/simple/simple.def",
                 "./input_files/simple/simple.v",
                 "./input_files/simple/simple_Early.lib",
                 "./input_files/simple/simple_Late.lib",
                 "./input_files/simple/simple.sdc"),
        m_design(m_builder.build()),
        m_liberty(parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        m_timing_arcs(m_design.standardCells()),
        m_timing_library(m_design.standardCells(), m_timing_arcs),
        m_dc(parser::SDCSimple().constraints()),
        m_graph(m_design.netlist())
    {
        m_timing_library.init(*m_liberty, true),
        timing::TimingGraphBuilder().build(m_design.netlist(),
                                           m_design.standardCells(),
                                           m_design.libraryMapping(),
                                           m_timing_arcs,
                                           m_timing_library,
                                           *m_dc, m_graph);
        m_from = m_graph.rise_node(m_design.netlist().find(circuit::Pin(), "u1:o"));
        m_to = m_graph.rise_node(m_design.netlist().find(circuit::Pin(), "u2:a"));

        m_arc = m_graph.out_arc_iterator(m_from);
    }
};
} // namespace

TEST_CASE_METHOD(TimingDataFixture, "TimingGraph Condensation","[timing][sta][condensation]")
{
    SECTION("TimingGraph Condensation: Values of nodes after creation","[timing][sta][condensation]")
    {
        timing::TimingData data(m_timing_library, m_graph);
        data.arrival(m_from, time_unit_type(1.0));
        data.slew(m_from, time_unit_type(3.0));
        data.required(m_from, time_unit_type(5.0));
        data.load(m_from, capacitance_unit_type(6.0));

        REQUIRE(data.arrival(m_from) == time_unit_type(1.0));
        REQUIRE(data.slew(m_from) == time_unit_type(3.0));
        REQUIRE(data.required(m_from) == time_unit_type(5.0));
        REQUIRE(data.load(m_from) == capacitance_unit_type(6.0));
    }

    SECTION("TimingGraph Condensation: Values of arcs after creation", "[timing][sta][condensation]")
    {
        timing::TimingData data(m_timing_library, m_graph);
        data.delay(m_arc, time_unit_type(10.0));
        data.slew(m_arc, time_unit_type(12.0));

        REQUIRE(data.delay(m_arc) == time_unit_type(10.0));
        REQUIRE(data.slew(m_arc) == time_unit_type(12.0));
    }
}
