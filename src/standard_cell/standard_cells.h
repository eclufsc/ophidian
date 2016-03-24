/*
 * standard_cells.h
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#ifndef SRC_STANDARD_CELL_STANDARD_CELLS_H_
#define SRC_STANDARD_CELL_STANDARD_CELLS_H_

#include "../entity/entity.h"
#include "../standard_cell/cells.h"
#include "../standard_cell/pins.h"

namespace openeda {
namespace standard_cell {


class standard_cells {

	entity::system m_cell_system;
	entity::system m_pin_system;

	cells m_cells;
	pins m_pins;

	std::unordered_map<std::string, entity::entity> m_name2cell;
	std::unordered_map<std::string, entity::entity> m_name2pin;
public:
	standard_cells();
	virtual ~standard_cells();

	void register_cell_property(entity::property* property);
	void register_pin_property(entity::property* property);

	const entity::vector_property<std::string> & cell_names() const {
		return m_cells.names();
	}
	entity::entity cell_create(std::string name);
	std::string cell_name(entity::entity cell) const {
		return m_cells.name(cell);
	}
	const std::vector<entity::entity> & cell_pins(entity::entity cell) const {
		return m_cells.pins(cell);
	}
	std::size_t cell_count() const {
		return m_cell_system.size();
	}
	const entity::system & cell_system() const {
		return m_cell_system;
	}

	entity::entity pin_create(entity::entity cell, std::string name);
	entity::entity pin_owner(entity::entity pin) const
	{
		return m_pins.owner(pin);
	}
	std::string pin_name(entity::entity pin) const {
		std::string the_name;
		entity::entity owner{m_pins.owner(pin)};
		if(!(owner == entity::entity{}))
			the_name = m_cells.name(owner) + ":";
		the_name += m_pins.name(pin);
		return the_name;
	}
	std::size_t pin_count() const {
		return m_pin_system.size();
	}
	const entity::system & pin_system() const {
		return m_pin_system;
	}
	pin_directions pin_direction(entity::entity pin) const {
		return m_pins.direction(pin);
	}

	void pin_direction(entity::entity pin, pin_directions direction);


	entity::entity pad_create(std::string name);



};

} /* namespace standard_cell */
} /* namespace openeda */

#endif /* SRC_STANDARD_CELL_STANDARD_CELLS_H_ */
