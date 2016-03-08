/*
 * library.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#include "library.h"

namespace openeda {
namespace placement {

library::library(openeda::standard_cell::standard_cells * std_cells) :
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
} /* namespace openeda */

