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
/// The Interonnection namespace.
namespace interconnection {

using namespace boost::units;

/// Packed RC Tree Class.
class packed_rc_tree {
    std::vector< std::size_t > m_pred;
    std::vector< quantity<si::resistance> > m_resistances;
    std::vector< quantity<si::capacitance> > m_capacitances;
    std::unordered_map<std::string, std::size_t> m_taps;

public:
    packed_rc_tree(std::size_t node_count=0);
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

    std::size_t tap(const std::string & name) const {
        return m_taps.at(name);
    }


    void tap(const std::string & name, std::size_t value);


};

/// RC Tree Class.
/**
*   A graph where the vertex set represents capacitors and the edge set represents resistors.
**/
class rc_tree {
public:
	using graph_t =lemon::ListGraph;
private:
	graph_t m_graph;
	graph_t::NodeMap<std::string> m_names;
	graph_t::NodeMap<quantity<si::capacitance> > m_capacitances;
	graph_t::EdgeMap<quantity<si::resistance> > m_resistances;
	quantity<si::capacitance> m_lumped_capacitance;
    std::vector< graph_t::Node > m_taps;

	std::unordered_map<std::string, lemon::ListGraph::Node> m_name2node;
public:
	using capacitor_id = lemon::ListGraph::Node;
	using resistor_id = lemon::ListGraph::Edge;
	using resistor_it = lemon::ListGraph::IncEdgeIt;

    /**
      \brief Creates an RC Tree
    */
	rc_tree();
    /**
      \brief Copies an RC Tree
    */
    rc_tree(const rc_tree& other);
    /**
      \brief Assigns the contents from another RC Tree, replacing its current.
      \param other_capacitor(resistor_id res, capacitor_id cap) const
      \return *this
    */
    rc_tree&  operator=(const rc_tree& other);
    /// RC Tree destructor.
	virtual ~rc_tree();
    /**
      Returns the lumped capacitance.
    \brief Returns the sum of the capacitance of all capacitors in the RC Tree
    \return The RC Tree's lumped capacitance.
    */
	quantity<si::capacitance> lumped() const {
		return m_lumped_capacitance;
	}
    /// Set a capacitor as a tap node of the RC Tree.
    void tap_insert(capacitor_id cap);

    /// Capacitor insertion.
    /**
      Inserts a capacitor.
    * \param name The name of the capacitor. The names must be unique.
    * \return The handler to the capacitor.
    **/
	capacitor_id capacitor_insert(std::string name);

    /// Return capacitor's name
    /**
    * \brief Returns the name of a capacitor.
    * \param u handler to a capacitor.
    * \return The name of the capacitor u.
    */
	std::string capacitor_name(capacitor_id u) const {
		return m_names[u];
	}

    /// Number of capacitors
    /**
    * \brief Returns the number of capacitors.
    * \return The number of capacitors.
    */
	std::size_t capacitor_count() const {
		return static_cast<std::size_t>(lemon::countNodes(m_graph));
	}

    /// Resistor insertion.
    /**
      Inserts a resistor.
    * \param u Handler to the first capacitor.
    * \param v Handler to the second capacitor.
    * \param res The resistance value of the resistor.
    * \return The handler to the resistor.
    **/
	resistor_id resistor_insert(capacitor_id u, capacitor_id v,
			quantity<si::resistance> res);

    /// Capacitance assignment.
    /**
    * \brief Assigns a capacitance value to a capacitor.
    * \param u handler to a capacitor.
    * \param cap The capacitance value.
    */
	void capacitance(capacitor_id u, quantity<si::capacitance> cap);

    /// Capacitor search.
    /**
    * \brief Finds a capacitor by its name.
    * \param name The name of the capacitor.
    * \return The handler to the capacitor.
    */
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

    packed_rc_tree pack(capacitor_id source) const;

};



} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_RC_TREE_H_ */
