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
 *
*/

#ifndef OPHIDIAN_TIMING_WNS_H
#define OPHIDIAN_TIMING_WNS_H

#include <boost/units/systems/si.hpp>
#include <boost/units/limits.hpp>
#include "generic_sta.h"

namespace ophidian {
namespace timing {

class wns
{
    boost::units::quantity< boost::units::si::time > m_value;
public:
    template <class POsContainer, class WireDelayModel, class MergeStrategy>
    wns(const POsContainer& POs, const generic_sta<WireDelayModel, MergeStrategy> & sta) :
        m_value(std::numeric_limits<boost::units::quantity< boost::units::si::time > >::max())
    {
        for(auto PO : POs)
            m_value = std::min(m_value, std::min(sta.rise_slack(PO), sta.fall_slack(PO)));
    }
    virtual ~wns();

    const boost::units::quantity< boost::units::si::time > value() const {
        return m_value;
    }
};

}
}

#endif // OPHIDIAN_TIMING_WNS_H
