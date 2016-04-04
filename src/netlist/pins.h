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

#ifndef SRC_NETLIST_PINS_H_
#define SRC_NETLIST_PINS_H_

#include <vector>
#include "../entity/entity.h"
#include "../entity/vector_property.h"

namespace ophidian {
namespace netlist {

class pins {
	entity::system & m_system;

	entity::vector_property<std::string> m_names;
	entity::vector_property<entity::entity> m_owners;
	entity::vector_property<entity::entity> m_nets;
	entity::vector_property<entity::entity> m_std_cell_pin;

public:
	pins(entity::system & system);
	virtual ~pins();

	std::string name(entity::entity pin) const {
		return m_names[m_system.lookup(pin)];
	}
	entity::entity owner(entity::entity pin) const {
		return m_owners[m_system.lookup(pin)];
	}
	entity::entity net(entity::entity pin) const {
		return m_nets[m_system.lookup(pin)];
	}
	entity::entity standard_cell_pin(entity::entity pin) const {
		return m_std_cell_pin[m_system.lookup(pin)];
	}

	std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > owners() const {
		return std::make_pair(m_owners.begin(), m_owners.end());
	}

	std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > nets() const {
		return std::make_pair(m_nets.begin(), m_nets.end());
	}

	void name(entity::entity pin, std::string name);
	void owner(entity::entity pin, entity::entity owner);
	void net(entity::entity pin, entity::entity net);
	void standard_cell_pin(entity::entity pin, entity::entity std_cell_pin);

};

} /* namespace netlist */
} /* namespace ophidian */

#endif /* SRC_NETLIST_PINS_H_ */
