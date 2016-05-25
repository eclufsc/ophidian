/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#ifndef SRC_PLACEMENT_CELLS_H_
#define SRC_PLACEMENT_CELLS_H_


#include "../netlist/netlist.h"
#include "../geometry/geometry.h"
#include "../entity_system/vector_property.h"
#include <boost/bimap.hpp>

namespace ophidian {
namespace placement {

class cells  {
	using point = geometry::point<double>;
	using polygon = geometry::polygon<point>;
	using multipolygon = geometry::multi_polygon<polygon>;

    const entity_system::entity_system & m_system;

    entity_system::vector_property<multipolygon> m_geometries;
    entity_system::vector_property<point> m_positions;
    entity_system::vector_property<bool> m_fixed;

public:
	cells(ophidian::netlist::netlist * netlist);
	virtual ~cells();

    void position(entity_system::entity cell, point position);
    point position(entity_system::entity cell) const {
		return m_positions[m_system.lookup(cell)];
	}
	const entity_system::vector_property<point> positions() const {
		return m_positions;
	}

    void geometry(entity_system::entity cell, multipolygon geometry);
    multipolygon geometry(entity_system::entity cell) const {
        return m_geometries[m_system.lookup(cell)];
    }
    const entity_system::vector_property<multipolygon> geometries() const {
        return m_geometries;
    }

    void fixed(entity_system::entity cell, bool fixed);
    bool fixed(entity_system::entity cell) const {
		return m_fixed[m_system.lookup(cell)];
	}
};

} /* namespace placement */
} /* namespace ophidian */

#endif /* SRC_PLACEMENT_CELLS_H_ */
