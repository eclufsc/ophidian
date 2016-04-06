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

#include "../timing/library.h"
#include "../timing/graph.h"
#include "../netlist/netlist.h"
#include "../interconnection/rc_tree.h"

#include <lemon/connectivity.h>

#include <boost/units/limits.hpp>
#include <boost/units/cmath.hpp>
#include <functional>

#include "ceff.h"
#include "design_constraints.h"

#include "graph_nodes_timing.h"
#include "graph_arcs_timing.h"

#ifndef GENERIC_STA_H
#define GENERIC_STA_H

namespace ophidian {
namespace timing {

class generic_sta
{
    const netlist::netlist & m_netlist;
    const timing::library & m_library;
    const timing::graph & m_graph;
    const entity::vector_property< interconnection::rc_tree > & m_rc_trees;

    using SlewType = boost::units::quantity< boost::units::si::time >;
    using CapacitanceType = boost::units::quantity< boost::units::si::capacitance >;


    graph_nodes_timing m_nodes;
    graph_arcs_timing m_arcs;

    std::vector<lemon::ListDigraph::Node> m_sorted_nodes;

    SlewType compute_slew(lemon::ListDigraph::Node node, CapacitanceType load) const {
        SlewType worst_slew = std::numeric_limits<SlewType>::lowest();
        if(lemon::countInArcs(m_graph.G(), node) == 0) // PI without driver
            return m_nodes.slew(node);
        switch(m_graph.node_edge(node))
        {
        case edges::RISE:
            for(lemon::ListDigraph::InArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = m_graph.edge_entity(it) ;
                worst_slew = std::max(worst_slew, m_library.timing_arc_rise_slew(tarc).compute(load, m_nodes.slew(m_graph.edge_source(it))));
            }
            break;
        case edges::FALL:
            for(lemon::ListDigraph::InArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = m_graph.edge_entity(it) ;
                worst_slew = std::max(worst_slew, m_library.timing_arc_fall_slew(tarc).compute(load, m_nodes.slew(m_graph.edge_source(it))));
            }
            break;
        }
        return worst_slew;
    }

public:
    generic_sta(const netlist::netlist & netlist, const timing::library & library, const timing::graph & graph, const entity::vector_property< interconnection::rc_tree > & rc_trees);

    void set_constraints(const design_constraints & dc);

    template <class WireDelayModel>
    void run(WireDelayModel calculator) {

        for(auto node : m_sorted_nodes)
        {
            if(lemon::countInArcs(m_graph.G(), node) == 0) continue;

            auto pin = m_graph.pin(node);
            std::cout << "updating pin " << m_netlist.pin_name(pin) << std::endl;

            auto net = m_netlist.pin_net(pin);
            auto tree = m_rc_trees[m_netlist.net_system().lookup(net)];

            lemon::ListGraph::NodeMap< SlewType > slews(tree.graph());
            lemon::ListGraph::NodeMap< SlewType > delays(tree.graph());
            lemon::ListGraph::NodeMap< CapacitanceType > ceffs(tree.graph());

            calculator.delay_map(delays);
            calculator.slew_map(slews);
            calculator.ceff_map(ceffs);

            std::function<SlewType(CapacitanceType)> s_calculator = std::bind(&generic_sta::compute_slew, this, node, std::placeholders::_1);

            auto source_capacitor = tree.capacitor_by_name(m_netlist.pin_name(pin));
            CapacitanceType load = calculator.simulate(s_calculator, tree, source_capacitor);

            m_nodes.slew(node, slews[source_capacitor]);

            SlewType worst_arrival = std::numeric_limits<SlewType>::lowest();
            switch(m_graph.node_edge(node))
            {
            case edges::RISE:
                for(lemon::ListDigraph::InArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it)
                {
                    auto tarc = m_graph.edge_entity(it) ;
                    auto edge_source = m_graph.edge_source(it);
                    auto arc_delay = m_library.timing_arc_rise_delay(tarc).compute(load, m_nodes.slew(edge_source));
                    auto arc_slew = m_library.timing_arc_rise_slew(tarc).compute(load, m_nodes.slew(edge_source));
                    m_arcs.delay(it, arc_delay);
                    m_arcs.slew(it, arc_slew);
                    worst_arrival = std::max(worst_arrival, m_nodes.arrival(edge_source) + arc_delay);
                }
                break;
            case edges::FALL:
                for(lemon::ListDigraph::InArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it)
                {
                    auto tarc = m_graph.edge_entity(it) ;
                    auto edge_source = m_graph.edge_source(it);
                    auto arc_delay = m_library.timing_arc_fall_delay(tarc).compute(load, m_nodes.slew(edge_source));
                    auto arc_slew = m_library.timing_arc_fall_slew(tarc).compute(load, m_nodes.slew(edge_source));
                    m_arcs.delay(it, arc_delay);
                    m_arcs.slew(it, arc_slew);
                    worst_arrival = std::max(worst_arrival, m_nodes.arrival(edge_source) + arc_delay);
                }
                break;
            }
            m_nodes.arrival(node, worst_arrival);

            for(lemon::ListDigraph::OutArcIt arc(m_graph.G(), node); arc != lemon::INVALID; ++arc)
            {
                auto arc_target = m_graph.edge_target(arc);
                auto target_pin = m_graph.pin(arc_target);
                auto target_capacitor = tree.capacitor_by_name(m_netlist.pin_name(target_pin));
                m_nodes.slew(arc_target, slews[target_capacitor]);
                m_nodes.arrival(arc_target, m_nodes.arrival(node) + delays[target_capacitor]);
            }
        }

        for(auto pin : m_netlist.pin_system())
        {
            std::cout << "pin " << m_netlist.pin_name(pin.first) << " ";
            std::cout << "rat " << m_nodes.arrival(m_graph.rise_node(pin.first)) << " ";
            std::cout << "fat " << m_nodes.arrival(m_graph.fall_node(pin.first)) << " ";
            std::cout << "rslew " << m_nodes.slew(m_graph.rise_node(pin.first)) << " ";
            std::cout << "fslew " << m_nodes.slew(m_graph.fall_node(pin.first)) << " ";
            std::cout << std::endl;
        }
    }


};

}
}


#endif // GENERIC_STA_H

