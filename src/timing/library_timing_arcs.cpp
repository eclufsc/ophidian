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

#include "library_timing_arcs.h"

namespace ophidian {
namespace timing {

library_timing_arcs::library_timing_arcs(standard_cell::standard_cells * std_cells) :
		m_std_cells(*std_cells) {

	m_system.register_property(&m_from);
	m_system.register_property(&m_to);
	m_std_cells.register_pin_property(&m_pin_timing_arcs);
}

library_timing_arcs::~library_timing_arcs() {
}

entity_system::entity library_timing_arcs::create(entity_system::entity from, entity_system::entity to) {
	auto pair = std::make_pair(from, to);
	auto result = m_pinpair2arc.find(pair);
	if (result != m_pinpair2arc.end())
		return result->second;

        entity_system::entity arc = m_system.create();
	m_pinpair2arc[pair] = arc;
	m_from[m_system.lookup(arc)] = from;
	m_to[m_system.lookup(arc)] = to;
	m_pin_timing_arcs[m_std_cells.pin_system().lookup(from)].push_back(arc);
	m_pin_timing_arcs[m_std_cells.pin_system().lookup(to)].push_back(arc);
	return arc;
}

entity_system::entity library_timing_arcs::get(entity_system::entity from, entity_system::entity to) const {
	return m_pinpair2arc.at(std::make_pair(from, to));
}

void library_timing_arcs::register_property(entity_system::property* property) {
	m_system.register_property(property);
}

} /* namespace timing */
} /* namespace ophidian */

