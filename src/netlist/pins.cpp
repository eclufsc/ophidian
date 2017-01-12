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

#include "pins.h"

namespace ophidian {
namespace netlist {

pins::pins(entity_system::entity_system & system) : m_system(system) {
	m_system.register_property(&m_names);
	m_system.register_property(&m_owners);
	m_system.register_property(&m_nets);
	m_system.register_property(&m_std_cell_pin);
    m_system.register_property(&m_positions);
}

pins::~pins() {
}

void pins::name(entity_system::entity pin, std::string name) {
	m_names[m_system.lookup(pin)] = name;
}

void pins::owner(entity_system::entity pin, entity_system::entity owner) {
	m_owners[m_system.lookup(pin)] = owner;
}

void pins::net(entity_system::entity pin, entity_system::entity net) {
	m_nets[m_system.lookup(pin)] = net;
}

void pins::standard_cell_pin(entity_system::entity pin,
        entity_system::entity std_cell_pin) {
	m_std_cell_pin[m_system.lookup(pin)] = std_cell_pin;
}

geometry::point<double> pins::pin_position(entity_system::entity pin) const{
    return m_positions[m_system.lookup(pin)];
}

void pins::set_position(entity_system::entity pin, geometry::point<double> position){
    m_positions[m_system.lookup(pin)] = position;
}

} /* namespace netlist */
} /* namespace ophidian */

