/*
 * Copyright 2016 Ophidian
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

#include "generic_sta.h"

namespace ophidian {
namespace timing {

void test_calculator:: compute_tests()
{
    auto tests = topology.g.tests();
    std::size_t i;
    using DelayType = boost::units::quantity< boost::units::si::time >;
    for(i = 0; i < tests.size(); ++i) // parallel
    {
        const test & t = tests[i];
        auto pin_ck = topology.g.pin(t.ck);
        auto pin_d = topology.g.pin(t.d);
        DelayType setup, hold;
        switch(topology.g.node_edge(t.d))
        {
        case edges::RISE:
            setup = late.lib.setup_rise(t.tarc).compute(early.nodes.slew(t.ck), late.nodes.slew(t.d));
            hold = early.lib.hold_rise(t.tarc).compute(late.nodes.slew(t.ck), early.nodes.slew(t.d));
            break;
        case edges::FALL:
            setup = late.lib.setup_fall(t.tarc).compute(early.nodes.slew(t.ck), late.nodes.slew(t.d));
            hold = early.lib.hold_fall(t.tarc).compute(late.nodes.slew(t.ck), early.nodes.slew(t.d));
            break;
        }
        const DelayType e_ck = early.nodes.arrival(topology.g.rise_node(pin_ck));
        const DelayType l_ck = late.nodes.arrival(topology.g.rise_node(pin_ck));
        late.nodes.required(t.d, e_ck-setup+clock_period);
        early.nodes.required(t.d, l_ck+hold);

    }
}

graph_and_topology::graph_and_topology(const graph &G, const netlist::netlist &netlist, const library &lib):
    g(G),
    netlist(netlist),
    sorted(g.nodes_count()),
    sorted_drivers(g.nodes_count()){

    using GraphType = lemon::ListDigraph;

    GraphType::NodeMap<int> order(g.G());
    lemon::topologicalSort(g.G(), order);


    std::vector<GraphType::Node> sorted_nodes(g.nodes_count());

    GraphType::NodeMap<int> level(g.G());

    for(GraphType::NodeIt it(g.G()); it != lemon::INVALID; ++it)
    {
        level[it] = std::numeric_limits<int>::max();
        sorted[ order[it] ] = it;
        sorted_drivers[ order[it] ] = it;
        if(lemon::countInArcs(g.G(), it) == 0)
            level[it] = 0;
    }

    int num_levels = 0;
    for(auto node : sorted)
    {
        if(lemon::countInArcs(g.G(), node) > 0)
        {
            int max_level = std::numeric_limits<int>::min();
            for(GraphType::InArcIt arc(g.G(), node); arc != lemon::INVALID; ++arc)
                max_level = std::max(max_level, level[g.edge_source(arc)]);
            level[node] = max_level + 1;
            num_levels = std::max(num_levels, max_level+1);
        }
    }

    levels.resize(num_levels+1);

    for(auto node : sorted)
    {
        if(lib.pin_direction(netlist.pin_std_cell(g.pin(node))) == standard_cell::pin_directions::OUTPUT)
            levels[level[node]].push_back(node);
    }

    auto beg = std::remove_if(levels.begin(), levels.end(), [this](std::vector<lemon::ListDigraph::Node> & vec)->bool{
            return vec.empty();
});
    levels.erase(beg, levels.end());


    auto begin = std::remove_if(
                sorted_drivers.begin(),
                sorted_drivers.end(),
                [this, lib, netlist](GraphType::Node node)->bool {
            return lib.pin_direction(netlist.pin_std_cell(g.pin(node))) != standard_cell::pin_directions::OUTPUT;
});

    sorted_drivers.erase(begin, sorted_drivers.end());
#ifndef NDEBUG
    std::for_each(sorted_drivers.begin(), sorted_drivers.end(), [this, lib, netlist](GraphType::Node node){
        assert(lib.pin_direction(netlist.pin_std_cell(g.pin(node))) == standard_cell::pin_directions::OUTPUT);
    });
    std::for_each(levels.begin(), levels.end(), [this](std::vector<lemon::ListDigraph::Node> & vec)->bool{
        assert(!vec.empty());
    });
#endif
}

}
}
