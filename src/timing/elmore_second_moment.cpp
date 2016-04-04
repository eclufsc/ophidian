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

#include "elmore_second_moment.h"
#include <boost/units/derived_dimension.hpp>
#include <boost/units/physical_dimensions/length.hpp>
#include <boost/units/physical_dimensions/mass.hpp>
#include <boost/units/physical_dimensions/time.hpp>
#include <boost/units/physical_dimensions/current.hpp>

namespace boost {

namespace units {

typedef derived_dimension<length_base_dimension,-2,
                          mass_base_dimension,-1,
                          time_base_dimension,5,
                          current_base_dimension,2>::type capacitance_time_dimension;


}

}

namespace ophidian {
namespace timing {



typedef boost::units::unit<boost::units::capacitance_time_dimension, boost::units::si::system> capacitance_time_unit;



elmore_second_moment::elmore_second_moment(const interconnection::rc_tree & tree, const elmore& e) :
		m_elmore(e), m_tree(tree), m_second_moment(tree.graph()) {
	update();
}

void elmore_second_moment::update() {

	using namespace boost::units;
	lemon::ListGraph::NodeMap<quantity<capacitance_time_unit> > m_downstream_capacitance(m_tree.graph());

	for (auto c : m_elmore.order())
		m_downstream_capacitance[c] = m_tree.capacitance(c) * m_elmore.at(c);

	for (auto it = m_elmore.order().rbegin(); it != m_elmore.order().rend(); ++it) {
		if (m_elmore.pred()[*it].first != lemon::INVALID)
			m_downstream_capacitance[m_elmore.pred()[*it].first] += m_downstream_capacitance[*it];
	}

	m_second_moment[m_elmore.order().front()] = 0.0 * boost::units::si::seconds*boost::units::si::seconds;
	for (auto c : m_elmore.order()) {
		if (m_elmore.pred()[c].first != lemon::INVALID)
			m_second_moment[c] = m_second_moment[m_elmore.pred()[c].first] + m_tree.resistance(m_elmore.pred()[c].second) * m_downstream_capacitance[c];
	}

}

elmore_second_moment::~elmore_second_moment() {
}

} /* namespace timing */
} /* namespace openeda */
