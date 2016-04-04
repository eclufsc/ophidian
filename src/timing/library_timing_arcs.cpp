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

entity::entity library_timing_arcs::create(entity::entity from, entity::entity to) {
	auto pair = std::make_pair(from, to);
	auto result = m_pinpair2arc.find(pair);
	if (result != m_pinpair2arc.end())
		return result->second;

	entity::entity arc = m_system.create();
	m_pinpair2arc[pair] = arc;
	m_from[m_system.lookup(arc)] = from;
	m_to[m_system.lookup(arc)] = to;
	m_pin_timing_arcs[m_std_cells.pin_system().lookup(from)].push_back(arc);
	m_pin_timing_arcs[m_std_cells.pin_system().lookup(to)].push_back(arc);
	return arc;
}

entity::entity library_timing_arcs::get(entity::entity from, entity::entity to) const {
	return m_pinpair2arc.at(std::make_pair(from, to));
}

void library_timing_arcs::register_property(entity::property* property) {
	m_system.register_property(property);
}

} /* namespace timing */
} /* namespace openeda */

