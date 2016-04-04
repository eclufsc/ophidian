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

#ifndef SRC_TIMING_STATIC_TIMING_ANALYSIS_H_
#define SRC_TIMING_STATIC_TIMING_ANALYSIS_H_

#include "graph.h"
#include "design_constraints.h"
#include "../netlist/netlist.h"

#include "sta_arc_calculator.h"

namespace ophidian {
namespace timing {

class static_timing_analysis {
	const graph & m_graph;
	const library & m_library;

	graph_nodes_timing m_nodes;
	graph_arcs_timing m_arcs;


    sta_interconnection_estimator * m_interconnection_estimator;
    sta_timing_arc_edge_calculator m_tarcs;
    sta_timing_net_edge_calculator m_tnets;

    lemon::ListDigraph::ArcMap<sta_timing_edge_calculator *> m_arcs_calculators;

    std::vector< lemon::ListDigraph::Node > m_topological_sorted_nodes;
    std::deque< entity::entity > m_dirty_nets;

    std::unordered_set< entity::entity > m_dirty;

public:
    static_timing_analysis(const graph & g, const library & lib, sta_interconnection_estimator * interconnection_estimator);
	virtual ~static_timing_analysis();

	void make_dirty(entity::entity net);

	void set_constraints(const netlist::netlist & netlist, const design_constraints & dc);

	void run();

    boost::units::quantity<boost::units::si::time> rise_slack(entity::entity pin) const {
        assert(m_graph.node_edge(m_graph.rise_node(pin)) == edges::RISE);
        return m_nodes.required(m_graph.rise_node(pin)) - m_nodes.arrival(m_graph.rise_node(pin));
    }

    boost::units::quantity<boost::units::si::time> fall_slack(entity::entity pin) const {
        assert(m_graph.node_edge(m_graph.fall_node(pin)) == edges::FALL);
        return m_nodes.required(m_graph.fall_node(pin)) - m_nodes.arrival(m_graph.fall_node(pin));
    }

	boost::units::quantity<boost::units::si::time> rise_arrival(entity::entity pin) const {
        assert(m_graph.node_edge(m_graph.rise_node(pin)) == edges::RISE);
		return m_nodes.arrival(m_graph.rise_node(pin));
	}

	boost::units::quantity<boost::units::si::time> fall_arrival(entity::entity pin) const {
        assert(m_graph.node_edge(m_graph.fall_node(pin)) == edges::FALL);
		return m_nodes.arrival(m_graph.fall_node(pin));
	}

	boost::units::quantity<boost::units::si::time> rise_slew(entity::entity pin) const {
		return m_nodes.slew(m_graph.rise_node(pin));
	}

	boost::units::quantity<boost::units::si::time> fall_slew(entity::entity pin) const {
		return m_nodes.slew(m_graph.fall_node(pin));
	}

    std::vector< entity::entity > critical_path() const;
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_STATIC_TIMING_ANALYSIS_H_ */
