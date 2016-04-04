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

#include "graph_nodes_timing.h"

namespace ophidian {
namespace timing {

graph_nodes_timing::graph_nodes_timing(const lemon::ListDigraph & graph) :
		m_arrivals(graph), m_slews(graph), m_requireds(graph), m_loads(graph) {

}

graph_nodes_timing::~graph_nodes_timing() {
}

void graph_nodes_timing::arrival(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> arrival) {
	m_arrivals[node] = arrival;
}

void graph_nodes_timing::slew(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> slew) {
	m_slews[node] = slew;
}

void graph_nodes_timing::required(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> required) {
	m_requireds[node] = required;
}

void graph_nodes_timing::load(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::capacitance> load) {
	m_loads[node] = load;
}

} /* namespace timing */
} /* namespace ophidian */

