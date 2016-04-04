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

#ifndef SRC_STANDARD_CELL_PINS_H_
#define SRC_STANDARD_CELL_PINS_H_

#include "../entity/entity.h"
#include "../entity/vector_property.h"

namespace ophidian {
namespace standard_cell {


enum class pin_directions {
		NOT_ASSIGNED, INPUT, OUTPUT
	};

class pins {
	entity::system & m_system;

	entity::vector_property< std::string > m_names;
	entity::vector_property< entity::entity > m_owners;
	entity::vector_property< pin_directions > m_directions;

public:
	pins(entity::system & system);
	virtual ~pins();

	void name(entity::entity pin, std::string name);
	std::string name(entity::entity pin) const {
		return m_names[m_system.lookup(pin)];
	}
	void owner(entity::entity pin, entity::entity cell);
	entity::entity owner(entity::entity pin) const {
		return m_owners[m_system.lookup(pin)];
	}

	void direction(entity::entity pin, pin_directions dir);
	pin_directions direction(entity::entity pin) const {
		return m_directions[m_system.lookup(pin)];
	}

};

} /* namespace standard_cell */
} /* namespace ophidian */

#endif /* SRC_STANDARD_CELL_PINS_H_ */
