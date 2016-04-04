/*
 * static_timing_analysis.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#include "static_timing_analysis.h"

#include <lemon/connectivity.h>

namespace openeda {
namespace timing {

static_timing_analysis::static_timing_analysis(const graph & g, const library & lib, sta_interconnection_estimator * interconnection_estimator) :
    m_graph(g),
    m_library(lib),
    m_nodes(g.G()),
    m_arcs(g.G()),
    m_interconnection_estimator(interconnection_estimator),
    m_tarcs(lib),
    m_tnets(g),
    m_arcs_calculators(g.G()),
    m_topological_sorted_nodes(g.nodes_count())
{
    for (lemon::ListDigraph::ArcIt it(g.G()); it != lemon::INVALID; ++it) {
        switch (g.edge_type(static_cast<timing::graph::edge>(it))) {
        case timing::edge_types::NET:
            m_arcs_calculators[it] = &m_tnets;
            break;
        case timing::edge_types::TIMING_ARC:
            m_arcs_calculators[it] = &m_tarcs;
            break;
        }
    }

    lemon::ListDigraph::NodeMap<uint32_t> sort(g.G());
    lemon::topologicalSort(g.G(), sort);
    for(lemon::ListDigraph::NodeIt node(g.G()); node != lemon::INVALID; ++node)
        m_topological_sorted_nodes[ sort[node] ] = node;
}

static_timing_analysis::~static_timing_analysis() {
}

void static_timing_analysis::make_dirty(entity::entity net) {
    if(m_dirty.insert(net).second)
        m_dirty_nets.push_back(net);
}

void static_timing_analysis::run() {
#pragma omp parallel for
    for(std::size_t i = 0; i < m_dirty_nets.size(); ++i)
        m_interconnection_estimator->update_net(&m_tnets, m_dirty_nets[i], m_nodes);
    m_dirty_nets.clear();
    m_dirty.clear();
    for(auto node : m_topological_sorted_nodes)
    {
        boost::units::quantity<boost::units::si::time> arrival(std::numeric_limits<double>::lowest() * boost::units::si::second);
        boost::units::quantity<boost::units::si::time> slew(std::numeric_limits<double>::lowest() *boost::units:: si::second);
        if( lemon::countInArcs(m_graph.G(), node) >= 1 )
        {
            for (lemon::ListDigraph::InArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it) {
                lemon::ListDigraph::Node from(m_graph.edge_source(static_cast<lemon::ListDigraph::Arc>(it)));
                arrival = std::max(arrival, m_nodes.arrival(from) + m_arcs.delay(static_cast<lemon::ListDigraph::Arc>(it)));
                slew = std::max(slew, m_arcs.slew(static_cast<lemon::ListDigraph::Arc>(it)));
            }
            m_nodes.arrival(node, arrival);
            m_nodes.slew(node, slew);
        }

        for(auto it = m_graph.out_edges_it(node); it != lemon::INVALID; ++it)
            m_arcs_calculators[it]->update(m_graph, it, m_nodes, m_arcs);
    }

    for(auto node_it = m_topological_sorted_nodes.rbegin(); node_it != m_topological_sorted_nodes.rend(); ++node_it)
    {
        auto node = *node_it;
        boost::units::quantity<boost::units::si::time> required(std::numeric_limits<double>::max() * boost::units::si::second);
        if( lemon::countOutArcs(m_graph.G(), node) >= 1 )
        {
            for(lemon::ListDigraph::OutArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it)
            {
                lemon::ListDigraph::Node to(m_graph.edge_target(static_cast<lemon::ListDigraph::Arc>(it)));
                required = std::min(required, m_nodes.required(to) - m_arcs.delay(it) );
            }
            m_nodes.required(node, required);
        }
    }
}

std::vector<entity::entity> static_timing_analysis::critical_path() const
{
    lemon::ListDigraph::Node worst_node;
    bool first = true;
    for(lemon::ListDigraph::NodeIt node(m_graph.G()); node != lemon::INVALID; ++node)
    {
        if(lemon::countOutArcs(m_graph.G(), node) == 0)
        {
            if(first)
            {
                worst_node = node;
                first = false;
            }
            else
            {
                if(m_nodes.required(node)-m_nodes.arrival(node) < m_nodes.required(worst_node)-m_nodes.arrival(worst_node))
                    worst_node = node;
            }
        }
    }
    std::vector<openeda::entity::entity> cp;

    while(lemon::countInArcs(m_graph.G(), worst_node) > 0)
    {
        cp.push_back(m_graph.pin(worst_node));

        boost::units::quantity< boost::units::si::time > worst_slack(std::numeric_limits<double>::max()*boost::units::si::seconds);
        lemon::ListDigraph::Node worst_input;
        bool first = true;
        for(lemon::ListDigraph::InArcIt arc(m_graph.G(), worst_node); arc != lemon::INVALID; ++arc)
        {
            auto source = m_graph.edge_source(arc);
            if(first)
            {
                worst_input = source;
                first = false;
            } else
            {
                if(m_nodes.required(source)-m_nodes.arrival(source) < m_nodes.required(worst_input)-m_nodes.arrival(worst_input))
                    worst_input = source;
            }
        }

        worst_node = worst_input;
    }
    return cp;
}

void static_timing_analysis::set_constraints(const netlist::netlist & netlist, const design_constraints & dc) {

    using namespace boost::units;
    using namespace boost::units::si;
    for(auto & i : dc.input_delays)
    {
        auto pin = netlist.pin_by_name(i.port_name);
        m_nodes.arrival( m_graph.rise_node(pin), quantity<si::time>(i.delay*pico*seconds) );
        m_nodes.arrival( m_graph.fall_node(pin), quantity<si::time>(i.delay*pico*seconds) );
    }

    for(auto & i : dc.input_drivers)
    {
        auto pin = netlist.pin_by_name(i.port_name);
        m_nodes.slew( m_graph.rise_node(pin), quantity<si::time>(i.slew_rise*pico*seconds) );
        m_nodes.slew( m_graph.fall_node(pin), quantity<si::time>(i.slew_fall*pico*seconds) );
    }


    for(auto node_it = m_topological_sorted_nodes.rbegin(); node_it != m_topological_sorted_nodes.rend(); ++node_it)
    {
        auto node = *node_it;
        if(lemon::countOutArcs(m_graph.G(), node) == 0)
            m_nodes.required( node, quantity<si::time>(dc.clock.period * pico* seconds) );
    }

}

} /* namespace timing */
} /* namespace openeda */

