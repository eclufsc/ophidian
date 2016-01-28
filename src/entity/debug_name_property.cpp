/*
 * debug_name_property.cpp
 *
 *  Created on: Jan 27, 2016
 *      Author: csguth
 */

#include "debug_name_property.h"

namespace openeda {
namespace entity {

debug_name_property::debug_name_property(system * s) {
	s->register_property(this);
}

debug_name_property::~debug_name_property() {
}

void debug_name_property::destroy(entity e) {
	std::size_t to_destroy_index = m_entity2index[e];
	std::size_t last_index = m_names.size()-1;

	m_names[to_destroy_index] = m_names[last_index];
	m_index2entity[to_destroy_index] = m_index2entity.back();

	m_names.pop_back();
	m_index2entity.pop_back();
	m_entity2index.erase(e);
}

void debug_name_property::create(entity e) {
	m_entity2index[e] = m_names.size();
	m_names.push_back("default name");
}

void debug_name_property::set(entity e, std::string name) {
	m_names[m_entity2index[e]] = name;
}

std::string debug_name_property::get(entity e) const {
	return m_names[m_entity2index.at(e)];
}

} /* namespace entity */
} /* namespace openeda */
