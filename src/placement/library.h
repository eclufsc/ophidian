/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#ifndef SRC_PLACEMENT_LIBRARY_H_
#define SRC_PLACEMENT_LIBRARY_H_

#include "../entity_system/property.h"
#include "../entity_system/vector_property.h"
#include "../standard_cell/standard_cells.h"
#include "../geometry/geometry.h"
#include <boost/bimap.hpp>
#include <vector>

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
    entity_system::vector_property< multipolygon > m_cell_geometry;
    entity_system::vector_property< point > m_pin_offset;

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
    multipolygon geometry(entity_system::entity cell) const {
        return m_cell_geometry[m_std_cells.cell_system().lookup(cell)];
	}
	/// Cell geometry setter.
	/**
	 * Sets the geometry of a cell.
	 * \param cell Cell to set the geometry.
	 * \param geometry Multi polygon representing the cell geometry.
	 */
    void geometry(entity_system::entity cell, multipolygon geometry);


    entity_system::entity cell_create(std::string name);

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
    void pin_offset(entity_system::entity pin, point offset);
	/// Pin offset getter.
	/**
	 * Returns the offset of a pin with relation to its owner cell.
	 * \param pin Pin to get the offset.
	 * \return Point describing the pin offset.
	 */
    point pin_offset(entity_system::entity pin) const {
        return m_pin_offset[m_std_cells.pin_system().lookup(pin)];
    }

    entity_system::entity pin_create(entity_system::entity cell, std::string name);

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
