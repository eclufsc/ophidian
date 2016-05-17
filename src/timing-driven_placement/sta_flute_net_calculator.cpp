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

#include "sta_flute_net_calculator.h"
#include "../timing/transition.h"
#include <map>
#include <set>

#include <boost/units/pow.hpp>
#include <boost/units/cmath.hpp>

namespace ophidian {
namespace timingdriven_placement {

sta_flute_net_calculator::sta_flute_net_calculator(const timing::graph & graph, const placement::placement & placement, const timing::library &timing_lib, netlist::netlist & netlist) :
    m_graph(graph), m_placement(placement),
    m_timing_lib(timing_lib) {
    netlist.register_net_property(&m_net_sources);


    for(auto net_it : netlist.net_system())
    {
        entity_system::entity net = net_it;
        auto net_pins = netlist.net_pins(net);
        bool source_found = false;
        for(auto pin : net_pins)
        {
            switch( timing_lib.pin_direction(netlist.pin_std_cell(pin)) )
            {
            case standard_cell::pin_directions::INPUT:
                break;
            case standard_cell::pin_directions::OUTPUT:
                m_net_sources[netlist.net_system().lookup(net)] = pin;
                source_found = true;
                break;
            case standard_cell::pin_directions::NOT_ASSIGNED:
            default:
                assert(false);
                break;
            }
            if(source_found)
                break;
        }
    }
}

sta_flute_net_calculator::~sta_flute_net_calculator() {
}

void sta_flute_net_calculator::update_net(timing::sta_timing_net_edge_calculator *tnet, entity_system::entity net, timing::graph_nodes_timing & nodes_timing)
{
    interconnection::rc_tree tree;
    auto tap_map = m_flute.create_tree(m_placement, net, tree, m_timing_lib);
    entity_system::entity source_pin = m_net_sources[m_placement.netlist().net_system().lookup(net)];

    nodes_timing.load(m_graph.rise_node(source_pin), tree.lumped());
    nodes_timing.load(m_graph.fall_node(source_pin), tree.lumped());

    timing::elmore delay(tree, tap_map.at(source_pin));
    timing::elmore_second_moment second(tree, delay);
    for( auto tap : tap_map )
    {
        if(tap.first == source_pin) continue;
        tnet->elmore_delay( m_graph.rise_node(tap.first), delay.at(tap.second) );
        tnet->elmore_delay( m_graph.fall_node(tap.first), delay.at(tap.second) );
        auto elmore_slew = boost::units::sqrt( second.at(tap.second)+second.at(tap.second) - boost::units::pow<2>(delay.at(tap.second)) );
        tnet->elmore_slew( m_graph.rise_node(tap.first), elmore_slew );
        tnet->elmore_slew( m_graph.fall_node(tap.first), elmore_slew );
    }
}



} /* namespace timing */
} /* namespace ophidian */
