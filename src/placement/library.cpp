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
    m_std_cells(*std_cells) {
    std_cells->register_cell_property(&m_cell_geometry);
    std_cells->register_pin_property(&m_pin_offset);
}

library::~library() {
}

void library::geometry(entity::entity cell, multipolygon geometry) {
    m_cell_geometry[m_std_cells.cell_system().lookup(cell)] = geometry;
}

entity::entity library::cell_create(std::string name)
{
    return m_std_cells.cell_create(name);
}

void library::dist2microns(int32_t dist) {
    m_dist2microns = dist;
}
void library::pin_offset(entity::entity pin, point offset) {
    m_pin_offset[m_std_cells.pin_system().lookup(pin)] = offset;
}

entity::entity library::pin_create(entity::entity cell, std::string name)
{
    return m_std_cells.pin_create(cell, name);
}


} /* namespace placement */
} /* namespace ophidian */

