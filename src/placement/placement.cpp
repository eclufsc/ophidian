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

#include <placement.h>

namespace ophidian {
namespace placement {

placement::placement(netlist::netlist* netlist, library* lib) :
		m_netlist(netlist), m_library(lib), m_cells(netlist) {
}

placement::~placement() {
}

void placement::cell_position(entity::entity cell,
		geometry::point<double> position) {
	if (!cell_fixed(cell)) {
		m_cells.position(cell, position);
		auto lib_geometry = m_library->geometry(m_netlist->cell_std_cell(cell));
		geometry::multi_polygon<geometry::polygon<geometry::point<double> > > translated;
		geometry::translate(lib_geometry, position, translated);
		m_cells.geometry(cell, translated);
	}
}

void placement::pad_position(entity::entity pad,
		geometry::point<double> position) {
	m_library->pin_offset(m_netlist->pin_std_cell(pad), position);
}

	void placement::cell_fixed(entity::entity cell, bool fixed) {
		m_cells.fixed(cell, fixed);
	}
} /* namespace placement */
} /* namespace ophidian */

