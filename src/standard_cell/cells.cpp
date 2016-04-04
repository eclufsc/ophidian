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

#include <cells.h>

namespace ophidian {
namespace standard_cell {

cells::cells(entity::system& system) :
		m_system(system) {
	system.register_property(&m_names);
	system.register_property(&m_pins);
}

cells::~cells() {
}

void cells::name(entity::entity e, std::string name) {
	m_names[m_system.lookup(e)] = name;
}

void cells::insert_pin(entity::entity cell, entity::entity pin) {
	m_pins[m_system.lookup(cell)].push_back(pin);
}

void cells::pins(entity::entity cell, std::vector<entity::entity> pins) {
	m_pins[m_system.lookup(cell)] = pins;
}

} /* namespace standard_cell */
} /* namespace ophidian */
