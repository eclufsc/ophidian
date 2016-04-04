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

#ifndef SRC_NETLIST_NETS_H_
#define SRC_NETLIST_NETS_H_

#include <vector>
#include "../entity/entity.h"
#include "../entity/vector_property.h"
#include <utility>

namespace ophidian {
namespace netlist {

class nets {
	entity::system & m_system;

	entity::vector_property<std::vector<entity::entity>> m_pins;
	entity::vector_property<std::string> m_names;

public:
	nets(entity::system & system);
	virtual ~nets();

	std::string name(entity::entity net) const {
		return m_names[m_system.lookup(net)];
	}
	std::vector<entity::entity> pins(entity::entity net) const {
		return m_pins[m_system.lookup(net)];
	}
	std::pair< std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator > names() const {
		return std::make_pair(m_names.begin(), m_names.end());
	}

	std::pair< std::vector<std::vector<entity::entity>>::const_iterator, std::vector<std::vector<entity::entity>>::const_iterator > pins() const {
		return std::make_pair(m_pins.begin(), m_pins.end());
	}

	void connect(entity::entity net, entity::entity pin);
	void disconnect(entity::entity net, entity::entity pin);
	void pins(entity::entity net, std::vector<entity::entity> pins);
	void name(entity::entity net, std::string name);
};

} /* namespace netlist */
} /* namespace ophidian */

#endif /* SRC_NETLIST_NETS_H_ */
