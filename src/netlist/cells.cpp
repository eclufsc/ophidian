/*
 * cells.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "cells.h"

namespace openeda {
namespace netlist {

cells::cells(entity::system * entity_system) {
	entity_system->register_property(this);
}

cells::~cells() {
}

void cells::destroy(entity::entity e) {
	std::size_t to_destroy = m_entity2index[e];
	std::size_t last = m_index2entity.size() - 1;
	m_names[to_destroy] = m_names[last];
	m_names.pop_back();

	m_standard_cells[to_destroy] = m_standard_cells[last];
	m_standard_cells.pop_back();

	m_pins[to_destroy] = m_pins[last];
	m_pins.pop_back();

	m_index2entity[to_destroy] = m_index2entity[last];
	m_entity2index[m_index2entity[to_destroy]] = to_destroy;

	m_index2entity.pop_back();
	m_entity2index.erase(e);
}

void cells::create(entity::entity e) {
	m_names.resize(m_names.size() + 1);
	m_standard_cells.resize(m_standard_cells.size() + 1);
	m_pins.resize(m_pins.size() + 1);
	m_entity2index[e] = m_index2entity.size();
	m_index2entity.push_back(e);
}

void cells::insert_pin(entity::entity cell, entity::entity pin) {
	m_pins[m_entity2index[cell]].push_back(pin);
}

void cells::pins(entity::entity cell, std::vector<entity::entity> pins) {
	m_pins[m_entity2index[cell]] = pins;
}

void cells::name(entity::entity cell, std::string name) {
	m_names[m_entity2index[cell]] = name;
}

void cells::standard_cell(entity::entity cell,
		entity::entity std_cell) {
	m_standard_cells[m_entity2index[cell]] = std_cell;
}

} /* namespace netlist */
} /* namespace openeda */


