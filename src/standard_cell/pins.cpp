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

#include <pins.h>

namespace ophidian {
namespace standard_cell {

pins::pins(entity_system::entity_system & system) :
	m_system(system){
	m_system.register_property(&m_names);
	m_system.register_property(&m_owners);
	m_system.register_property(&m_directions);
    m_system.register_property(&m_clock_input);
}

pins::~pins() {
}

void pins::name(entity_system::entity pin, std::string name) {
	m_names[m_system.lookup(pin)] = name;
}

void pins::owner(entity_system::entity pin, entity_system::entity cell) {
	m_owners[m_system.lookup(pin)] = cell;
}

void pins::direction(entity_system::entity pin, pin_directions direction) {
    m_directions[m_system.lookup(pin)] = direction;
}

void pins::clock_input(entity_system::entity pin, bool clock)
{
    m_clock_input[m_system.lookup(pin)] = clock;
}

} /* namespace standard_cell */
} /* namespace ophidian */

