/*
 * cells.h
 *
 *  Created on: Mar 7, 2016
 *      Author: csguth
 */

#ifndef SRC_STANDARD_CELL_CELLS_H_
#define SRC_STANDARD_CELL_CELLS_H_

#include "entity.h"
#include "vector_property.h"

namespace openeda {
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
} /* namespace openeda */

#endif /* SRC_STANDARD_CELL_CELLS_H_ */
