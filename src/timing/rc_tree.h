/*
 * rc_tree.h
 *
 *  Created on: Mar 10, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_RC_TREE_H_
#define SRC_TIMING_RC_TREE_H_

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

#include <lemon/list_graph.h>
#include <lemon/maps.h>

#include <unordered_map>

namespace openeda {
namespace timing {

using namespace boost::units;

class rc_tree {
public:
	using graph_t =lemon::ListGraph;
private:
public:
	graph_t m_graph;
	graph_t::NodeMap<std::string> m_names;
	graph_t::NodeMap<quantity<si::capacitance> > m_capacitances;
	graph_t::EdgeMap<quantity<si::resistance> > m_resistances;
	quantity<si::capacitance> m_lumped_capacitance;

	std::unordered_map<std::string, lemon::ListGraph::Node> m_name2node;
public:
	using capacitor_id = lemon::ListGraph::Node;
	using resistor_id = lemon::ListGraph::Edge;
	using resistor_it = lemon::ListGraph::IncEdgeIt;
	rc_tree();
	virtual ~rc_tree();

	quantity<si::capacitance> lumped() const {
		return m_lumped_capacitance;
	}

	capacitor_id capacitor_insert(std::string name);

    std::string capacitor_name(capacitor_id u) const {
        return m_names[u];
    }

	resistor_id resistor_insert(capacitor_id u, capacitor_id v,
			quantity<si::resistance> res);

	void capacitance(capacitor_id u, quantity<si::capacitance> cap);

	capacitor_id node_by_name(std::string name) const {
		return m_name2node.at(name);
	}
	quantity<si::capacitance> capacitance(capacitor_id u) const {
		return m_capacitances[u];
	}

	quantity<si::resistance> resistance(resistor_id uv) const {
		return m_resistances[uv];
	}

	resistor_it capacitor_resistors(capacitor_id u) const {
		return resistor_it(m_graph, u);
	}

	capacitor_id other_capacitor(resistor_id res, capacitor_id cap) const {
		return m_graph.oppositeNode(cap, res);
	}

	static resistor_it invalid() {
		return lemon::INVALID;
	}

	graph_t & graph() {
		return m_graph;
	}

};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_RC_TREE_H_ */
