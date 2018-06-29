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

#ifndef OPHIDIAN_TIMING_TIMINGFACTORY_H
#define OPHIDIAN_TIMING_TIMINGFACTORY_H

#include <ophidian/timing/StaticTimingAnalysis.h>

namespace ophidian
{
namespace timing
{
    namespace factory
    {
        using sta_type = StaticTimingAnalysis;

         sta_type make_static_timing_analysis(sta_type::standard_cells_type& std_cells,
                                              sta_type::netlist_type& netlist,
                                              sta_type::library_mapping_type& lib_mapping,
                                              sta_type::placement_type& placement,
                                              sta_type::placment_library_type& placement_mapping,
                                              const sta_type::liberty_type& early,
                                              const sta_type::liberty_type& late,
                                              const sta_type::lef_type& lef,
                                              const sta_type::design_constraints_type& dc) noexcept;
    }
}     // namespace TIMING
}     // namespace ophidian

#endif // OPHIDIAN_TIMING_DEF2TIMING_H
