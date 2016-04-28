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
