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

            floorplan::floorplan & m_floorplan;
            placement::placement & m_placement;

            point m_bin_dimensions;

            void build_density_map();

            double measure_abu_for_range(double range, std::vector<double> & utilizations);
        public:

            abu(floorplan::floorplan & floorplan, placement::placement & placement, point bin_dimensions)
                    : m_floorplan(floorplan), m_placement(placement), m_bin_dimensions(bin_dimensions) {

            }

            ~abu() { }

            double measure_abu(double target_utilization, std::vector<double> abu_ranges, std::vector<double> abu_weights);
        };
    }
}



#endif //OPENEDA_ABU_H
