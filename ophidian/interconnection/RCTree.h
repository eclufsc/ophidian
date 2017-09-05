/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_RCTREE_H
#define OPHIDIAN_RCTREE_H

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
class PackedRCTree {
    std::vector< std::size_t > mPred;
    std::vector< quantity<si::resistance> > mResistances;
    std::vector< quantity<si::capacitance> > mCapacitances;
    std::unordered_map<std::string, std::size_t> mTaps;

public:
    PackedRCTree(std::size_t nodeCount=0);
    virtual ~PackedRCTree();
    void pred(std::size_t i, std::size_t pred);
    void capacitance(std::size_t i, quantity<si::capacitance> cap);
    void resistance(std::size_t i, quantity<si::resistance> res);
    std::size_t nodeCount() const {
        return mPred.size();
    }
    std::size_t pred(std::size_t i) const {
        return mPred[i];
    }
    quantity<si::resistance> resistance(std::size_t i) const {
        return mResistances[i];
    }
    quantity<si::capacitance> capacitance(std::size_t i) const {
        return mCapacitances[i];
    }

    std::size_t tap(const std::string & name) const {
        return mTaps.at(name);
    }


    void tap(const std::string & name, std::size_t value);


};

/// RC Tree Class.
/**
*   A graph where the vertex set represents capacitors and the edge set represents resistors.
**/
class RCTree {
public:
	using graph_t =lemon::ListGraph;
private:
    graph_t mGraph;
    graph_t::NodeMap<std::string> mNames;
    graph_t::NodeMap<quantity<si::capacitance> > mCapacitances;
    graph_t::EdgeMap<quantity<si::resistance> > mResistances;
    quantity<si::capacitance> mLumpedCapacitance;
    std::vector< graph_t::Node > mTaps;

    std::unordered_map<std::string, lemon::ListGraph::Node> mName2Node;
public:
    using capacitorID = lemon::ListGraph::Node;
    using resistorID = lemon::ListGraph::Edge;
    using resistorIt = lemon::ListGraph::IncEdgeIt;

    /**
      \brief Creates an RC Tree
    */
    RCTree();
    /**
      \brief Copies an RC Tree
    */
    RCTree(const RCTree& other);
    /**
      \brief Assigns the contents from another RC Tree, replacing its current.
      \param otherCapacitor(resistorID res, capacitorID cap) const
      \return *this
    */
    RCTree&  operator=(const RCTree& other);
    /// RC Tree destructor.
    virtual ~RCTree();
    /**
      Returns the lumped capacitance.
    \brief Returns the sum of the capacitance of all capacitors in the RC Tree
    \return The RC Tree's lumped capacitance.
    */
	quantity<si::capacitance> lumped() const {
        return mLumpedCapacitance;
	}
    /// Set a capacitor as a tap node of the RC Tree.
    void tapInsert(capacitorID cap);

    /// Capacitor insertion.
    /**
      Inserts a capacitor.
    * \param name The name of the capacitor. The names must be unique.
    * \return The handler to the capacitor.
    **/
    capacitorID capacitorInsert(std::string name);

    /// Return capacitor's name
    /**
    * \brief Returns the name of a capacitor.
    * \param u handler to a capacitor.
    * \return The name of the capacitor u.
    */
    std::string capacitorName(capacitorID u) const {
        return mNames[u];
	}

    /// Number of capacitors
    /**
    * \brief Returns the number of capacitors.
    * \return The number of capacitors.
    */
    std::size_t capacitorCount() const {
        return static_cast<std::size_t>(lemon::countNodes(mGraph));
	}

    /// Resistor insertion.
    /**
      Inserts a resistor.
    * \param u Handler to the first capacitor.
    * \param v Handler to the second capacitor.
    * \param res The resistance value of the resistor.
    * \return The handler to the resistor.
    **/
    resistorID resistorInsert(capacitorID u, capacitorID v,
			quantity<si::resistance> res);

    /// Capacitance assignment.
    /**
    * \brief Assigns a capacitance value to a capacitor.
    * \param u handler to a capacitor.
    * \param cap The capacitance value.
    */
    void capacitance(capacitorID u, quantity<si::capacitance> cap);

    /// Capacitor search.
    /**
    * \brief Finds a capacitor by its name.
    * \param name The name of the capacitor.
    * \return The handler to the capacitor.
    */
    capacitorID capacitorByName(std::string name) const {
        return mName2Node.at(name);
	}


    quantity<si::capacitance> capacitance(capacitorID u) const {
        return mCapacitances[u];
	}

    quantity<si::resistance> resistance(resistorID uv) const {
        return mResistances[uv];
	}

    resistorIt capacitorResistors(capacitorID u) const {
        return resistorIt(mGraph, u);
	}

    capacitorID otherCapacitor(resistorID res, capacitorID cap) const {
        return mGraph.oppositeNode(cap, res);
	}


    static resistorIt invalid() {
		return lemon::INVALID;
	}

	const graph_t & graph() const {
        return mGraph;
	}

    PackedRCTree pack(capacitorID source) const;

};



} /* namespace timing */
} /* namespace ophidian */

#endif /* OPHIDIAN_RCTREE_H */
