/*
 * cells.h
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#ifndef SRC_NETLIST_CELLS_H_
#define SRC_NETLIST_CELLS_H_

#include <vector>
#include "../entity/vector_property.h"
#include "../standard_cell/standard_cells.h"

namespace openeda {
namespace netlist {

class cells {
	entity::system & m_system;

	entity::vector_property<std::string> m_names;
	entity::vector_property<entity::entity> m_standard_cells;
	entity::vector_property<std::vector<entity::entity> > m_pins;
public:
	cells(entity::system & system);
	virtual ~cells();

	std::string name(entity::entity cell) const {
		return m_names[m_system.lookup(cell)];
	}
	entity::entity standard_cell(entity::entity cell) const {
		return m_standard_cells[m_system.lookup(cell)];
	}
	std::vector<entity::entity> pins(entity::entity cell) const {
		return m_pins[m_system.lookup(cell)];
	}

	std::pair< std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator > names() const {
		return std::make_pair(m_names.begin(), m_names.end());
	}

	std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > standard_cells() const {
		return std::make_pair(m_standard_cells.begin(), m_standard_cells.end());
	}

	std::pair< std::vector<std::vector<entity::entity>>::const_iterator, std::vector<std::vector<entity::entity>>::const_iterator > pins() const {
		return std::make_pair(m_pins.begin(), m_pins.end());
	}

	void insert_pin(entity::entity cell, entity::entity pin);
	void pins(entity::entity cell, std::vector<entity::entity> pins);
	void name(entity::entity cell, std::string name);
	void standard_cell(entity::entity cell, entity::entity std_cell);

};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_CELLS_H_ */
