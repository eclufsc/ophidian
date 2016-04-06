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

#include "generic_sta.h"

namespace ophidian {
namespace timing {

generic_sta::generic_sta(const ophidian::netlist::netlist &netlist, const ophidian::timing::library &library, const ophidian::timing::graph &graph, const entity::vector_property<ophidian::interconnection::rc_tree> &rc_trees) :
    m_netlist(netlist),
    m_library(library),
    m_graph(graph),
    m_rc_trees(rc_trees),
    m_nodes(graph.G()),
    m_arcs(graph.G()),
    m_sorted_nodes(m_graph.nodes_count()),
    m_sorted_driver_nodes(m_graph.nodes_count())
{

    using GraphType = lemon::ListDigraph;

    GraphType::NodeMap<int> order(graph.G());
    lemon::topologicalSort(graph.G(), order);


    std::vector<GraphType::Node> sorted_nodes(graph.nodes_count());

    GraphType::NodeMap<int> level(graph.G());

    for(GraphType::NodeIt it(graph.G()); it != lemon::INVALID; ++it)
    {
        level[it] = std::numeric_limits<int>::max();
        m_sorted_nodes[ order[it] ] = it;
        m_sorted_driver_nodes[ order[it] ] = it;
        if(lemon::countInArcs(graph.G(), it) == 0)
            level[it] = 0;
    }

    int num_levels = 0;
    for(auto node : m_sorted_nodes)
    {
        if(lemon::countInArcs(graph.G(), node) > 0)
        {
            int max_level = std::numeric_limits<int>::min();
            for(GraphType::InArcIt arc(graph.G(), node); arc != lemon::INVALID; ++arc)
                max_level = std::max(max_level, level[graph.edge_source(arc)]);
            level[node] = max_level + 1;
            num_levels = std::max(num_levels, max_level+1);
        }
    }

    m_levels.resize(num_levels);

    for(auto node : m_sorted_nodes)
        m_levels[level[node]].push_back(node);

    auto beg = std::remove_if(m_levels.begin(), m_levels.end(), [this](std::vector<lemon::ListDigraph::Node> & vec)->bool{
        return vec.empty();
    });
    m_levels.erase(beg, m_levels.end());


    auto begin = std::remove_if(
                m_sorted_driver_nodes.begin(),
                m_sorted_driver_nodes.end(),
                [this](GraphType::Node node)->bool {
            return m_library.pin_direction(m_netlist.pin_std_cell(m_graph.pin(node))) != standard_cell::pin_directions::OUTPUT;
});

    m_sorted_driver_nodes.erase(begin, m_sorted_driver_nodes.end());
#ifndef NDEBUG
    std::for_each(m_sorted_driver_nodes.begin(), m_sorted_driver_nodes.end(), [this](GraphType::Node node){
        assert(m_library.pin_direction(m_netlist.pin_std_cell(m_graph.pin(node))) == standard_cell::pin_directions::OUTPUT);
    });
    std::for_each(m_levels.begin(), m_levels.end(), [this](std::vector<lemon::ListDigraph::Node> & vec)->bool{
        assert(!vec.empty());
    });
#endif


}

void generic_sta::set_constraints(const design_constraints &dc)
{

    using namespace boost::units;
    using namespace boost::units::si;
    for(auto & i : dc.input_delays)
    {
        auto pin = m_netlist.pin_by_name(i.port_name);
        m_nodes.arrival( m_graph.rise_node(pin), quantity<si::time>(i.delay*pico*seconds) );
        m_nodes.arrival( m_graph.fall_node(pin), quantity<si::time>(i.delay*pico*seconds) );
    }

    for(auto & i : dc.input_drivers)
    {
        auto pin = m_netlist.pin_by_name(i.port_name);
        m_nodes.slew( m_graph.rise_node(pin), quantity<si::time>(i.slew_rise*pico*seconds) );
        m_nodes.slew( m_graph.fall_node(pin), quantity<si::time>(i.slew_fall*pico*seconds) );
    }


    for(lemon::ListDigraph::NodeIt node(m_graph.G()); node != lemon::INVALID; ++node)
    {
        if(lemon::countOutArcs(m_graph.G(), node) == 0)
            m_nodes.required( node, quantity<si::time>(dc.clock.period * pico* seconds) );
    }

}


}
}
