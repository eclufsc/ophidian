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

#ifndef SRC_TIMING_GRAPH_NODES_TIMING_H_
#define SRC_TIMING_GRAPH_NODES_TIMING_H_

#include <lemon/list_graph.h>
#include <boost/units/systems/si.hpp>
#include "../entity_system/entity_system.h"

namespace ophidian {
namespace timing {

class graph_nodes_timing {
	lemon::ListDigraph::NodeMap<boost::units::quantity<boost::units::si::time> > m_arrivals;
	lemon::ListDigraph::NodeMap<boost::units::quantity<boost::units::si::time> > m_slews;
	lemon::ListDigraph::NodeMap<boost::units::quantity<boost::units::si::time> > m_requireds;
	lemon::ListDigraph::NodeMap<boost::units::quantity<boost::units::si::capacitance> > m_loads;
public:
	graph_nodes_timing(const lemon::ListDigraph & graph);
	virtual ~graph_nodes_timing();

	void arrival(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> arrival);
	void slew(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> slew);
	void required(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> required);
	void load(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::capacitance> load);

	boost::units::quantity<boost::units::si::time> arrival(lemon::ListDigraph::Node node) const {
		return m_arrivals[node];
	}
	boost::units::quantity<boost::units::si::time> slew(lemon::ListDigraph::Node node) const {
		return m_slews[node];
	}
	boost::units::quantity<boost::units::si::time> required(lemon::ListDigraph::Node node) const {
		return m_requireds[node];
	}
	boost::units::quantity<boost::units::si::capacitance> load(lemon::ListDigraph::Node node) const {
		return m_loads[node];
	}
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_GRAPH_NODES_TIMING_H_ */
