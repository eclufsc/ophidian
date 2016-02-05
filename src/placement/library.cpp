/*
 * library.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#include "library.h"

namespace openeda {
namespace placement {

library::library(openeda::standard_cell::standard_cells * std_cells) {
	std_cells->register_property(this);
}

library::~library() {
}

void library::create(entity::entity e, std::size_t index) {
	m_mapping.insert(entity2index_map::value_type(e, m_geometries.size()));
	m_geometries.push_back(multipolygon());
}

void library::destroy(entity::entity e, std::size_t index) {
	std::size_t to_destroy = m_mapping.left.at(e);
	std::size_t last = m_geometries.size()-1;
	entity::entity last_entity = m_mapping.right.at(last);
	m_geometries[to_destroy] = m_geometries[last];
	m_geometries.pop_back();
	m_mapping.left.erase(e);
	m_mapping.right.erase(last);
	m_mapping.insert(entity2index_map::value_type(last_entity, to_destroy));
}

void library::geometry(entity::entity e, multipolygon geometry) {
	m_geometries[m_mapping.left.at(e)] = geometry;
}

} /* namespace placement */
} /* namespace openeda */
