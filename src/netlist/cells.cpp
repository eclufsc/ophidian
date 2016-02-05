/*
 * cells.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "cells.h"

namespace openeda {
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

} /* namespace netlist */
} /* namespace openeda */


