/*
 * pins.h
 *
 *  Created on: Mar 7, 2016
 *      Author: csguth
 */

#ifndef SRC_STANDARD_CELL_PINS_H_
#define SRC_STANDARD_CELL_PINS_H_

#include "../entity/entity.h"
#include "../entity/vector_property.h"

namespace openeda {
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
} /* namespace openeda */

#endif /* SRC_STANDARD_CELL_PINS_H_ */
