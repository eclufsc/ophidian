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
#include "../timing/library.h"

#include <unordered_map>

namespace openeda {
namespace timingdriven_placement {

std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> flute_rc_tree(const placement::placement& placement,
		const entity::entity net, interconnection::rc_tree& rc_tree, timing::library & library);

} /* namespace timingdriven_placement */
} /* namespace openeda */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_ */
