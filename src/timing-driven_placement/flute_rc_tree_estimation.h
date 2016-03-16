/*
 * flute_rc_tree_estimation.h
 *
 *  Created on: Mar 15, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_
#define SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_

#include "../interconnection/rc_tree.h"
#include "../placement/placement.h"

namespace openeda {
namespace timingdriven_placement {

void flute_rc_tree(const placement::placement& placement, const entity::entity net, interconnection::rc_tree& tree);

} /* namespace timingdriven_placement */
} /* namespace openeda */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_ */
