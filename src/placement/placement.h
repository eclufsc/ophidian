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
			entity::entity cell) const {
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
	void cell_position(entity::entity cell,
			geometry::point<double> position);
	/// Cell position getter.
	/**
	 * Returns the position of a cell.
	 * \param cell Cell to get the position.
	 * \return Point describing the cell position.
	 */
	geometry::point<double> cell_position(entity::entity cell) const {
		return m_cells.position(cell);
	}
	void cell_fixed(entity::entity cell, bool fixed);
	bool cell_fixed(entity::entity cell) const {
		return m_cells.fixed(cell);
	}

	/// Pin position getter.
	/**
	 * Returns the position of a pin, calculated from the cell position and the pin offset.
	 * \param pin Pin to get the position.
	 * \return Point describing the pin position.
	 */
	geometry::point<double> pin_position(entity::entity pin) const {
		entity::entity owner = m_netlist->pin_owner(pin);
		entity::entity std_cell_pin = m_netlist->pin_std_cell(pin);

		geometry::point<double> position = m_library->pin_offset(std_cell_pin);
		if(!(owner == entity::entity{}))
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
	void pad_position(entity::entity pad, geometry::point<double> position);


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
