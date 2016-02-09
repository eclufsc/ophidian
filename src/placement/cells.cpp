/*
 * cells.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#include "cells.h"

namespace openeda {
namespace placement {

cells::cells(openeda::netlist::netlist * netlist) : m_system(netlist->cell_system()) {
	netlist->register_cell_property(&m_positions);
	netlist->register_cell_property(&m_geometries);
}

cells::~cells() {
}

void cells::position(entity::entity cell, point position) {
	m_positions[m_system.lookup(cell)] = position;
}

void cells::geometry(entity::entity cell,
		multipolygon geometry) {
	m_geometries[m_system.lookup(cell)] = geometry;
}

} /* namespace placement */
} /* namespace openeda */
