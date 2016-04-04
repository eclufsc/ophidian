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
namespace placement {

class placement {
	netlist::netlist * m_netlist;
	library * m_library;
	cells m_cells;
public:
	placement(netlist::netlist * netlist, library * lib);
	virtual ~placement();
	geometry::multi_polygon<geometry::polygon<geometry::point<double> > > cell_geometry(
			entity::entity cell) const {
        auto position = cell_position(cell);
        auto lib_geometry = m_library->geometry(m_netlist->cell_std_cell(cell));
        geometry::multi_polygon<geometry::polygon<geometry::point<double> > > translated;
        geometry::translate(lib_geometry, position, translated);
        return translated;
	}
	void cell_position(entity::entity cell,
			geometry::point<double> position);
	geometry::point<double> cell_position(entity::entity cell) const {
		return m_cells.position(cell);
	}


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

	void pad_position(entity::entity pad, geometry::point<double> position);


	const netlist::netlist& netlist() const
	{
		return *m_netlist;
	}

	const library& lib() const {
		return *m_library;
	}

};

} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_PLACEMENT_H_ */
