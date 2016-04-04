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

#include "pins.h"

namespace ophidian {
namespace netlist {

pins::pins(entity::system & system) : m_system(system) {
	m_system.register_property(&m_names);
	m_system.register_property(&m_owners);
	m_system.register_property(&m_nets);
	m_system.register_property(&m_std_cell_pin);
}

pins::~pins() {
}

void pins::name(entity::entity pin, std::string name) {
	m_names[m_system.lookup(pin)] = name;
}

void pins::owner(entity::entity pin, entity::entity owner) {
	m_owners[m_system.lookup(pin)] = owner;
}

void pins::net(entity::entity pin, entity::entity net) {
	m_nets[m_system.lookup(pin)] = net;
}

void pins::standard_cell_pin(entity::entity pin,
		entity::entity std_cell_pin) {
	m_std_cell_pin[m_system.lookup(pin)] = std_cell_pin;
}
} /* namespace netlist */
} /* namespace ophidian */

