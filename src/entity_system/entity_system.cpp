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

#include "entity_system.h"

namespace ophidian{
namespace entity_system{

void entity_system::destroy(entity e){
    entity_index e_index = m_id_to_index.at(e);
#ifndef NDEBUG
    if(e_index == invalid_entity_index)
        throw std::out_of_range("lookup::_invalid_entity");
#endif

    auto last_entity_index = m_entities.size()-1;
    auto entity_to_destroy_index = m_id_to_index.at(e);
    m_id_to_index.at(m_entities.at(last_entity_index)) = entity_to_destroy_index;
    m_id_to_index.at(e) = invalid_entity_index;

    m_entities.at(entity_to_destroy_index) = m_entities.at(last_entity_index);
    m_entities.pop_back();
    for(auto property : m_associated_properties)
        attorney::destroy(*property, e_index);
}

entity entity_system::create(){
    entity new_entity = static_cast<entity>(m_next++);
    entity_index new_entity_index = static_cast<entity_index>(m_entities.size());
    m_id_to_index.push_back(new_entity_index);
    m_entities.push_back(new_entity);
    for(auto property : m_associated_properties)
        attorney::create(*property, m_id_to_index.at(new_entity));

    return new_entity;
}

void entity_system::preallocate(std::size_t qnt){
    m_last_prealloc_qnt = qnt;
    m_entities.reserve(qnt);
    m_id_to_index.reserve(qnt);
    for(auto property : m_associated_properties)
        attorney::preallocate(*property, qnt);
}

void entity_system::register_property(property *property){
    //preallocate the same size as the already inserted properties
    attorney::preallocate(*property, m_last_prealloc_qnt);
    m_associated_properties.insert(property);
    for(auto entity : m_entities)
        attorney::create(*property, m_id_to_index.at(entity));
}

} /* namespace entity system */
} /* namespace ophidian */
