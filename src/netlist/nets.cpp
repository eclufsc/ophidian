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

#include "nets.h"

namespace ophidian {
namespace netlist {

nets::nets(entity::system & system) : m_system(system) {
	m_system.register_property(&m_names);
	m_system.register_property(&m_pins);
}

nets::~nets() {
}

void nets::connect(entity::entity net, entity::entity pin) {
	m_pins[m_system.lookup(net)].push_back(pin);
}

void nets::disconnect(entity::entity net, entity::entity pin) {
	auto & pins_vector = m_pins[m_system.lookup(net)];
	std::size_t i = 0;
	while(i < pins_vector.size())
	{
		if(pins_vector[i] == pin)
		{
			pins_vector[i] = pins_vector.back();
			pins_vector.pop_back();
			return;
		}
		++i;
	}
	throw std::runtime_error("pin already disconnected");
}

void nets::pins(entity::entity net, std::vector<entity::entity> pins) {
	m_pins[m_system.lookup(net)] = pins;
}

void nets::name(entity::entity net, std::string name) {
	m_names[m_system.lookup(net)] = name;
}

} /* namespace netlist */
} /* namespace ophidian */
