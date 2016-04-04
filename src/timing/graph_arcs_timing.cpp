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

#include "graph_arcs_timing.h"

namespace ophidian {
namespace timing {

using namespace boost::units;

graph_arcs_timing::graph_arcs_timing(const lemon::ListDigraph & graph) :
         m_delays(graph), m_slews(graph) {
}

graph_arcs_timing::~graph_arcs_timing() {
}

void graph_arcs_timing::delay(lemon::ListDigraph::Arc arc, const quantity<si::time> delay) {
	m_delays[arc] = delay;
}

void graph_arcs_timing::slew(lemon::ListDigraph::Arc arc, const quantity<si::time> delay) {
	m_slews[arc] = delay;
}


} /* namespace timing */
} /* namespace openeda */

