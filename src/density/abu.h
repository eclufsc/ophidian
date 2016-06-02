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

#ifndef ophidian_ABU_H
#define ophidian_ABU_H

#include "density_map.h"
#include "placement.h"
#include "floorplan.h"

/// The namespace of Ophidian
namespace ophidian {
    /// Classes and algorithms related to density
    namespace density {

        /// ABU, a placement density metric.
        /**
        * ABU is the average bin utilization of the top γ% densest bins excluding bins fully occupied by fixed macros.
        * From: Kim, Myung-Chul, et al. "MAPLE: multilevel adaptive placement for mixed-size designs." Proceedings of the 2012 ACM international symposium on International Symposium on Physical Design. ACM, 2012.
        **/
        class abu {
            using point = geometry::point<double>;

            density_map m_density;

            point m_bin_dimensions;

            double measure_abu_for_range(double range, std::vector<double> & utilizations, double target_utilization);
        public:

            abu(floorplan::floorplan * floorplan, placement::placement * placement, point bin_dimensions)
                    : m_density(floorplan, placement), m_bin_dimensions(bin_dimensions) {

            }

            ~abu() { }

            double measure_abu(double target_utilization, std::vector<double> abu_ranges = {0.02, 0.05, 0.1, 0.2}, std::vector<double> abu_weights = {10, 4, 2, 1}, double bin_area_threshold = 0.2, double free_space_threshold = 0.2);
        };
    }
}



#endif //ophidian_ABU_H
