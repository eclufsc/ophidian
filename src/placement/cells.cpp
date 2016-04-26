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

#include "cells.h"

namespace ophidian {
namespace placement {

cells::cells(netlist::netlist * netlist) : m_system(netlist->cell_system()), m_fixed(false) {
	netlist->register_cell_property(&m_positions);
	netlist->register_cell_property(&m_geometries);
	netlist->register_cell_property(&m_fixed);
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

void cells::fixed(entity::entity cell, bool fixed) {
	m_fixed[m_system.lookup(cell)] = fixed;
}
} /* namespace placement */
} /* namespace ophidian */
