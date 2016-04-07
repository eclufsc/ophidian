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

#include <omp.h>

#ifndef GENERIC_STA_H
#define GENERIC_STA_H

namespace ophidian {
namespace timing {

template <class WireDelayModel, class MergeStrategy>
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
    std::vector< std::vector<lemon::ListDigraph::Node> > m_levels;
    std::vector<lemon::ListDigraph::Node> m_sorted_driver_nodes;

    MergeStrategy m_merge;

    SlewType compute_slew(lemon::ListDigraph::Node node, CapacitanceType load) const {
        SlewType worst_slew = MergeStrategy::best();
        if(lemon::countInArcs(m_graph.G(), node) == 0) // PI without driver
            return m_nodes.slew(node);
        switch(m_graph.node_edge(node))
        {
        case edges::RISE:
            for(lemon::ListDigraph::InArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = m_graph.edge_entity(it) ;
                worst_slew = m_merge(worst_slew, m_library.timing_arc_rise_slew(tarc).compute(load, m_nodes.slew(m_graph.edge_source(it))));
            }
            break;
        case edges::FALL:
            for(lemon::ListDigraph::InArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = m_graph.edge_entity(it) ;
                worst_slew = m_merge(worst_slew, m_library.timing_arc_fall_slew(tarc).compute(load, m_nodes.slew(m_graph.edge_source(it))));
            }
            break;
        }
        return worst_slew;
    }

public:
    generic_sta(const netlist::netlist & netlist, const timing::library & library, const timing::graph & graph, const entity::vector_property< interconnection::rc_tree > & rc_trees) :
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

        m_levels.resize(num_levels+1);

        for(auto node : m_sorted_nodes)
        {
            if(m_library.pin_direction(m_netlist.pin_std_cell(m_graph.pin(node))) == standard_cell::pin_directions::OUTPUT)
                m_levels[level[node]].push_back(node);
        }

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

    void set_constraints(const design_constraints & dc)
    {

        using namespace boost::units;
        using namespace boost::units::si;

        m_nodes.arrival( m_graph.rise_node(m_netlist.pin_by_name(dc.clock.port_name)), 0.0*seconds );
        m_nodes.arrival( m_graph.fall_node(m_netlist.pin_by_name(dc.clock.port_name)), 0.0*seconds );

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


            if(m_library.pin_clock_input(m_graph.pin(node)))
                m_nodes.required( node, MergeStrategy::worst() );
            else if(lemon::countOutArcs(m_graph.G(), node) == 0 )
                m_nodes.required( node, m_merge(quantity<si::time>(0.0*seconds), quantity<si::time>(dc.clock.period * pico* seconds)) );
        }

    }


    SlewType rise_arrival(const entity::entity pin) const
    {
        return m_nodes.arrival(m_graph.rise_node(pin));
    }
    SlewType fall_arrival(const entity::entity pin) const
    {
        return m_nodes.arrival(m_graph.fall_node(pin));
    }

    SlewType rise_slew(const entity::entity pin) const
    {
        return m_nodes.slew(m_graph.rise_node(pin));
    }
    SlewType fall_slew(const entity::entity pin) const
    {
        return m_nodes.slew(m_graph.fall_node(pin));
    }

    SlewType rise_slack(const entity::entity pin) const
    {
        auto node = m_graph.rise_node(pin);
        return MergeStrategy::slack_signal()*(m_nodes.required(node)-m_nodes.arrival(node));
    }
    SlewType fall_slack(const entity::entity pin) const
    {
        auto node = m_graph.fall_node(pin);
        return MergeStrategy::slack_signal()*(m_nodes.required(node)-m_nodes.arrival(node));
    }

    void run() {
        {
        for(auto & level : m_levels)
        {
            std::size_t i;
            for(i = 0; i < level.size(); ++i) // paralell
            {
                auto node = level[i];
                if(lemon::countInArcs(m_graph.G(), node) != 0)
                {
                    auto pin = m_graph.pin(node);
                    std::cout << "processing pin " << m_netlist.pin_name(pin) << std::endl;
                    auto net = m_netlist.pin_net(pin);
                    auto tree = m_rc_trees[m_netlist.net_system().lookup(net)];
                    lemon::ListGraph::NodeMap< SlewType > slews(tree.graph());
                    lemon::ListGraph::NodeMap< SlewType > delays(tree.graph());
                    lemon::ListGraph::NodeMap< CapacitanceType > ceffs(tree.graph());
                    WireDelayModel calculator;
                    calculator.delay_map(delays);
                    calculator.slew_map(slews);
                    calculator.ceff_map(ceffs);
                    std::function<SlewType(CapacitanceType)> s_calculator = std::bind(&generic_sta::compute_slew, this, node, std::placeholders::_1);
                    auto source_capacitor = tree.capacitor_by_name(m_netlist.pin_name(pin));
                    CapacitanceType load = calculator.simulate(s_calculator, tree, source_capacitor);

                    m_nodes.load(node, load);
                    m_nodes.slew(node, slews[source_capacitor]);

                    SlewType worst_arrival = MergeStrategy::best();
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
                            worst_arrival = m_merge(worst_arrival, m_nodes.arrival(edge_source) + arc_delay);
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
                            worst_arrival = m_merge(worst_arrival, m_nodes.arrival(edge_source) + arc_delay);
                        }
                        break;
                    }
                    m_nodes.arrival(node, worst_arrival);

                    for(lemon::ListDigraph::OutArcIt arc(m_graph.G(), node); arc != lemon::INVALID; ++arc)
                    {
                        auto arc_target = m_graph.edge_target(arc);
                        auto target_pin = m_graph.pin(arc_target);

                        std::cout << "    setting to " << m_netlist.pin_name(target_pin) << " " << (m_graph.node_edge(arc_target)==edges::RISE?"RISE":"FALL") << " the arrival and slew " << std::endl;

                        auto target_capacitor = tree.capacitor_by_name(m_netlist.pin_name(target_pin));
                        m_arcs.slew(arc, slews[target_capacitor]);
                        m_nodes.slew(arc_target, m_arcs.slew(arc));
                        m_nodes.arrival(arc_target, m_nodes.arrival(node) + m_arcs.delay(arc));
                    }
                }
            }
        }

        }

        for(auto node_it = m_sorted_nodes.rbegin(); node_it != m_sorted_nodes.rend(); ++node_it)
        {
            auto node = *node_it;
            if(lemon::countOutArcs(m_graph.G(), node) > 0)
            {
                SlewType required = MergeStrategy::worst();
                for(lemon::ListDigraph::OutArcIt arc(m_graph.G(), node); arc != lemon::INVALID; ++arc)
                    required = m_merge.inverted(required, m_nodes.required(m_graph.edge_target(arc))-m_arcs.delay(arc));
                m_nodes.required(node, required);
            }
            std::cout << m_netlist.pin_name( m_graph.pin(node) ) << " " << (m_graph.node_edge(node)==edges::RISE?"RISE":"FALL") << " required = " << m_nodes.required(node) << std::endl;
        }
    }


};

}
}


#endif // GENERIC_STA_H

