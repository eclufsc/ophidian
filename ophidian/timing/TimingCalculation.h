/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_TIMING_TIMINGCALCULATION_H
#define OPHIDIAN_TIMING_TIMINGCALCULATION_H

#include <ophidian/timing/GenericSTA.h>

namespace ophidian
{
namespace timing
{

class WorstNegativeSlack
{
public:
    using time_unit_type = util::second_t;

    template <class POsContainer, class WireDelayModel, class MergeStrategy>
    WorstNegativeSlack(const POsContainer& POs, const GenericSTA<WireDelayModel, MergeStrategy> & sta) :
        m_value(std::numeric_limits<time_unit_type>::max())
    {
        for(auto PO : POs)
            m_value = units::math::min(m_value, units::math::min(sta.rise_slack(PO), sta.fall_slack(PO)));
    }

    virtual ~WorstNegativeSlack();

    const time_unit_type value() const;

private:
    time_unit_type m_value;
};

class TotalNegativeSlack
{
public:
    using time_unit_type = util::second_t;

    template <class POsContainer, class WireDelayModel, class MergeStrategy>
    TotalNegativeSlack(const POsContainer& POs, const GenericSTA<WireDelayModel, MergeStrategy> & sta) :
        m_value(0.0)
    {
        for(auto PO : POs)
            m_value += units::math::min(time_unit_type(0), units::math::min(sta.rise_slack(PO), sta.fall_slack(PO)));
    }

    virtual ~TotalNegativeSlack();

    const time_unit_type value() const;

private:
    time_unit_type m_value;
};

}   // namespace timing
}   // namespace ophidian

#endif // TIMINGCALCULATION_H
