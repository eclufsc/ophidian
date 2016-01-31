/*
 * standard_cells.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "standard_cells.h"

namespace openeda {
namespace standard_cell {

standard_cells::~standard_cells() {
}

entity::entity standard_cells::create(std::string name) {
	auto result = m_mapping.left.find(name);
	entity::entity e;
	if (result != m_mapping.left.end())
		e = result->second;
	else {
		e = m_system.create();
		m_mapping.insert(
				boost::bimaps::bimap<std::string, entity::entity>::value_type(
						name, e));
	}
	return e;
}

std::vector<std::string> standard_cells::names() const {
	std::vector<std::string> names;
	for (auto e : m_mapping)
		names.push_back(e.left);
	return names;
}

void standard_cells::register_property(
		entity::property* property) {
	m_system.register_property(property);
}

} /* namespace standard_cell */
} /* namespace openeda */

