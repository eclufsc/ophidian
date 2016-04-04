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

#include "library.h"

namespace ophidian {
namespace placement {

library::library(ophidian::standard_cell::standard_cells * std_cells) :
		m_cell_system(std_cells->cell_system()), m_pin_system(
				std_cells->pin_system()) {
	std_cells->register_cell_property(&m_cell_geometry);
	std_cells->register_pin_property(&m_pin_offset);
}

library::~library() {
}

void library::geometry(entity::entity cell, multipolygon geometry) {
	m_cell_geometry[m_cell_system.lookup(cell)] = geometry;
}

void library::dist2microns(int32_t dist) {
	m_dist2microns = dist;
}
void library::pin_offset(entity::entity pin, point offset) {
	m_pin_offset[m_pin_system.lookup(pin)] = offset;
}


} /* namespace placement */
} /* namespace ophidian */

