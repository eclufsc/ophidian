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
    using GraphType = lemon::ListGraph;
    using Capacitor = GraphType::Node;
    using Resistor = GraphType::Edge;
    using ResistorIt = GraphType::OutArcIt;

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
    Resistor addResistor(const Capacitor & u, const Capacitor & v, const util::ohm_t res);

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
    void capacitance(const Capacitor & cap, const util::farad_t value);

    //! Capacitance's value.
    /*!
       \brief Returns the capacitance value of the capacitor.
       \param cap The handler to the capacitor.
       \return The capacitance value.
     */
    util::farad_t capacitance(const Capacitor & cap) const;

    //! Resistance's value.
    /*!
       \brief Returns the resistance value of the resistor
       \param res The handler to the resistor.
       \return The resistance value.
     */
    util::ohm_t resistance(const Resistor & res) const;

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
    util::farad_t lumped() const;

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
    GraphType mGraph;
    GraphType::NodeMap<std::string> mNames;
    GraphType::NodeMap<util::farad_t> mCapacitances;
    GraphType::EdgeMap<util::ohm_t> mResistances;
    std::vector<Capacitor> mTaps;
    std::unordered_map<std::string, Capacitor> mName2Capacitor;
    util::farad_t mLumpedCapacitance;
};

}   // namespace timingdriven_placement
}   // namespace ophidian



#endif // OPHIDIAN_TIMINGDRIVEN_PLACEMENT_RCTREE_H
