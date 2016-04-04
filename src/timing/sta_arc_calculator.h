/*
 * sta_net_calculator.h
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_STA_ARC_CALCULATOR_H_
#define SRC_TIMING_STA_ARC_CALCULATOR_H_

#include <lemon/list_graph.h>
#include "graph.h"
#include "sta_timing_point_calculator.h"
#include "library.h"
#include "graph_arcs_timing.h"
#include "graph_nodes_timing.h"
#include <unordered_set>

namespace openeda {
namespace timing {

class sta_timing_point_calculator;
class sta_timing_edge_calculator {
protected:
public:
    virtual ~sta_timing_edge_calculator() {}
    virtual void update(const graph & g, const graph::edge arc, const graph_nodes_timing & nodes, graph_arcs_timing & arcs) = 0;
};


class sta_timing_arc_edge_calculator : public sta_timing_edge_calculator {
    const library & m_lib;
public:
    sta_timing_arc_edge_calculator(const library & lib);
    void update(const graph & g, const graph::edge arc, const graph_nodes_timing & nodes, graph_arcs_timing & arcs);
};


class sta_timing_net_edge_calculator : public sta_timing_edge_calculator {
    lemon::ListDigraph::NodeMap< boost::units::quantity< boost::units::si::time > > m_elmore_delay;
    lemon::ListDigraph::NodeMap< boost::units::quantity< boost::units::si::time > > m_elmore_slew;
public:
    sta_timing_net_edge_calculator(const graph & g);
    void elmore_delay(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> delay);
    void elmore_slew( lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time > slew);
    void update(const graph & g, const graph::edge arc, const graph_nodes_timing & nodes, graph_arcs_timing & arcs);
};

class sta_interconnection_estimator {
public:
    virtual void update_net(timing::sta_timing_net_edge_calculator * tnet, entity::entity net, timing::graph_nodes_timing &nodes_timing) = 0;
};



} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_STA_ARC_CALCULATOR_H_ */
