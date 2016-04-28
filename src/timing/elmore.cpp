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

#include <elmore.h>

#include <queue>
#include <set>

namespace ophidian {
namespace timing {
using rc_tree = interconnection::rc_tree;
using namespace boost::units;

elmore::elmore(const rc_tree& tree, rc_tree::capacitor_id source) :
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
	m_elmore_delay[m_source] = 0.0 * si::seconds;
	for (auto c : m_order) {
		if (m_pred[c].first != lemon::INVALID)
			m_elmore_delay[c] = m_elmore_delay[m_pred[c].first] + m_tree.resistance(m_pred[c].second) * m_downstream_capacitance[c];
	}
}

} /* namespace timing */
} /* namespace ophidian */
