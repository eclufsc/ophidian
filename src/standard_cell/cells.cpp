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

#include <cells.h>

namespace ophidian {
namespace standard_cell {

cells::cells(entity_system::entity_system& system) :
		m_system(system) {
	system.register_property(&m_names);
	system.register_property(&m_pins);
    system.register_property(&m_sequential);
}

cells::~cells() {
}

void cells::name(entity_system::entity e, std::string name) {
	m_names[m_system.lookup(e)] = name;
}

void cells::insert_pin(entity_system::entity cell, entity_system::entity pin) {
	m_pins[m_system.lookup(cell)].push_back(pin);
}

void cells::pins(entity_system::entity cell, std::vector<entity_system::entity> pins) {
    m_pins[m_system.lookup(cell)] = pins;
}

void cells::sequential(entity_system::entity cell, bool sequential)
{
    m_sequential[m_system.lookup(cell)] = sequential;
}

} /* namespace standard_cell */
} /* namespace ophidian */
