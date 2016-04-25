//
// Created by renan on 3/15/16.
//

#ifndef OPENEDA_ABU_H
#define OPENEDA_ABU_H

#include "density_map.h"
#include "placement.h"
#include "floorplan.h"

namespace openeda {
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
