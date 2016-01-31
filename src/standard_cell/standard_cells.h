/*
 * standard_cells.h
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#ifndef SRC_STANDARD_CELL_STANDARD_CELLS_H_
#define SRC_STANDARD_CELL_STANDARD_CELLS_H_

#include <entity.h>

#include <boost/bimap/bimap.hpp>

namespace openeda {
namespace standard_cell {

class standard_cells {
	entity::system m_system;
	boost::bimaps::bimap<std::string, entity::entity> m_mapping;
public:
	virtual ~standard_cells();
	entity::entity create(std::string name);
	std::string name(entity::entity cell) const { return m_mapping.right.at(cell); }
	std::size_t size() const { return m_system.size(); }
	bool empty() const { return m_system.empty(); }
	std::vector<std::string> names() const;


	void register_property(entity::property* property);
	const entity::system & system() const {
		return m_system;
	}


};

} /* namespace standard_cell */
} /* namespace openeda */

#endif /* SRC_STANDARD_CELL_STANDARD_CELLS_H_ */
