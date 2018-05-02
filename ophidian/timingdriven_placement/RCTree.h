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

#ifndef OPHIDIAN_TIMINGDRIVEN_PLACEMENT_RCTREE_H
#define OPHIDIAN_TIMINGDRIVEN_PLACEMENT_RCTREE_H

#include <unordered_map>
#include <lemon/list_graph.h>
#include <lemon/euler.h>
#include <lemon/connectivity.h>
#include <ophidian/util/Units.h>

namespace ophidian
{
namespace timingdriven_placement
{

//! RCTree Class
/*!
   A graph G(V,E), where V is the set of Capacitances and E is the set of Resistances.
 */
class RCTree
{
public:
    template <class T> using container_type     = std::vector<T>;
    template <class T, class U> using map_type  = std::unordered_map<T, U>;

    using capacitance_type                      = util::farad_t;
    using resistance_type                       = util::ohm_t;

    using GraphType                             = lemon::ListDigraph;
    using Capacitor                             = GraphType::Node;
    using Resistor                              = GraphType::Arc;
    using ResistorIt                            = GraphType::OutArcIt;
    template <class T> using capacitor_map_type = GraphType::NodeMap<T>;
    template <class T> using resistor_map_type  = GraphType::ArcMap<T>;

    //! RCTree Constructor
    /*!
       \brief Constructs a RCTree with properties.
     */
    RCTree();

    //! RCTree Copy Constructor
    /*!
       \brief Constructs a RCTree with of another RCTree.
     */
    RCTree(const RCTree & other) = delete;

    //! RCTree Assignment Operator
    /*!
       \brief Copia as propriedades e atributos de outra RCTree.
     */
    RCTree & operator=(const RCTree & other) = delete;

    //! RCTree Destructor
    /*!
       \brief Destructor the RCTree and yours proprieties.
     */
    virtual ~RCTree();

    //! Add a capacitor.
    /*!
       \brief Create a new Capacitor if don't exist.
       \param name The name of the capacitor. The names must be unique.
       \return The handler to the capacitor.
     */
    Capacitor addCapacitor(const std::string name);

    //! Add a resistor.
    /*!
       \brief Create a new Resistor if don't exist.
       \param u Handler to the first capacitor.
       \param v Handler to the second capacitor.
       \param res The resistance value of the resistor.
       \return The handler to the resistor.
     */
    Resistor addResistor(const Capacitor & u, const Capacitor & v, const resistance_type res);

    //! Insert a Tap.
    /*!
       \brief Set a capacitor as a tap node of the RC Tree.
       \param cap The handler to the capacitor.
     */
    void insertTap(const Capacitor & cap);

    //! Capacitor's name
    /*!
       \brief Returns the name of a capacitor.
       \param cap The handler to the capacitor.
       \return The name of the capacitor cap.
     */
    std::string name(const Capacitor & cap) const;

    //! Name's capacitor
    /*!
       \brief Returns the capacitor of a name.
       \param name The name of the capacitor.
       \return The handler to the capacitor.
     */
    Capacitor capacitor(const std::string name);

    //! Opposite Capacitor
    /*!
       \brief Returns the opposite capacitor of an edge.
       \param u The handler to the capacitor.
       \param res The handler to the resistor.
       \return The opposite capacitor v.
     */
    Capacitor oppositeCapacitor(const Capacitor & u, const Resistor & res) const;

    //! Resistor Getter
    /*!
       \brief Returns the resistor associed with to capacitors.
       \param u The handler to the capacitor.
       \param v The handler to the capacitor.
       \return The handler to the resistor.
     */
    Resistor resistor(const Capacitor & u, const Capacitor & v) const;

    //! Resistor iterator
    /*!
       \brief Returns the iterator's resistor of a capacitor.
       \param cap The handler to the capacitor.
       \return The Iterator handler.
     */
    ResistorIt resistors(const Capacitor & cap) const;

    //! Capacitance assignment.
    /*!
       \brief Assigns a capacitance value to a capacitor.
       \param cap The handler to the capacitor.
       \param value The capacitance value.
     */
    void capacitance(const Capacitor & cap, const capacitance_type value);

    //! Capacitance's value.
    /*!
       \brief Returns the capacitance value of the capacitor.
       \param cap The handler to the capacitor.
       \return The capacitance value.
     */
    capacitance_type capacitance(const Capacitor & cap) const;

    //! Resistance's value.
    /*!
       \brief Returns the resistance value of the resistor
       \param res The handler to the resistor.
       \return The resistance value.
     */
    resistance_type resistance(const Resistor & res) const;

    //! Capacitor's predecessor.
    /*!
       \brief Finds the predecessor of a capacitor.
       \return Cap's predecessor.
     */
    Capacitor pred(const Capacitor& cap);

    //! Topological order
    /*!
       \brief Returns the topological order of the tree.
       \return Container with the topological order.
     */
    const container_type<Capacitor>& order();

    //! Capacitor's size.
    /*!
       \brief Returns the amount of capacitors.
       \return The amount of capacitors.
     */
    std::size_t size(Capacitor) const;

    //! Resistor's size.
    /*!
       \brief Returns the amount of resistor.
       \return The amount of resistor.
     */
    std::size_t size(Resistor) const;

    //! Returns the lumped capacitance.
    /*!
       \brief Returns the sum of the capacitance of all capacitors in the RC Tree.
       \return The RC Tree's lumped capacitance.
     */
    capacitance_type lumped() const;

    //! Set RCTree's source.
    /*!
       \brief Sets a new source for the tree;
       \return The source of the tree.
     */
    void source(const Capacitor & cap);

    //! RCTree's source.
    /*!
       \brief If the graph are connected returns the source of the tree, else return invalid;
       \return The source of the tree.
     */
    Capacitor source() const;

    //! RCTree's graph.
    /*!
       \brief Returns the graph of the RCTree
       \return The graph of the RCTree.
     */
    const GraphType & g() const;

    //! Invalid iterator
    /*!
       \brief Assists in the seamless use of RCTree.
       \return The constant invalid.
     */
    static ResistorIt invalid()
    {
        return lemon::INVALID;
    }

private:
    void topology_updates();

    GraphType                            mGraph;
    capacitor_map_type<std::string>      mNames;
    capacitor_map_type<capacitance_type> mCapacitances;
    resistor_map_type<resistance_type>   mResistances;

    container_type<Capacitor>            mTaps;
    map_type<std::string, Capacitor>     mName2Capacitor;
    capacitance_type                     mLumpedCapacitance;

    capacitor_map_type<Resistor>         mPred;
    container_type<Capacitor>            mOrder;
    Capacitor                            mSource;
    bool                                 mValidPred;
};

}   // namespace timingdriven_placement
}   // namespace ophidian



#endif // OPHIDIAN_TIMINGDRIVEN_PLACEMENT_RCTREE_H
