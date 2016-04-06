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

#ifndef SRC_TIMING_RC_TREE_H_
#define SRC_TIMING_RC_TREE_H_

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

#include <lemon/list_graph.h>
#include <lemon/maps.h>

#include <unordered_map>

namespace ophidian {
namespace interconnection {

using namespace boost::units;

class rc_tree {
public:
	using graph_t =lemon::ListGraph;
private:
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
    rc_tree(const rc_tree& other);
    rc_tree&  operator=(const rc_tree& other);
	virtual ~rc_tree();

	quantity<si::capacitance> lumped() const {
		return m_lumped_capacitance;
	}

	capacitor_id capacitor_insert(std::string name);

	std::string capacitor_name(capacitor_id u) const {
		return m_names[u];
	}

	std::size_t capacitor_count() const {
		return static_cast<std::size_t>(lemon::countNodes(m_graph));
	}

	resistor_id resistor_insert(capacitor_id u, capacitor_id v,
			quantity<si::resistance> res);

	void capacitance(capacitor_id u, quantity<si::capacitance> cap);

	capacitor_id capacitor_by_name(std::string name) const {
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

	const graph_t & graph() const {
		return m_graph;
	}

};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_RC_TREE_H_ */
