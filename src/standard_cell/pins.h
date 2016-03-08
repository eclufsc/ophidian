/*
 * pins.h
 *
 *  Created on: Mar 7, 2016
 *      Author: csguth
 */

#ifndef SRC_STANDARD_CELL_PINS_H_
#define SRC_STANDARD_CELL_PINS_H_

#include "entity.h"
#include "vector_property.h"

namespace openeda {
namespace standard_cell {

class pins {
	entity::system & m_system;

	entity::vector_property< std::string > m_names;
	entity::vector_property< entity::entity > m_owners;

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

};

} /* namespace standard_cell */
} /* namespace openeda */

#endif /* SRC_STANDARD_CELL_PINS_H_ */
