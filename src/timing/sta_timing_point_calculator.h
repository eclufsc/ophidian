/*
 * sta_timing_point_calculator.h
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#ifndef TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_
#define TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_

#include "graph_nodes_timing.h"
#include "graph_arcs_timing.h"

namespace openeda {
namespace timing {

class sta_timing_point_calculator {
public:
	sta_timing_point_calculator();
	virtual ~sta_timing_point_calculator();


	void update( const lemon::ListDigraph & graph, const graph_arcs_timing & arcs_timing, graph_nodes_timing & nodes_timing, lemon::ListDigraph::Node node );
};

} /* namespace timing */
} /* namespace openeda */

#endif /* TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_ */
