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
} /* namespace ophidian */

