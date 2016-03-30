/*
 * elmore_second_moment.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: csguth
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

namespace openeda {
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
