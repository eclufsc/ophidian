/*
 * placement.cpp
 *
 *  Created on: Feb 4, 2016
 *      Author: csguth
 */

#include <placement.h>

namespace openeda {
namespace placement {

placement::placement(netlist::netlist* netlist, library* lib) :
		m_netlist(netlist), m_library(lib), m_cells(netlist) {
}

placement::~placement() {
}

void placement::cell_position(entity::entity cell,
		geometry::point<double> position) {
	m_cells.position(cell, position);
}

void placement::pad_position(entity::entity pad,
		geometry::point<double> position) {
	m_library->pin_offset(m_netlist->pin_std_cell(pad), position);
}

} /* namespace placement */
} /* namespace openeda */

