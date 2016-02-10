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

	const cells & cells_properties() const {
		return m_cells;
	}

};

} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_PLACEMENT_H_ */
