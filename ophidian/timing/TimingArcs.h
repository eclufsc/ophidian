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

#ifndef OPHIDIAN_TIMING_TIMINGARCS_H
#define OPHIDIAN_TIMING_TIMINGARCS_H

#include <memory>
#include <unordered_map>
#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/util/Range.h>

namespace ophidian
{
namespace timing
{

class TimingArc : public entity_system::EntityBase
{
public:
    using entity_system::EntityBase::EntityBase;
};

class TimingArcs
{
public:
    template <class T> using entity_container_type    = entity_system::EntitySystem<T>;
    template <class T> using vector_type              = std::vector<T>;
    template <class K, class V> using map_type        = std::unordered_map<K, V>;

    using timing_arc_name_type                        = std::string;
    using timing_arc_entity_type                      = TimingArc;
    using standard_cells_type                         = standard_cell::StandardCells;
    using standard_pin_entity_type                    = standard_cell::Pin;
    using standard_cell_entity_type                   = standard_cell::Cell;
    using standard_pin_entity_vector_type             = vector_type<standard_pin_entity_type>;
    using timing_arc_vector_type                      = vector_type<timing_arc_entity_type>;
    using ArcNotifier                                 = entity_system::EntitySystem<timing_arc_entity_type>::NotifierType;
    using ArcsIterator                                = entity_system::EntitySystem<timing_arc_entity_type>::const_iterator;
    using timing_arc_range_type                       = util::Range<ArcsIterator>;
    using timing_arc_entity_container_type            = entity_container_type<timing_arc_entity_type>;

    template <class T> using timing_arc_property_type = entity_system::Property<timing_arc_entity_type, T>;
    template <class T> using pin_property_type        = entity_system::Property<standard_pin_entity_type, T>;

    //! TimingArcs Constructor
    /*!
       \brief Constructs an empty system with no Arcs.
     */
    TimingArcs(const standard_cells_type & stdCells);


    //! TimingArcs Move Constructor
    /*!
        \brief Move the entity system and its properties.
     */
    //TimingArcs(const TimingArcs && timingArcs);

    //! TimingArcs Destructor
    /*!
       \brief Destroys the Arcs EntitySystem, including its properties.
     */
    ~TimingArcs();

//--------------------------- Arcs -------------------------------//

    //! Add TimingArc
    /*!
       \brief Adds an arc instance. An arc has a name associated to it. If the arc already exist then just return the existing arc.
       \param name Name of the arc, used to identify it.
       \return A handler for the created/existing Cell.
     */
    timing_arc_entity_type add(const timing_arc_name_type & name);

    //! Add Pin of origin of an arc
    /*!
       \brief Sets the pin of origin of an arc
       \param arc A handler for the TimingArc we want to get the pin.
       \param from_ A handler for the pin of origin.
     */
    void from(const timing_arc_entity_type & arc, const standard_pin_entity_type & from_);

    //! Add Target pin of an arc
    /*!
       \brief Sets the target pin of an arc
       \param arc A handler for the TimingArc we want to get the pin.
       \param to_ A handler for the target pin.
     */
    void to(const timing_arc_entity_type & arc, const standard_pin_entity_type & to_);

    //! Erase TimingArc
    /*!
       \param atc A handler for the TimingArc to erase.
       \brief Erases an TimingArc instance.
     */
    void erase(const timing_arc_entity_type & arc);

    //! Allocate space for storing TimingArc entities
    /*!
       \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions arcs), then it is worth reserving space for this amount before starting to build the standard arcs.
       \param size Minimum capacity for the arc container.
     */
    void reserve(uint32_t size);

    //! Size of TimingArc's System
    /*!
       \brief Returns the number of Arcs.
       \return The number of Arcs.
     */
    uint32_t size() const;

    //! Iterator to beginning
    /*!
       \brief Returns an iterator pointing to the first element in the Net's EntitySystem.
       \return Iterator to the first element in the Net's EntitySystem.
     */
    ArcsIterator begin() const;
    //! Iterator to end
    /*!
       \brief Returns an iterator referring to the past-the-end element in the Net's EntitySystem.
       \return Iterator referring to the past-the-end element in the Net's EntitySystem.
     */
    ArcsIterator end() const;

    //! Capacity of the TimingArc's System
    /*!
       \return The capacity of the TimingArc EntitySystem.
     */
    uint32_t capacity() const;

    //! Find an arc
    /*!
       \brief Using the mapping, return an arc handler by arc's name.
       \param The arc name.
       \return Return a arc handler by arc's name.
     */
    timing_arc_entity_type find(const timing_arc_name_type & arcName);

    //! Arc's name
    /*!
       \brief Return the name of a arc
       \param The arc.
       \return Return arc's name.
     */
    timing_arc_name_type name(const timing_arc_entity_type & arc);

    //! Arcs iterator
    /*!
       \return Range iterator for the arcs.
     */
    timing_arc_range_type range() const;

    //! Pin of origin of an arc
    /*!
       \brief Returns the pin of origin of the arc.
       \param arc A handler for the TimingArc we want to get the pin.
       \return circuit::Pin Pin of origin
     */
    standard_pin_entity_type from(const timing_arc_entity_type & arc) const;

    //! Target pin of an arc
    /*!
       \brief Returns the target pin of the arc.
       \param arc A handler for the TimingArc we want to get the pin.
       \return circuit::Pin Target pin
     */
    standard_pin_entity_type to(const timing_arc_entity_type & arc) const;

    //! Arcs of a pin
    /*!
       \brief Returns a vector of the Arcs of a Pin.
       \param pin A handler for the Pin we want to get the Arcs.
       \return Vector for the Arcs of a Pin.
     */
    const timing_arc_vector_type & pinArcs(const standard_pin_entity_type & pin) const;

    //! Make Cell Property
    /*!
       \brief Creates a Property for the Standard Cell's Entity System.
       \tparam Value value type of the Property.
       \return An Cell => \p Value Map.
     */
    template <typename Value>
    timing_arc_property_type<Value> makeProperty() const
    {
        return timing_arc_property_type<Value>(mArcs);
    }

private:
    //Arcs entity system and properties
    timing_arc_entity_container_type                       mArcs;
    timing_arc_property_type<standard_pin_entity_type>     mFrom;
    timing_arc_property_type<standard_pin_entity_type>     mTo;
    timing_arc_property_type<timing_arc_name_type>         mArcNames;
    map_type<timing_arc_name_type, timing_arc_entity_type> mName2Arc;
    pin_property_type<timing_arc_vector_type>              mPin2TimingArcs;
};

} // namespace timing
} // namespace ophidian


#endif // OPHIDIAN_TIMING_TIMINGARCS_H
