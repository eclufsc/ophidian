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

#ifndef SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_
#define SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_

#include "../interconnection/rc_tree.h"
#include "../placement/placement.h"
#include "../timing/library.h"

#include <unordered_map>

#include <boost/units/systems/si.hpp>

namespace ophidian {
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

    std::unordered_map<entity_system::entity, interconnection::rc_tree::capacitor_id> create_tree(const placement::placement& placement,
                                                                                           const entity_system::entity net, interconnection::rc_tree& rc_tree, const timing::library & library);

};

} /* namespace timingdriven_placement */
} /* namespace ophidian */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_ */

