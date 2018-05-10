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

    using capacitance_unit_type                 = util::farad_t;
    using resistance_unit_type                  = util::ohm_t;

    using graph_type                            = lemon::ListGraph;
    using capacitor_type                        = graph_type::Node;
    using capacitor_name_type                   = std::string;
    using resistor_type                         = graph_type::Edge;
    using resistor_iterator_type                = graph_type::OutArcIt;
    template <class T> using capacitor_map_type = graph_type::NodeMap<T>;
    template <class T> using resistor_map_type  = graph_type::EdgeMap<T>;

    //! RCTree Constructor
    /*!
       \brief Constructs a RCTree with properties.
     */
    RCTree();

    //! RCTree Copy Constructor
    /*!
       \brief Constructs a RCTree with of another RCTree.
     */
    RCTree(const RCTree & other);

    //! RCTree Assignment Operator
    /*!
       \brief Copia as propriedades e atributos de outra RCTree.
     */
    RCTree & operator=(const RCTree & other);

    //! RCTree Destructor
    /*!
       \brief Destructor the RCTree and yours proprieties.
     */
    virtual ~RCTree();

    //! Add a capacitor.
    /*!
       \brief Create a new capacitor_type if don't exist.
       \param name The name of the capacitor. The names must be unique.
       \return The handler to the capacitor.
     */
    capacitor_type addCapacitor(const std::string name);

    //! Add a resistor.
    /*!
       \brief Create a new resistor_type if don't exist.
       \param u Handler to the first capacitor.
       \param v Handler to the second capacitor.
       \param res The resistance value of the resistor.
       \return The handler to the resistor.
     */
    resistor_type addResistor(const capacitor_type & u, const capacitor_type & v, const resistance_unit_type res);

    //! Insert a Tap.
    /*!
       \brief Set a capacitor as a tap node of the RC Tree.
       \param cap The handler to the capacitor.
     */
    void insertTap(const capacitor_type & cap);

    //! capacitor_type's name
    /*!
       \brief Returns the name of a capacitor.
       \param cap The handler to the capacitor.
       \return The name of the capacitor cap.
     */
    std::string name(const capacitor_type & cap) const;

    //! Name's capacitor
    /*!
       \brief Returns the capacitor of a name.
       \param name The name of the capacitor.
       \return The handler to the capacitor.
     */
    capacitor_type capacitor(const std::string name) const;

    //! Opposite capacitor_type
    /*!
       \brief Returns the opposite capacitor of an edge.
       \param u The handler to the capacitor.
       \param res The handler to the resistor.
       \return The opposite capacitor v.
     */
    capacitor_type oppositeCapacitor(const capacitor_type & u, const resistor_type & res) const;

    //! resistor_type Getter
    /*!
       \brief Returns the resistor associed with to capacitors.
       \param u The handler to the capacitor.
       \param v The handler to the capacitor.
       \return The handler to the resistor.
     */
    resistor_type resistor(const capacitor_type & u, const capacitor_type & v) const;

    //! resistor_type iterator
    /*!
       \brief Returns the iterator's resistor of a capacitor.
       \param cap The handler to the capacitor.
       \return The Iterator handler.
     */
    resistor_iterator_type resistors(const capacitor_type & cap) const;

    //! Capacitance assignment.
    /*!
       \brief Assigns a capacitance value to a capacitor.
       \param cap The handler to the capacitor.
       \param value The capacitance value.
     */
    void capacitance(const capacitor_type & cap, const capacitance_unit_type value);

    //! Capacitance's value.
    /*!
       \brief Returns the capacitance value of the capacitor.
       \param cap The handler to the capacitor.
       \return The capacitance value.
     */
    capacitance_unit_type capacitance(const capacitor_type & cap) const;

    //! Resistance's value.
    /*!
       \brief Returns the resistance value of the resistor
       \param res The handler to the resistor.
       \return The resistance value.
     */
    resistance_unit_type resistance(const resistor_type & res) const;

    //! capacitor_type's predecessor.
    /*!
       \brief Finds the predecessor of a capacitor.
       \return Cap's predecessor.
     */
    capacitor_type pred(const capacitor_type& cap);

    //! Topological order
    /*!
       \brief Returns the topological order of the tree.
       \return Container with the topological order.
     */
    const container_type<capacitor_type>& order();

    //! capacitor_type's size.
    /*!
       \brief Returns the amount of capacitors.
       \return The amount of capacitors.
     */
    std::size_t size(capacitor_type) const;

    //! resistor_type's size.
    /*!
       \brief Returns the amount of resistor.
       \return The amount of resistor.
     */
    std::size_t size(resistor_type) const;

    //! Returns the lumped capacitance.
    /*!
       \brief Returns the sum of the capacitance of all capacitors in the RC Tree.
       \return The RC Tree's lumped capacitance.
     */
    capacitance_unit_type lumped() const;

    //! Set RCTree's source.
    /*!
       \brief Sets a new source for the tree;
       \return The source of the tree.
     */
    void source(const capacitor_type & cap);

    //! RCTree's source.
    /*!
       \brief If the graph are connected returns the source of the tree, else return invalid;
       \return The source of the tree.
     */
    capacitor_type source() const;

    //! RCTree's graph.
    /*!
       \brief Returns the graph of the RCTree
       \return The graph of the RCTree.
     */
    const graph_type & g() const;

    //! Invalid iterator
    /*!
       \brief Assists in the seamless use of RCTree.
       \return The constant invalid.
     */
    static resistor_iterator_type invalid()
    {
        return lemon::INVALID;
    }

private:
    void topology_updates();

    graph_type                                mGraph;
    capacitor_map_type<capacitor_name_type>   mNames;
    capacitor_map_type<capacitance_unit_type> mCapacitances;
    resistor_map_type<resistance_unit_type>   mResistances;

    container_type<capacitor_type>            mTaps;
    map_type<std::string, capacitor_type>     mName2Capacitor;
    capacitance_unit_type                     mLumpedCapacitance;

    capacitor_map_type<resistor_type>         mPred;
    container_type<capacitor_type>            mOrder;
    capacitor_type                            mSource;
    bool                                      mValidPred;
};

}   // namespace timingdriven_placement
}   // namespace ophidian



#endif // OPHIDIAN_TIMINGDRIVEN_PLACEMENT_RCTREE_H
