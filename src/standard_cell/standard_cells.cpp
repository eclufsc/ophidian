/*
 * standard_cells.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "standard_cells.h"

namespace openeda {
namespace standard_cell {

standard_cells::standard_cells() :
						m_cells(m_cell_system), m_pins(m_pin_system) {

}

standard_cells::~standard_cells() {
}

entity::entity standard_cells::cell_create(std::string name) {
	auto result = m_name2cell.find(name);
	if (result != m_name2cell.end())
		return result->second;

	auto id = m_cell_system.create();
	m_name2cell[name] = id;
	m_cells.name(id, name);
	return id;
}

void standard_cells::register_cell_property(entity::property* property) {
	m_cell_system.register_property(property);
}

void standard_cells::register_pin_property(
		entity::property* property) {
	m_pin_system.register_property(property);
}

entity::entity standard_cells::pin_create(entity::entity cell,
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

entity::entity standard_cells::pad_create(std::string pin_name) {

	auto result = m_name2pin.find(pin_name);
	if (result != m_name2pin.end())
		return result->second;

	auto id = m_pin_system.create();
	m_name2pin[pin_name] = id;
	m_pins.name(id, pin_name);
	return id;
}

void standard_cells::pin_direction(entity::entity pin, pin_directions direction) {
	m_pins.direction(pin, direction);
}

} /* namespace standard_cell */
} /* namespace openeda */

