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

#include <ophidian/timing/TimingGraph.h>

using namespace ophidian;
using namespace ophidian::timing;

using netlist_type = circuit::Netlist;
using pin_type     = circuit::Pin;
using net_type     = circuit::Net;
using cell_type    = circuit::Cell;

namespace
{
class TimingGraphFixture
{
public:
    netlist_type m_netlist;
    cell_type cell1, cell2;
    pin_type cell1Input, cell1Output, cell2Input, cell2Output;
    net_type net1To2;
    TimingArcs::timing_arc_entity_type cell1Arc;

    TimingGraphFixture()
    {
        cell1 = m_netlist.add(cell_type(), "cell1");
        cell1Input = m_netlist.add(pin_type(), "cell1:pinInput");
        cell1Output = m_netlist.add(pin_type(), "cell1:pinOutput");
        cell2 = m_netlist.add(cell_type(), "cell2");
        cell2Input = m_netlist.add(pin_type(), "cell2:pinInput");
        cell2Output = m_netlist.add(pin_type(), "cell2:pinOutput");
        net1To2 = m_netlist.add(net_type(), "net1To2");
        m_netlist.add(cell1, cell1Input);
        m_netlist.add(cell1, cell1Output);
        m_netlist.add(cell2, cell2Input);
        m_netlist.add(cell2, cell2Output);
        m_netlist.connect(net1To2, cell1Output);
        m_netlist.connect(net1To2, cell2Input);
    }
};
} // namespace

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph", "[timing][TimingGraph]")
{
    SECTION("TimingGraph: init", "[timing][TimingGraph]")
    {
        TimingGraph graph(m_netlist);

        REQUIRE(m_netlist.size(pin_type()) == 4);
        REQUIRE(graph.size(TimingGraph::node_type()) == 0);
        REQUIRE(graph.size(TimingGraph::arc_type()) == 0);
    }

    SECTION("TimingGraph: creating rise nodes", "[timing][TimingGraph]")
    {
        TimingGraph graph(m_netlist);

        auto no1 = graph.rise_node_create(cell1Input);
        auto no2 = graph.rise_node_create(cell1Output);
        REQUIRE(graph.size(TimingGraph::node_type()) == 2);

        REQUIRE(graph.graph().id(graph.rise_node(cell1Input)) == graph.graph().id(no1));
        REQUIRE(graph.graph().id(graph.rise_node(cell1Output)) == graph.graph().id(no2));
    }

    SECTION("TimingGraph: creating fall nodes", "[timing][TimingGraph]")
    {
        TimingGraph graph(m_netlist);

        auto no1 = graph.fall_node_create(cell1Input);
        auto no2 = graph.fall_node_create(cell1Output);
        REQUIRE(graph.size(TimingGraph::node_type()) == 2);

        REQUIRE(graph.graph().id(graph.fall_node(cell1Input)) == graph.graph().id(no1));
        REQUIRE(graph.graph().id(graph.fall_node(cell1Output)) == graph.graph().id(no2));
    }

    SECTION("TimingGraph: arc from net creating", "[timing][TimingGraph]")
    {
        TimingGraph graph(m_netlist);

        auto n1 = graph.rise_node_create(cell1Output);
        auto n2 = graph.rise_node_create(cell2Input);
        auto arc = graph.arc_create(n1, n2, net1To2);

        REQUIRE(graph.size(TimingGraph::arc_type()) == 1);
        REQUIRE(graph.property(arc) == TimingGraph::ArcProperty::Net);
        REQUIRE(graph.source(arc) == n1);
        REQUIRE(graph.target(arc) == n2);
        REQUIRE(graph.entity(net_type(), arc) == net1To2);
        REQUIRE_THROWS(graph.entity(TimingArcs::timing_arc_entity_type(), arc));
    }

    SECTION("TimingGraph: arc from timing arc creating", "[timing][TimingGraph]")
    {
        TimingGraph graph(m_netlist);

        auto n1 = graph.rise_node_create(cell1Input);
        auto n2 = graph.rise_node_create(cell1Output);
        auto arc = graph.arc_create(n1, n2, cell1Arc);

        REQUIRE(graph.size(TimingGraph::arc_type()) == 1);
        REQUIRE(graph.property(arc) == TimingGraph::ArcProperty::TimingArc);
        REQUIRE(graph.source(arc) == n1);
        REQUIRE(graph.target(arc) == n2);
        REQUIRE(graph.entity(TimingArcs::timing_arc_entity_type(), arc) == cell1Arc);
        REQUIRE_THROWS(graph.entity(net_type(), arc));
    }

    SECTION("TimingGraph: edges of a node", "[timing][TimingGraph]")
    {
        TimingGraph graph(m_netlist);

        auto n1 = graph.rise_node_create(cell1Input);
        auto n2 = graph.rise_node_create(cell1Output);
        auto n3 = graph.rise_node_create(cell2Input);
        auto timingArc = graph.arc_create(n1, n2, cell1Arc);
        auto net = graph.arc_create(n2, n3, net1To2);

        REQUIRE(graph.size(TimingGraph::arc_type()) == 2);

        const TimingGraph::graph_type & g = graph.graph();

        REQUIRE(graph.in_arc_iterator(n1) == lemon::INVALID);
        REQUIRE(g.id(graph.out_arc_iterator(n1)) == g.id(timingArc));

        REQUIRE(g.id(graph.in_arc_iterator(n2)) == g.id(timingArc));
        REQUIRE(g.id(graph.out_arc_iterator(n2)) == g.id(net));

        REQUIRE(g.id(graph.in_arc_iterator(n3)) == g.id(net));
        REQUIRE(graph.out_arc_iterator(n3) == lemon::INVALID);
    }

    SECTION("TimingGraph: removing nodes", "[timing][TimingGraph]")
    {
        TimingGraph graph(m_netlist);
        std::vector<TimingGraph::node_type> remove;

        auto n1 = graph.rise_node_create(cell1Input);
        auto n2 = graph.rise_node_create(cell1Output);
        auto n3 = graph.rise_node_create(cell2Input);
        graph.arc_create(n1, n2, cell1Arc);
        graph.arc_create(n2, n3, net1To2);

        remove.push_back(n1);
        remove.push_back(n2);
        remove.push_back(n3);

        REQUIRE(graph.size(TimingGraph::node_type()) == 3);
        REQUIRE(graph.size(TimingGraph::arc_type()) == 2);

        graph.destroy(remove.begin(), remove.end());

        REQUIRE(graph.size(TimingGraph::node_type()) == 0);
        REQUIRE(graph.size(TimingGraph::arc_type()) == 0);
    }

    SECTION("TimingGraph: removing edges", "[timing][TimingGraph]")
    {
        TimingGraph graph(m_netlist);
        std::vector<TimingGraph::arc_type> remove;

        auto n1 = graph.rise_node_create(cell1Input);
        auto n2 = graph.rise_node_create(cell1Output);
        auto n3 = graph.rise_node_create(cell2Input);
        remove.push_back(graph.arc_create(n1, n2, cell1Arc));
        remove.push_back(graph.arc_create(n2, n3, net1To2));

        REQUIRE(graph.size(TimingGraph::node_type()) == 3);
        REQUIRE(graph.size(TimingGraph::arc_type()) == 2);

        graph.destroy(remove.begin(), remove.end());

        REQUIRE(graph.size(TimingGraph::node_type()) == 3);
        REQUIRE(graph.size(TimingGraph::arc_type()) == 0);
    }
}
