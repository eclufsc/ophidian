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

#ifndef OPENEDA_ABU_H
#define OPENEDA_ABU_H

#include "density_map.h"
#include "placement.h"
#include "floorplan.h"

namespace ophidian {
    namespace density {
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



#endif //OPENEDA_ABU_H
