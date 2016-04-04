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

#ifndef SRC_STANDARD_CELL_CELLS_H_
#define SRC_STANDARD_CELL_CELLS_H_

#include "../entity/entity.h"
#include "../entity/vector_property.h"

namespace ophidian {
namespace standard_cell {

class cells {
	entity::system & m_system;
	entity::vector_property<std::string> m_names;
    entity::vector_property<std::vector<entity::entity> > m_pins;
public:
	cells(entity::system & system);
	virtual ~cells();
	const entity::vector_property<std::string>& names() const {
		return m_names;
	}
	std::string name(entity::entity e) const {
		return m_names[m_system.lookup(e)];
	}
	const std::vector<entity::entity> & pins(entity::entity cell) const {
		return m_pins[m_system.lookup(cell)];
	}
	void name(entity::entity e, std::string name);
	void insert_pin(entity::entity cell, entity::entity pin);
    void pins(entity::entity cell, std::vector<entity::entity> pins);
};

} /* namespace standard_cell */
} /* namespace ophidian */

#endif /* SRC_STANDARD_CELL_CELLS_H_ */
