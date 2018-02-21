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

class Arc : public entity_system::EntityBase
{
public:
    using entity_system::EntityBase::EntityBase;
};

class TimingArcs
{
public:
    using ArcNotifier = entity_system::EntitySystem<Arc>::NotifierType;
    using ArcsIterator = entity_system::EntitySystem<Arc>::const_iterator;

    //! TimingArcs Constructor
    /*!
       \brief Constructs an empty system with no Arcs.
     */
    TimingArcs();


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

    //! Add Arc
    /*!
       \brief Adds an arc instance. An arc has a name associated to it. If the arc already exist then just return the existing arc.
       \param name Name of the arc, used to identify it.
       \return A handler for the created/existing Cell.
     */
    Arc add(const std::string & name);

    //! Add Pin of origin of an arc
    /*!
       \brief Sets the pin of origin of an arc
       \param arc A handler for the Arc we want to get the pin.
       \param from_ A handler for the pin of origin.
     */
    void from(const Arc & arc, const standard_cell::Pin & from_);

    //! Add Target pin of an arc
    /*!
       \brief Sets the target pin of an arc
       \param arc A handler for the Arc we want to get the pin.
       \param to_ A handler for the target pin.
     */
    void to(const Arc & arc, const standard_cell::Pin & to_);

    //! Erase Arc
    /*!
       \param atc A handler for the Arc to erase.
       \brief Erases an Arc instance.
     */
    void erase(const Arc & arc);

    //! Allocate space for storing Arc entities
    /*!
       \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions arcs), then it is worth reserving space for this amount before starting to build the standard arcs.
       \param size Minimum capacity for the arc container.
     */
    void reserve(uint32_t size);

    //! Size of Arc's System
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

    //! Capacity of the Arc's System
    /*!
       \return The capacity of the Arc EntitySystem.
     */
    uint32_t capacity() const;

    //! Find an arc
    /*!
       \brief Using the mapping, return an arc handler by arc's name.
       \param The arc name.
       \return Return a arc handler by arc's name.
     */
    Arc find(const std::string & arcName);

    //! Arcs iterator
    /*!
       \return Range iterator for the arcs.
     */
    ophidian::util::Range<TimingArcs::ArcsIterator> range() const;

    //! Pin of origin of an arc
    /*!
       \brief Returns the pin of origin of the arc.
       \param arc A handler for the Arc we want to get the pin.
       \return circuit::Pin Pin of origin
     */
    standard_cell::Pin from(const Arc & arc) const;

    //! Target pin of an arc
    /*!
       \brief Returns the target pin of the arc.
       \param arc A handler for the Arc we want to get the pin.
       \return circuit::Pin Target pin
     */
    standard_cell::Pin to(const Arc & arc) const;

    //! Make Cell Property
    /*!
       \brief Creates a Property for the Standard Cell's Entity System.
       \tparam Value value type of the Property.
       \return An Cell => \p Value Map.
     */
    template <typename Value>
    entity_system::Property<Arc, Value> makeProperty()
    const {
        return entity_system::Property<Arc, Value>(mArcs);
    }

private:
    //Arcs entity system and properties
    entity_system::EntitySystem<Arc> mArcs;
    entity_system::Property<Arc, standard_cell::Pin> mFrom;
    entity_system::Property<Arc, standard_cell::Pin> mTo;
    entity_system::Property<Arc, std::string> mArcNames;
    std::unordered_map<std::string, Arc> mName2Arc;

};

} // namespace timing
} // namespace ophidian


#endif // OPHIDIAN_TIMING_TIMINGARCS_H
