/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef SRC_PLACEMENT_LIBRARY_H_
#define SRC_PLACEMENT_LIBRARY_H_

#include "../entity/property.h"
#include "../entity/vector_property.h"
#include "../standard_cell/standard_cells.h"
#include "../geometry/geometry.h"
#include <boost/bimap.hpp>
#include <vector>

namespace ophidian {
namespace placement {

class library_pins {

};

class library {

	using point = geometry::point<double>;
	using polygon = geometry::polygon<point>;
	using multipolygon = geometry::multi_polygon<polygon>;

	const entity::system & m_cell_system;
	const entity::system & m_pin_system;


	entity::vector_property< multipolygon > m_cell_geometry;

	entity::vector_property< point > m_pin_offset;


    int32_t m_dist2microns;


public:
	library(ophidian::standard_cell::standard_cells * std_cells);
	virtual ~library();

	multipolygon geometry(entity::entity cell) const {
		return m_cell_geometry[m_cell_system.lookup(cell)];
	}

	void geometry(entity::entity cell, multipolygon geometry);

    int32_t dist2microns() const {
        return m_dist2microns;
    }

    // pins
    void pin_offset(entity::entity pin, point offset);
    point pin_offset(entity::entity pin) const {
    	return m_pin_offset[m_pin_system.lookup(pin)];
    }

    void dist2microns(int32_t dist);


};

} /* namespace placement */
} /* namespace ophidian */

#endif /* SRC_PLACEMENT_LIBRARY_H_ */
