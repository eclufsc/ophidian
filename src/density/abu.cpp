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

#include "abu.h"
#include "../../apps/placement_viewer/application.h"

namespace ophidian {
    namespace density {

        double abu::measure_abu(double target_utilization, std::vector<double> abu_ranges, std::vector<double> abu_weights, double bin_area_threshold, double free_space_threshold) {
            assert(abu_ranges.size() == abu_weights.size());

            std::vector<double> utilizations;
            m_density.build_density_map(m_bin_dimensions, utilizations);

            double numerator = 0.0, denominator = 0.0;
            for (size_t abu_range_id = 0; abu_range_id < abu_ranges.size(); abu_range_id++) {
                double abu = measure_abu_for_range(abu_ranges.at(abu_range_id), utilizations, target_utilization);
                numerator += abu * abu_weights.at(abu_range_id);
                denominator += abu_weights.at(abu_range_id);
            }
            double abu = numerator / denominator;
            return abu;
        }

        double abu::measure_abu_for_range(double range, std::vector<double> &utilizations, double target_utilization) {
            size_t number_of_bins = m_density.bin_count();

            double local_abu = 0;
            int clip_index = range * (number_of_bins - m_density.skipped_bins());
            for (size_t bin_index = number_of_bins-1; bin_index > number_of_bins-1-clip_index; bin_index--) {
                local_abu += utilizations.at(bin_index);
            }
            local_abu = (clip_index) ? local_abu / clip_index : utilizations.at(number_of_bins - 1);
            local_abu = std::max(0.0, local_abu / target_utilization - 1.0);
            return local_abu;
        }
    }
}
