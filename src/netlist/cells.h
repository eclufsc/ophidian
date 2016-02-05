/*
 * cells.h
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#ifndef SRC_NETLIST_CELLS_H_
#define SRC_NETLIST_CELLS_H_

#include <vector>
#include <vector_property.h>
#include <standard_cells.h>

namespace openeda {
namespace netlist {

class cells: public entity::system {
	entity::vector_property<std::string> m_names;
	entity::vector_property<entity::entity> m_standard_cells;
	entity::vector_property<std::vector<entity::entity> > m_pins;
public:
	cells();
	virtual ~cells();

	std::string name(entity::entity cell) const {
		return m_names[m_entities.left.at(cell)];
	}
	entity::entity standard_cell(entity::entity cell) const {
		return m_standard_cells[m_entities.left.at(cell)];
	}
	std::vector<entity::entity> pins(entity::entity cell) const {
		return m_pins[m_entities.left.at(cell)];
	}
	void insert_pin(entity::entity cell, entity::entity pin);
	void pins(entity::entity cell, std::vector<entity::entity> pins);
	void name(entity::entity cell, std::string name);
	void standard_cell(entity::entity cell, entity::entity std_cell);

};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_CELLS_H_ */
