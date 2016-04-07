/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "standard_cells.h"

namespace ophidian {
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

void standard_cells::cell_sequential(entity::entity cell, bool sequential)
{
    m_cells.sequential(cell, sequential);
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

void standard_cells::pin_clock_input(entity::entity pin, bool clock_input)
{
    m_pins.clock_input(pin, clock_input);
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
} /* namespace ophidian */

