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

#ifndef SRC_PLACEMENT_PLACEMENT_H_
#define SRC_PLACEMENT_PLACEMENT_H_

#include "../netlist/netlist.h"
#include "library.h"
#include "cells.h"

namespace ophidian {
	/// Namespace describing placement entities and basic placement interface.
namespace placement {
	/// Placement class.
	/**
	 * This class provides the basic placement interface, such as cells and pads position manipulation.
	 */
class placement {
	netlist::netlist * m_netlist;
	library * m_library;
	cells m_cells;
public:
	/// Constructor.
	/**
	 * Placement constructor. Initializes the placement cell properties, such as positions and geometries.
	 * \param netlist Circuit netlist.
	 * \param lib Placement library.
	 */
	placement(netlist::netlist * netlist, library * lib);
	virtual ~placement();
	/// Cell geometry getter.
	/**
	 * Returns the geometry of a cell as a multi polygon. Each polygon in the multi polygon represents a rectangle of the cell.
	 * \param cell Cell to get the geometry.
	 * \return Multi polygon with the cell geometry.
	 */
	geometry::multi_polygon<geometry::polygon<geometry::point<double> > > cell_geometry(
            entity_system::entity cell) const {
        auto position = cell_position(cell);
        auto lib_geometry = m_library->geometry(m_netlist->cell_std_cell(cell));
        geometry::multi_polygon<geometry::polygon<geometry::point<double> > > translated;
        geometry::translate(lib_geometry, position, translated);
        return translated;
	}
	/// Places a cell.
	/**
	 * Changes the position of a cell.
	 * \param cell Cell to move.
	 * \param position Target position of the cell.
	 */
    void cell_position(entity_system::entity cell,
			geometry::point<double> position);
	/// Cell position getter.
	/**
	 * Returns the position of a cell.
	 * \param cell Cell to get the position.
	 * \return Point describing the cell position.
	 */
    geometry::point<double> cell_position(entity_system::entity cell) const {
		return m_cells.position(cell);
	}
    void cell_fixed(entity_system::entity cell, bool fixed);
    bool cell_fixed(entity_system::entity cell) const {
		return m_cells.fixed(cell);
	}
	geometry::point<double> cell_dimensions(entity_system::entity cell) const {
		auto geometry = cell_geometry(cell);
		geometry::box<geometry::point<double>> cell_box;
		boost::geometry::envelope(geometry, cell_box);
		geometry::point<double> dimensions(cell_box.max_corner().x() - cell_box.min_corner().x(), cell_box.max_corner().y() - cell_box.min_corner().y());
		return dimensions;
	}

    entity_system::entity cell_create(std::string name, std::string type);

	/// Pin position getter.
	/**
	 * Returns the position of a pin, calculated from the cell position and the pin offset.
	 * \param pin Pin to get the position.
	 * \return Point describing the pin position.
	 */
    geometry::point<double> pin_position(entity_system::entity pin) const {
        entity_system::entity owner = m_netlist->pin_owner(pin);
        entity_system::entity std_cell_pin = m_netlist->pin_std_cell(pin);

		geometry::point<double> position = m_library->pin_offset(std_cell_pin);
        if(!(owner == entity_system::invalid_entity))
		{
			auto owner_position = m_cells.position(owner);
			position.x( position.x() + owner_position.x() );
			position.y( position.y() + owner_position.y() );
		}
		return position;
	}

	/// Places a pad.
	/**
	 * Changes the position of a pad.
	 * \param pad Pad to move.
	 * \param position Target position of the pad.
	 */
    void pad_position(entity_system::entity pad, geometry::point<double> position);


	/// Netlist getter.
	/**
	 * Returns the netlist used in the placement construction.
	 * \return Constant reference to the netlist object.
	 */
	const netlist::netlist& netlist() const
	{
		return *m_netlist;
	}

	/// Placement library getter.
	/**
	 * Returns the placement library used in the placement construction.
	 * \return Constant reference to the library object.
	 */
	const library& lib() const {
		return *m_library;
	}
};

} /* namespace placement */
} /* namespace ophidian */

#endif /* SRC_PLACEMENT_PLACEMENT_H_ */
