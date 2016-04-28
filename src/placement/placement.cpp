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

