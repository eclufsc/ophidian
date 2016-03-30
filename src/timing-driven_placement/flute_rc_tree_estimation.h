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

#include <boost/units/systems/si.hpp>

namespace openeda {
namespace timingdriven_placement {



class flute_rc_tree_creator {
    struct params {
        boost::units::quantity< boost::units::si::resistance > resistance_per_micron;
        boost::units::quantity< boost::units::si::capacitance > capacitance_per_micron;
    };
    params m_params;
  public:
    flute_rc_tree_creator();
    virtual ~flute_rc_tree_creator();
    void resistance_per_microns(boost::units::quantity< boost::units::si::resistance > resistance);
    void capacitance_per_micron(boost::units::quantity< boost::units::si::capacitance > capacitance);

    std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> create_tree(const placement::placement& placement,
                                                                                           const entity::entity net, interconnection::rc_tree& rc_tree, const timing::library & library);

};

} /* namespace timingdriven_placement */
} /* namespace openeda */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_ */

