/*
 * cells.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#include "cells.h"

namespace openeda {
namespace placement {

cells::cells(openeda::netlist::netlist * netlist) {
	netlist->register_cell_property(this);
	for (auto c : netlist->cell_system())
		create(c.second);
}

cells::~cells() {
}

void cells::create(entity::entity e) {
	m_mapping.insert(
			boost::bimap<entity::entity, std::size_t>::value_type(e,
					m_positions.size()));
	geometry::point<double> point;
	point.x(0.0); point.y(0.0);
	m_positions.push_back(point);
	m_geometries.resize(m_geometries.size() + 1);
}

void cells::destroy(entity::entity e) {
	std::size_t to_destroy = m_mapping.left.at(e);
	std::size_t last = m_geometries.size() - 1;
	entity::entity last_entity = m_mapping.right.at(last);
	m_geometries[to_destroy] = m_geometries[last];
	m_positions[to_destroy] = m_positions[last];
	m_geometries.pop_back();
	m_positions.pop_back();
	m_mapping.left.erase(e);
	m_mapping.right.erase(last);
	m_mapping.insert(boost::bimap<entity::entity, std::size_t>::value_type(last_entity, to_destroy));
}

void cells::position(entity::entity e, geometry::point<double> point) {
	m_positions[m_mapping.left.at(e)] = point;
}

void cells::geometry(entity::entity e,
		geometry::multi_polygon<geometry::polygon<geometry::point<double> > > geometry) {
	m_geometries[m_mapping.left.at(e)] = geometry;
}

} /* namespace placement */
} /* namespace openeda */
