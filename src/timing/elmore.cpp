/*
 * elmore.cpp
 *
 *  Created on: Mar 11, 2016
 *      Author: csguth
 */

#include <elmore.h>

#include <queue>
#include <set>

namespace openeda {
namespace timing {
using rc_tree = interconnection::rc_tree;
using namespace boost::units;

elmore::elmore(rc_tree& tree, rc_tree::capacitor_id source) :
		m_tree(tree), m_elmore_delay(tree.graph()), m_downstream_capacitance(tree.graph()), m_pred(tree.graph()), m_source(source) {

	std::deque<rc_tree::capacitor_id> to_process;
	std::set<rc_tree::capacitor_id> reached;
	m_pred[source] = std::make_pair(lemon::INVALID, lemon::INVALID);
	reached.insert(source);
	to_process.push_back(source);
	while (!to_process.empty()) {
		auto element = to_process.front();
		to_process.pop_front();
		m_order.push_back(element);
		for (rc_tree::resistor_it it { tree.capacitor_resistors(element) }; it != rc_tree::invalid(); ++it) {
			auto other = tree.other_capacitor(static_cast<rc_tree::resistor_id>(it), element);
			if (reached.insert(other).second) {
				m_pred[other] = std::make_pair(element, static_cast<rc_tree::resistor_id>(it));
				std::cout << tree.capacitor_name(other) << " pred = " << tree.capacitor_name(element) << std::endl;

				to_process.push_back(other);
			}
		}
	}
	assert(m_order.size() == lemon::countNodes(tree.graph()));

	update();
}

elmore::~elmore() {
}

void elmore::update() {
	for (auto c : m_order)
		m_downstream_capacitance[c] = m_tree.capacitance(c);

	for (std::vector<rc_tree::capacitor_id>::reverse_iterator it = m_order.rbegin(); it != m_order.rend(); ++it) {
		if (m_pred[*it].first != lemon::INVALID)
			m_downstream_capacitance[m_pred[*it].first] += m_downstream_capacitance[*it];
	}

	for (auto c : m_order)
		std::cout << "cap " << m_tree.capacitor_name(c) << " cap " << m_tree.capacitance(c) << " downstream " << m_downstream_capacitance[c] << std::endl;

	m_elmore_delay[m_source] = 0.0 * si::seconds;
	for (auto c : m_order) {
		if (m_pred[c].first != lemon::INVALID)
			m_elmore_delay[c] = m_elmore_delay[m_pred[c].first] + m_tree.resistance(m_pred[c].second) * m_downstream_capacitance[c];
	}
}

} /* namespace timing */
} /* namespace openeda */
