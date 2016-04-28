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

#include "lef.h"

namespace ophidian {
namespace placement {

class library_pins {

};

/// Placement library class.
/**
 * This class provides the interface to placement library properties, such as cell geometries and pin offsets.
 */
class library {

	using point = geometry::point<double>;
	using polygon = geometry::polygon<point>;
	using multipolygon = geometry::multi_polygon<polygon>;

    ophidian::standard_cell::standard_cells & m_std_cells;
	entity::vector_property< multipolygon > m_cell_geometry;
	entity::vector_property< point > m_pin_offset;

    int32_t m_dist2microns;

public:
	/// Constructor.
	/**
	 * Placement library constructor. Register the cell geometries and pin offsets properties.
	 * \param std_cells Standard cell library.
	 */
	library(ophidian::standard_cell::standard_cells * std_cells);
	virtual ~library();

	/// Cell geometry getter.
	/**
	 * Returns the geometry of a cell as a multi polygon. Each polygon in the multi polygon represents a rectangle of the cell.
	 * \param cell Cell to get the geometry.
	 * \return Multi polygon representing the cell geometry.
	 */
	multipolygon geometry(entity::entity cell) const {
        return m_cell_geometry[m_std_cells.cell_system().lookup(cell)];
	}
	/// Cell geometry setter.
	/**
	 * Sets the geometry of a cell.
	 * \param cell Cell to set the geometry.
	 * \param geometry Multi polygon representing the cell geometry.
	 */
	void geometry(entity::entity cell, multipolygon geometry);


    entity::entity cell_create(std::string name);

	/// Distance unit to microns ratio getter.
	/**
	 * Returns the number of microns in a distance unit of the library.
	 * \return Number of microns in a distance unit.
	 */
    int32_t dist2microns() const {
        return m_dist2microns;
    }

    // pins
	/// Pin offset setter.
	/**
	 * Sets the offset of a pin with relation to its owner cell.
	 * \param pin Pin to set the offset.
	 * \param offset Point describing the pin offset.
	 */
    void pin_offset(entity::entity pin, point offset);
	/// Pin offset getter.
	/**
	 * Returns the offset of a pin with relation to its owner cell.
	 * \param pin Pin to get the offset.
	 * \return Point describing the pin offset.
	 */
    point pin_offset(entity::entity pin) const {
        return m_pin_offset[m_std_cells.pin_system().lookup(pin)];
    }

    entity::entity pin_create(entity::entity cell, std::string name);

	/// Distance unit to microns ratio setter.
	/**
	 * Sets the number of microns in a distance unit of the library.
	 * \param dist Number of microns in a distance unit.
	 */
    void dist2microns(int32_t dist);

};

} /* namespace placement */
} /* namespace ophidian */

#endif /* SRC_PLACEMENT_LIBRARY_H_ */
