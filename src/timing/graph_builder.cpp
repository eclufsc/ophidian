/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
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

    // create a node for each pin
    for (auto pin : netlist.pin_system())
        util::transitions<graph::node> node { graph.rise_node_create(pin.first), graph.fall_node_create(pin.first) };

    // create net edges
    std::vector< entity::entity > net_pins;
    for (auto net : netlist.net_system()) {
        net_pins = netlist.net_pins(net.first);
        entity::entity source;
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
        assert(!(source == entity::entity { }));
        for (auto pin : net_pins) {
            if (pin == source)
                continue;
            graph.edge_create(graph.rise_node(source), graph.rise_node(pin), edge_types::NET, net.first);
            graph.edge_create(graph.fall_node(source), graph.fall_node(pin), edge_types::NET, net.first);
        }
    }

    // create timing arc edges


    std::vector< entity::entity > input_pins;
    std::unordered_map< entity::entity, entity::entity > output_pins;
    std::vector< entity::entity > arcs;
    for (auto cell : netlist.cell_system()) {
        auto cell_pins = netlist.cell_pins(cell.first);
        input_pins.resize(0);
        output_pins.clear();

        for(auto pin : cell_pins)
        {
            standard_cell::pin_directions direction = lib.pin_direction(netlist.pin_std_cell(pin));
            switch(direction)
            {
            case standard_cell::pin_directions::INPUT:
                input_pins.push_back(pin);
                break;
            case standard_cell::pin_directions::OUTPUT:
                output_pins[ netlist.pin_std_cell(pin) ] = pin;
                break;
            default:
                break;
            }
        }

        for (auto from : input_pins) {

            entity::entity from_std_cell = netlist.pin_std_cell(from);
            arcs = lib.pin_timing_arcs(from_std_cell);
            for(auto arc : arcs)
            {
                auto result = output_pins.find(lib.timing_arc_to(arc));
                if(result == output_pins.end()) continue;
                auto to = result->second;
                switch (lib.timing_arc_timing_sense(arc)) {
                case unateness::POSITIVE_UNATE:
                    graph.edge_create(graph.rise_node(from), graph.rise_node(to), edge_types::TIMING_ARC, arc);
                    graph.edge_create(graph.fall_node(from), graph.fall_node(to), edge_types::TIMING_ARC, arc);
                    break;
                case unateness::NON_UNATE:
                    graph.edge_create(graph.rise_node(from), graph.rise_node(to), edge_types::TIMING_ARC, arc);
                    graph.edge_create(graph.fall_node(from), graph.fall_node(to), edge_types::TIMING_ARC, arc);
                    graph.edge_create(graph.rise_node(from), graph.fall_node(to), edge_types::TIMING_ARC, arc);
                    graph.edge_create(graph.fall_node(from), graph.rise_node(to), edge_types::TIMING_ARC, arc);
                    break;
                case unateness::NEGATIVE_UNATE:
                default:
                    graph.edge_create(graph.rise_node(from), graph.fall_node(to), edge_types::TIMING_ARC, arc);
                    graph.edge_create(graph.fall_node(from), graph.rise_node(to), edge_types::TIMING_ARC, arc);
                    break;
                }
            }
        }
    }

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

        arcs = lib.pin_timing_arcs(lib.pin_create(lib.cell_create(dc.input_drivers.at(i).lib_cell), dc.input_drivers.at(i).pin_name));
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

