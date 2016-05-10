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

#include "graph_builder.h"

namespace ophidian {
namespace timing {

namespace util {
template<class T>
struct transitions {
    T rise;
    T fall;
};
}

void graph_builder::build(const netlist::netlist & netlist, library & lib, const timing::design_constraints & dc, graph& graph) {

    std::cout << "  graph_builder::build(): creating two nodes for each pin" << std::endl << std::flush;
    // create a node for each pin
    for (auto pin : netlist.pin_system())
        util::transitions<graph::node> node { graph.rise_node_create(pin), graph.fall_node_create(pin) };

    std::cout << "  graph_builder::build(): creating net edges" << std::endl << std::flush;
    // create net edges
    for (auto net : netlist.net_system()) {
        auto net_pins = netlist.net_pins(net);
        entity_system::entity source;
        for (auto pin : net_pins) {
            std::string pin_name = netlist.pin_name(pin);
            standard_cell::pin_directions direction = lib.pin_direction(netlist.pin_std_cell(pin));
            switch (direction) {
            case standard_cell::pin_directions::INPUT:
                break;
            case standard_cell::pin_directions::OUTPUT:
            default:
                source = pin;
                break;
            }
        }
        assert(!(source == entity_system::invalid_entity));
        for (auto pin : net_pins) {
            if (pin == source)
                continue;
            graph.edge_create(graph.rise_node(source), graph.rise_node(pin), edge_types::NET, net);
            graph.edge_create(graph.fall_node(source), graph.fall_node(pin), edge_types::NET, net);
        }
    }

    // create timing arc edges

    std::cout << "  graph_builder::build(): creating timing arc edges" << std::endl << std::flush;
    std::vector< entity_system::entity > input_pins;
    std::unordered_map< entity_system::entity, entity_system::entity, entity_system::entity_hash > output_pins;
    std::size_t current_cell{0};

    std::vector<double> percentages {
        0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0
    };
    for (auto cell : netlist.cell_system()) {
        ++current_cell;

        for(auto p : percentages)
        {
            if(static_cast<std::size_t>(p*netlist.cell_system().size()) == current_cell)
                std::cout << "       graph_builder::build(): ["<<100*p<<"%]" << std::endl << std::flush;
        }
        auto cell_pins = netlist.cell_pins(cell);
//        std::cout << "  graph_builder::build(): creating timing arcs for cell " << netlist.cell_name(cell.first) << std::endl << std::endl << std::flush;
        input_pins.resize(0);
        output_pins.clear();

        entity_system::entity data_pin, clk_pin;

        for(auto pin : cell_pins)
        {
            standard_cell::pin_directions direction = lib.pin_direction(netlist.pin_std_cell(pin));
            switch(direction)
            {
            case standard_cell::pin_directions::INPUT:
                input_pins.push_back(pin);

                if(lib.pin_clock_input(netlist.pin_std_cell(pin)))
                    clk_pin = pin;
                else if(lib.cell_sequential(netlist.cell_std_cell(netlist.pin_owner(pin))))
                    data_pin = pin;

                break;
            case standard_cell::pin_directions::OUTPUT:
                output_pins[ netlist.pin_std_cell(pin) ] = pin;
                break;
            default:
                break;
            }


        }

        bool test_created = false;
        for (auto from : input_pins) {

            entity_system::entity from_std_cell = netlist.pin_std_cell(from);
            auto arcs = lib.pin_timing_arcs(from_std_cell);
            for(auto arc : arcs)
            {
                if(lib.timing_arc_timing_type(arc) == timing_arc_types::SEQUENTIAL && !test_created)
                {
                    graph.test_insert(graph.rise_node(clk_pin), graph.rise_node(data_pin), arc);
                    graph.test_insert(graph.fall_node(clk_pin), graph.fall_node(data_pin), arc);
                    test_created = true;
                }
                else
                {
                    auto result = output_pins.find(lib.timing_arc_to(arc));
                    if(result == output_pins.end()) continue;
                    auto to = result->second;
                    switch (lib.timing_arc_timing_sense(arc)) {
                    case unateness::POSITIVE_UNATE:
                        graph.edge_create(graph.rise_node(from), graph.rise_node(to), edge_types::TIMING_ARC, arc);
                        if(lib.timing_arc_timing_type(arc) != timing_arc_types::RISING_EDGE)
                            graph.edge_create(graph.fall_node(from), graph.fall_node(to), edge_types::TIMING_ARC, arc);
                        break;
                    case unateness::NON_UNATE:
                        graph.edge_create(graph.rise_node(from), graph.rise_node(to), edge_types::TIMING_ARC, arc);
                        if(lib.timing_arc_timing_type(arc) != timing_arc_types::RISING_EDGE)
                            graph.edge_create(graph.fall_node(from), graph.fall_node(to), edge_types::TIMING_ARC, arc);
                        graph.edge_create(graph.rise_node(from), graph.fall_node(to), edge_types::TIMING_ARC, arc);
                        if(lib.timing_arc_timing_type(arc) != timing_arc_types::RISING_EDGE)
                            graph.edge_create(graph.fall_node(from), graph.rise_node(to), edge_types::TIMING_ARC, arc);
                        break;
                    case unateness::NEGATIVE_UNATE:
                    default:
                        graph.edge_create(graph.rise_node(from), graph.fall_node(to), edge_types::TIMING_ARC, arc);
                        if(lib.timing_arc_timing_type(arc) != timing_arc_types::RISING_EDGE)
                            graph.edge_create(graph.fall_node(from), graph.rise_node(to), edge_types::TIMING_ARC, arc);
                        break;
                    }
                }
            }
        }
    }

    std::cout << "  graph_builder::build(): creating input drivers edges" << std::endl << std::endl << std::flush;
    std::vector<lemon::ListDigraph::Arc> out_rise_arcs, out_fall_arcs;
    for (std::size_t i = 0; i < dc.input_drivers.size(); ++i) {
        out_rise_arcs.resize(0);
        out_fall_arcs.resize(0);
        auto PI_pin = netlist.pin_by_name(dc.input_drivers.at(i).port_name);
        auto rise_node = graph.rise_node(PI_pin);
        auto fall_node = graph.fall_node(PI_pin);
        auto new_rise_node = graph.rise_node_create(PI_pin);
        auto new_fall_node = graph.fall_node_create(PI_pin);

        for (lemon::ListDigraph::OutArcIt out(graph.G(), rise_node); out != lemon::INVALID; ++out)
            out_rise_arcs.push_back(out);
        for (lemon::ListDigraph::OutArcIt out(graph.G(), fall_node); out != lemon::INVALID; ++out)
            out_fall_arcs.push_back(out);

        for (auto arc : out_rise_arcs)
            graph.edge_source(arc, new_rise_node);
        for (auto arc : out_fall_arcs)
            graph.edge_source(arc, new_fall_node);

        auto arcs = lib.pin_timing_arcs(lib.pin_create(lib.cell_create(dc.input_drivers.at(i).lib_cell), dc.input_drivers.at(i).pin_name));
        for (auto arc : arcs) {
            switch (lib.timing_arc_timing_sense(arc)) {
            case unateness::POSITIVE_UNATE:
                graph.edge_create(rise_node, new_rise_node, edge_types::TIMING_ARC, arc);
                graph.edge_create(fall_node, new_fall_node, edge_types::TIMING_ARC, arc);
                break;
            case unateness::NON_UNATE:
                graph.edge_create(rise_node, new_rise_node, edge_types::TIMING_ARC, arc);
                graph.edge_create(fall_node, new_fall_node, edge_types::TIMING_ARC, arc);
                graph.edge_create(rise_node, new_fall_node, edge_types::TIMING_ARC, arc);
                graph.edge_create(fall_node, new_rise_node, edge_types::TIMING_ARC, arc);
                break;
            case unateness::NEGATIVE_UNATE:
            default:
                graph.edge_create(rise_node, new_fall_node, edge_types::TIMING_ARC, arc);
                graph.edge_create(fall_node, new_rise_node, edge_types::TIMING_ARC, arc);
                break;
            }
        }
    }

    //	for (lemon::ListDigraph::ArcIt it(graph.G()); it != lemon::INVALID; ++it) {
    //		auto source_node = graph.edge_source(it);
    //		auto target_node = graph.edge_target(it);
    //		auto source_pin = graph.pin(source_node);
    //		auto target_pin = graph.pin(target_node);
    //		std::string source_type = "r";
    //		std::string target_type = "r";
    //		if (graph.node_edge(source_node) == edges::FALL)
    //			source_type = "f";
    //		if (graph.node_edge(target_node) == edges::FALL)
    //			target_type = "f";
    //		std::cout << netlist.pin_name(source_pin) << " " << source_type <<
    //		" -> " << netlist.pin_name(target_pin) << " " << target_type << std::endl;

    //	}

}
} /* namespace timing */
} /* namespace ophidian */

