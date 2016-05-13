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

#include "sta_arc_calculator.h"
#include <boost/units/pow.hpp>
#include <boost/units/cmath.hpp>

namespace ophidian {
namespace timing {


sta_timing_arc_edge_calculator::sta_timing_arc_edge_calculator(const library &lib) :
    m_lib(lib)
{

}

void sta_timing_arc_edge_calculator::update(const graph &g, const graph::edge graph_arc, const graph_nodes_timing &nodes, graph_arcs_timing &arcs)
{
    const entity_system::entity tarc = g.edge_entity(graph_arc);
    boost::units::quantity<boost::units::si::capacitance> target_load = nodes.load(g.edge_target(graph_arc));
    boost::units::quantity<boost::units::si::time> source_slew = nodes.slew(g.edge_source(graph_arc));
    boost::units::quantity<boost::units::si::time> delay, slew;
    switch (g.node_edge( g.edge_target(graph_arc) )) {
    case edges::RISE:
        delay = m_lib.timing_arc_rise_delay(tarc).compute(target_load, source_slew);
        slew = m_lib.timing_arc_rise_slew(tarc).compute(target_load, source_slew);
        break;
    case edges::FALL:
        delay = m_lib.timing_arc_fall_delay(tarc).compute(target_load, source_slew);
        slew = m_lib.timing_arc_fall_slew(tarc).compute(target_load, source_slew);
        break;
    default:
        break;
    }
    arcs.slew(graph_arc, slew);
    arcs.delay(graph_arc, delay);
}


sta_timing_net_edge_calculator::sta_timing_net_edge_calculator(const graph &g) :
    m_elmore_delay(g.G()),
    m_elmore_slew(g.G())
{

}

void sta_timing_net_edge_calculator::elmore_delay(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time > delay)
{
    m_elmore_delay[node] = delay;
}

void sta_timing_net_edge_calculator::elmore_slew(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> slew)
{
    m_elmore_slew[node] = slew;
}

void sta_timing_net_edge_calculator::update(const graph &g, const graph::edge arc, const graph_nodes_timing &nodes, graph_arcs_timing &arcs)
{
    using namespace boost::units;
    using namespace boost::units::si;
    const quantity<si::time> source_slew = nodes.slew(g.edge_source(arc));
    const quantity<si::time> elmore_slew = m_elmore_slew[g.edge_target(arc)];
    // extend to ramp slew
    arcs.delay(arc, elmore_slew);
    arcs.slew(arc, boost::units::sqrt(boost::units::pow<2>(elmore_slew) + boost::units::pow<2>(source_slew)));
}

}
}



