/*
 * library.h
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#ifndef SRC_PLACEMENT_LIBRARY_H_
#define SRC_PLACEMENT_LIBRARY_H_

#include <property.h>
#include <boost/bimap.hpp>
#include <vector>
#include <standard_cells.h>
#include <vector_property.h>
#include "../geometry/geometry.h"

namespace openeda {
namespace placement {

class library {

	using point = geometry::point<double>;
	using polygon = geometry::polygon<point>;
	using multipolygon = geometry::multi_polygon<polygon>;

	const entity::system & m_system;

	entity::vector_property< multipolygon > m_geometries;

    int32_t m_dist2microns;


public:
	library(openeda::standard_cell::standard_cells * std_cells);
	virtual ~library();

	multipolygon geometry(entity::entity cell) const {
		return m_geometries[m_system.lookup(cell)];
	}

	void geometry(entity::entity cell, multipolygon geometry);

    int32_t dist2microns() const {
        return m_dist2microns;
    }

    void dist2microns(int32_t dist);


};

} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_LIBRARY_H_ */
