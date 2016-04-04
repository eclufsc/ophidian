/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
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

    std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> create_tree(const placement::placement& placement,
                                                                                           const entity::entity net, interconnection::rc_tree& rc_tree, const timing::library & library);

};

} /* namespace timingdriven_placement */
} /* namespace openeda */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_FLUTE_RC_TREE_ESTIMATION_H_ */

