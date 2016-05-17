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

#include "standard_cells.h"

namespace ophidian {
namespace standard_cell {

standard_cells::standard_cells() :
						m_cells(m_cell_system), m_pins(m_pin_system) {

}

standard_cells::~standard_cells() {
}

entity_system::entity standard_cells::cell_create(std::string name) {
	auto result = m_name2cell.find(name);
	if (result != m_name2cell.end())
		return result->second;

	auto id = m_cell_system.create();
	m_name2cell[name] = id;
	m_cells.name(id, name);
    return id;
}

void standard_cells::cell_sequential(entity_system::entity cell, bool sequential)
{
    m_cells.sequential(cell, sequential);
}

void standard_cells::register_cell_property(entity_system::property* property) {
	m_cell_system.register_property(property);
}

void standard_cells::register_pin_property(
        entity_system::property* property) {
	m_pin_system.register_property(property);
}

entity_system::entity standard_cells::pin_create(entity_system::entity cell,
		std::string name) {

	const std::string std_cell_name = m_cells.name(cell);
	std::string pin_name = std_cell_name + ":" + name;

	auto result = m_name2pin.find(pin_name);
	if (result != m_name2pin.end())
		return result->second;

	auto id = m_pin_system.create();
	m_name2pin[pin_name] = id;
	m_pins.name(id, name);
	m_pins.owner(id, cell);
	m_cells.insert_pin(cell, id);
    return id;
}

void standard_cells::pin_clock_input(entity_system::entity pin, bool clock_input)
{
    m_pins.clock_input(pin, clock_input);
}

entity_system::entity standard_cells::pad_create(std::string pin_name) {

	auto result = m_name2pin.find(pin_name);
	if (result != m_name2pin.end())
		return result->second;

	auto id = m_pin_system.create();
	m_name2pin[pin_name] = id;
	m_pins.name(id, pin_name);
	return id;
}

void standard_cells::pin_direction(entity_system::entity pin, pin_directions direction) {
	m_pins.direction(pin, direction);
}

} /* namespace standard_cell */
} /* namespace ophidian */

