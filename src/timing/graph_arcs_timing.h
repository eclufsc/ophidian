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

#ifndef SRC_TIMING_GRAPH_ARCS_TIMING_H_
#define SRC_TIMING_GRAPH_ARCS_TIMING_H_

#include <lemon/list_graph.h>
#include <boost/units/systems/si.hpp>
#include "transition.h"
namespace ophidian {
namespace timing {

class graph_arcs_timing {
	lemon::ListDigraph::ArcMap<boost::units::quantity<boost::units::si::time> > m_delays;
	lemon::ListDigraph::ArcMap<boost::units::quantity<boost::units::si::time> > m_slews;
public:
	graph_arcs_timing(const lemon::ListDigraph & graph);
	virtual ~graph_arcs_timing();
	void delay(lemon::ListDigraph::Arc, const boost::units::quantity<boost::units::si::time> delay);
	const boost::units::quantity<boost::units::si::time> delay(lemon::ListDigraph::Arc arc) const {
		return m_delays[arc];
	}
	void slew(lemon::ListDigraph::Arc, const boost::units::quantity<boost::units::si::time> delay);
	const boost::units::quantity<boost::units::si::time> slew(lemon::ListDigraph::Arc arc) const {
		return m_slews[arc];
	}
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_GRAPH_ARCS_TIMING_H_ */
