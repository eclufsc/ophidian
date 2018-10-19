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

#ifndef OPHIDIAN_TIMING_OPENTIMER_H
#define OPHIDIAN_TIMING_OPENTIMER_H

#include <ophidian/design/Design.h>
#include <ot/timer/timer.hpp>

namespace ophidian::timing
{
    class OpenTimer{
    public:
        //Constructors
        OpenTimer() = delete;

        OpenTimer(const OpenTimer&) = default;
        OpenTimer& operator=(const OpenTimer&) = default;

        OpenTimer(OpenTimer&&) = default;
        OpenTimer& operator=(OpenTimer&&) = default;

        OpenTimer(ophidian::design::Design & design, std::string early_lib_path, std::string late_lib_path, std::string sdc_path, std::string spef_path);

        //Timing info queries
        float wns();

        //Design modifiers
        void update_timing();


    private:
        ot::Timer m_timer;
        ophidian::design::Design & m_design;
    };
}

#endif // OPHIDIAN_TIMING_OPENTIMER_H
