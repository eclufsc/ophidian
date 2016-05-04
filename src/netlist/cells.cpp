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

#include "cells.h"

namespace ophidian {
namespace netlist {

cells::cells(entity::system & system) : m_system(system) {
	m_system.register_property(&m_names);
	m_system.register_property(&m_standard_cells);
	m_system.register_property(&m_pins);
}

cells::~cells() {
}

void cells::insert_pin(entity::entity cell, entity::entity pin) {
	m_pins[m_system.lookup(cell)].push_back(pin);
}

void cells::pins(entity::entity cell, std::vector<entity::entity> pins) {
	m_pins[m_system.lookup(cell)] = pins;
}

void cells::name(entity::entity cell, std::string name) {
	m_names[m_system.lookup(cell)] = name;
}

void cells::standard_cell(entity::entity cell,
		entity::entity std_cell) {
    m_standard_cells[m_system.lookup(cell)] = std_cell;
}

void cells::pins_preallocate(entity::entity cell, std::size_t pin_count)
{
    m_pins[m_system.lookup(cell)].reserve(pin_count);
}

} /* namespace netlist */
} /* namespace ophidian */


