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

#ifndef SRC_TIMING_RC_TREE_H_
#define SRC_TIMING_RC_TREE_H_

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

#include <lemon/list_graph.h>
#include <lemon/maps.h>

#include <unordered_map>

namespace ophidian {
	/// Namespace describing interconnection estimation functions.
namespace interconnection {

using namespace boost::units;

class packed_rc_tree {
    std::vector< std::size_t > m_pred;
    std::vector< quantity<si::resistance> > m_resistances;
    std::vector< quantity<si::capacitance> > m_capacitances;
public:
    packed_rc_tree(std::size_t node_count);
    virtual ~packed_rc_tree();
    void pred(std::size_t i, std::size_t pred);
    void capacitance(std::size_t i, quantity<si::capacitance> cap);
    void resistance(std::size_t i, quantity<si::resistance> res);
    std::size_t node_count() const {
        return m_pred.size();
    }
    std::size_t pred(std::size_t i) const {
        return m_pred[i];
    }
    quantity<si::resistance> resistance(std::size_t i) const {
        return m_resistances[i];
    }
    quantity<si::capacitance> capacitance(std::size_t i) const {
        return m_capacitances[i];
    }
};

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

    packed_rc_tree pack(capacitor_id source);

};



} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_RC_TREE_H_ */
