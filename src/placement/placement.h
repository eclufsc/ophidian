/*
 * placement.h
 *
 *  Created on: Feb 4, 2016
 *      Author: csguth
 */

#ifndef SRC_PLACEMENT_PLACEMENT_H_
#define SRC_PLACEMENT_PLACEMENT_H_

#include "../netlist/netlist.h"
#include "library.h"
#include "cells.h"

namespace openeda {
namespace placement {

class placement {
	netlist::netlist * m_netlist;
	library * m_library;
	cells m_cells;
	// pins
public:
	placement(netlist::netlist * netlist, library * lib);
	virtual ~placement();
	geometry::multi_polygon<geometry::polygon<geometry::point<double> > > cell_geometry(
			entity::entity cell) const {
		return m_cells.geometry(cell);
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

};

} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_PLACEMENT_H_ */
