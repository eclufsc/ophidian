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

#ifndef OPHIDIAN_SRC_ENTITY_SYSTEM_PROPERTY_H
#define OPHIDIAN_SRC_ENTITY_SYSTEM_PROPERTY_H

#include "entity.h"

namespace ophidian{
namespace entity_system{

/// Property class.
/**
 * Describes a property that can be associated to an entity system.
 */
class property {
    friend class attorney;
public:
    property() {}
    /// Virtual destructor.
    virtual ~property() { }

private:
    /// Virtual destroy method.
    /**
     * This method is called when an entity in the system is destroyed.
     * \param e index of the entity whose property will be destroyed
     */
    virtual void destroy( entity_index e ) = 0;

    /// Virtual create method.
    /**
     * This method is called when a new entity is created in the system.
     * \param e index of the entity whose property will be created
     */
    virtual void create( ) = 0;

    /// Virtual preallocate method.
    /**
     * This method preallocate a given quantity to be stored in the future.
     * \param qnt quantity to be allocated.
     */
    virtual void preallocate( std::size_t qnt ) = 0;

};

/// attorney class.
/*
 * This class provides three static functions to access private methods from properties.
 */
class attorney {
private:
    inline static void destroy(property & p, entity_index e) {
        return p.destroy(e);
    }
    inline static void create(property & p) {
        return p.create();
    }
    inline static void preallocate(property & p, size_t qnt) {
        return p.preallocate(qnt);
    }
    friend class entity_system;
};

} /* namespace entity system */
} /* namespace ophidian */

#endif //OPHIDIAN_SRC_ENTITY_SYSTEM_PROPERTY_H
