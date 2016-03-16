/*
 * cells.h
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#ifndef SRC_PLACEMENT_CELLS_H_
#define SRC_PLACEMENT_CELLS_H_


#include "../netlist/netlist.h"
#include "../geometry/geometry.h"
#include "../entity/vector_property.h"
#include <boost/bimap.hpp>

namespace openeda {
namespace placement {

class cells  {
	using point = geometry::point<double>;
	using polygon = geometry::polygon<point>;
	using multipolygon = geometry::multi_polygon<polygon>;

	const entity::system & m_system;

	entity::vector_property<multipolygon> m_geometries;
	entity::vector_property<point> m_positions;

public:
	cells(openeda::netlist::netlist * netlist);
	virtual ~cells();

	void position(entity::entity cell, point position);
	point position(entity::entity cell) const {
		return m_positions[m_system.lookup(cell)];
	}
	std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > positions() const {
		return std::make_pair(m_positions.begin(), m_positions.end());
	}

	void geometry(entity::entity cell, multipolygon geometry);
	multipolygon geometry(entity::entity cell) const {
		return m_geometries[m_system.lookup(cell)];
	}
	std::pair< std::vector<multipolygon>::const_iterator, std::vector<multipolygon>::const_iterator > geometries() const {
		return std::make_pair(m_geometries.begin(), m_geometries.end());
	}
};

} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_CELLS_H_ */
