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

#include <pins.h>

namespace ophidian {
namespace standard_cell {

pins::pins(entity::system & system) :
	m_system(system){
	m_system.register_property(&m_names);
	m_system.register_property(&m_owners);
	m_system.register_property(&m_directions);
    m_system.register_property(&m_clock_input);
}

pins::~pins() {
}

void pins::name(entity::entity pin, std::string name) {
	m_names[m_system.lookup(pin)] = name;
}

void pins::owner(entity::entity pin, entity::entity cell) {
	m_owners[m_system.lookup(pin)] = cell;
}

void pins::direction(entity::entity pin, pin_directions direction) {
    m_directions[m_system.lookup(pin)] = direction;
}

void pins::clock_input(entity::entity pin, bool clock)
{
    m_clock_input[m_system.lookup(pin)] = clock;
}

} /* namespace standard_cell */
} /* namespace ophidian */

