/*
 * graph_builder.h
 *
 *  Created on: Mar 24, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_GRAPH_BUILDER_H_
#define SRC_TIMING_GRAPH_BUILDER_H_

#include "../netlist/netlist.h"
#include "graph.h"
#include "library.h"
#include "design_constraints.h"

namespace openeda {
namespace timing {

class graph_builder {
public:
	static void build(const netlist::netlist & netlist, library & lib, const timing::design_constraints & dc, graph& graph);
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_GRAPH_BUILDER_H_ */
