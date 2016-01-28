/*
 * cells.h
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#ifndef SRC_NETLIST_CELLS_H_
#define SRC_NETLIST_CELLS_H_

#include <vector>
#include <property.h>
#include <standard_cells.h>
#include "pins.h"

namespace openeda {
namespace netlist {

class cells: entity::property {
	std::vector<std::string> m_names;
	std::vector<entity::entity> m_standard_cells;
	std::vector<std::vector<entity::entity> > m_pins;
	std::vector<entity::entity> m_index2entity;
	std::unordered_map<entity::entity, std::size_t> m_entity2index;
public:
	cells(entity::system * entity_system);
	virtual ~cells();

	void destroy(entity::entity e);
	void create(entity::entity e);

	std::string name(entity::entity cell) const {
		return m_names[m_entity2index.at(cell)];
	}
	entity::entity standard_cell(entity::entity cell) const {
		return m_standard_cells[m_entity2index.at(cell)];
	}
	std::vector<entity::entity> pins(entity::entity cell) const {
		return m_pins[m_entity2index.at(cell)];
	}
	void insert_pin(entity::entity cell, entity::entity pin);
	void pins(entity::entity cell, std::vector<entity::entity> pins);
	void name(entity::entity cell, std::string name);
	void standard_cell(entity::entity cell, entity::entity std_cell);

};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_CELLS_H_ */
