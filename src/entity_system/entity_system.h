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

#ifndef OPHIDIAN_SRC_ENTITY_SYSTEM_ENTITY_SYSTEM_H
#define OPHIDIAN_SRC_ENTITY_SYSTEM_ENTITY_SYSTEM_H

#include "entity.h"
#include "property.h"
#include <vector>
#include <unordered_set>
#include <assert.h>

namespace ophidian{
namespace entity_system{

/// attorney class.
/*
 * This class provides three static functions to access private methods from properties.
 */
class attorney {
private:
    inline static void destroy(property & p, entity_index e) {
        return p.destroy(e);
    }
    inline static void create(property & p, entity_index e) {
        return p.create(e);
    }
    inline static void preallocate(property & p, size_t qnt) {
        return p.preallocate(qnt);
    }
    friend class entity_system;
};

using entities_storage = typename std::vector<entity>;
using entities_index_storage = typename std::vector<entity_index>;
/// entity_system class.
/*
 * This class describes an entity system, which stores all its entities and pointers to the properties associated to them.
 */
class entity_system{
    entities_storage m_entities;
    entities_index_storage m_id_to_index;

    std::unordered_set<property *> m_associated_properties;

    entity m_next;
    std::size_t m_last_prealloc_qnt;

    inline void m_live_entity_check(entity_index e_index) const {
        if(e_index == invalid_entity_index)
            throw std::out_of_range("lookup::_invalid_entity");
    }

public:

    /// Constructor.
    /**
     * Default constructor. Creates an empty vector of entities.
     */
    entity_system() : m_next(0), m_last_prealloc_qnt(0){

    }

    /// Constructor.
    /**
     * Non-default constructor. It preallocates a given quantity of entities to be stored in the future.
     * \param qnt quantity to be allocated.
     */
    entity_system(std::size_t qnt) : m_entities(qnt), m_id_to_index(qnt), m_next(0), m_last_prealloc_qnt(0){

    }

    ~entity_system(){

    }

    /// Returns the size of the system.
    /**
     * Returns the number of entities as the size of the system.
     * \return Number of entities in the system.
     */
    std::size_t size() const { return m_entities.size(); }

    /// Returns if the system is empty.
    /**
     * Returns if the system is empty or not. An empty system do not have entities belonging to it.
     * \return true if the system is empty, false otherwise.
     */
    bool empty() const { return m_entities.empty(); }

    /// Begin iterator.
    /**
     * Returns an iterator pointing to the beginning of the entities in the system.
     * The iterator can be used to iterate through the entities.
     * \return Iterator pointing to the beginning of the entities in the system.
     */
    entities_storage::const_iterator begin() const { return m_entities.begin(); }

    /// End iterator.
    /**
     * Returns an interator pointing to the end of the entities in the system.
     * \return Iterator pointing to the end of the entities in the system.
     */
    entities_storage::const_iterator end() const { return m_entities.end(); }

    /// Entities getter.
    /**
     * Getter of entities vector.
     * \return A constant reference to the vector containing all entities in the system.
     */
    const entities_storage & entities() const { return m_entities; }

    /// Destroy an entity.
    /**
     * Destroys a entity from the system.
     * If there are properties associtated to the system they are notified that an entity has been destroyed.
     * \param e Entity to be destroyed
     */
    void destroy( entity e );

    /// Creates an entity.
    /**
     * Creates a new entity and add it to the system.
     * If there are properties associtated to the system they are notified that a new entity has been created.
     * \return The created entity.
     */
    entity create();

    /// Preallocate method.
    /**
     * This method preallocate a given quantity of entities to be stored in the future. It also notifies every property to increase its size.
     * \param qnt quantity to be allocated.
     */
    void preallocate(std::size_t qnt);

    /// Registers property.
    /**
     *	Registers a new property in the system. If there are already entities created in the system the property is notified to create values to them.
     *	\param p Pointer to the new property.
     */
    void register_property(property * property);

    /// Gets the index of an entity
    /**
     * Gets the index of an entity, which can be used to acess its properties.
     * \param e Entity to lookup.
     * \return Index of the entity.
     */
    inline entity_index lookup(entity e){
        entity_index e_index = m_id_to_index.at(e);
#ifndef NDEBUG
        if(e_index == invalid_entity_index)
            throw std::out_of_range("lookup::_invalid_entity");
#endif
        return e_index;
    }

    inline entity_index lookup(entity e) const {
        entity_index e_index = m_id_to_index.at(e);
#ifndef NDEBUG
        m_live_entity_check(e_index);
#endif
        return e_index;
    }
};

} /* namespace entity system */
} /* namespace ophidian */

#endif //OPHIDIAN_SRC_ENTITY_SYSTEM_ENTITY_SYSTEM_H
