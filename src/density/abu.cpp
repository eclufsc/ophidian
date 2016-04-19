//
// Created by renan on 3/15/16.
//

#include "abu.h"
#include "../../apps/placement_viewer/application.h"

namespace openeda {
    namespace density {

        double abu::measure_abu(double target_utilization, std::vector<double> abu_ranges, std::vector<double> abu_weights) {
            size_t number_of_bins = m_density.bins_system().entities().size();

            build_density_map();
            std::vector<double> utilizations;
            utilizations.reserve(number_of_bins);
            for (entity::entity bin : m_density.bins_system().entities()) {
                double utilization = m_density.bin_movable_utilization(bin) / m_density.bin_free_space(bin);
                utilizations.push_back(utilization);
            }
            std::sort(utilizations.begin(), utilizations.end());

            assert(abu_ranges.size() == abu_weights.size());
            double numerator = 0.0, denominator = 0.0;
            for (size_t abu_range_id = 0; abu_range_id < abu_ranges.size(); abu_range_id++) {
                double abu = measure_abu_for_range(abu_ranges.at(abu_range_id), utilizations);
                numerator += abu * abu_weights.at(abu_range_id);
                denominator += abu_weights.at(abu_range_id);
            }
            double abu = numerator / denominator;
            return abu;
        }

        void abu::build_density_map() {
            std::pair<unsigned, unsigned> number_of_bins(m_floorplan.chip_boundaries().x() / m_bin_dimensions.x(),
                                                         m_floorplan.chip_boundaries().y() / m_bin_dimensions.y());
            for (unsigned y_index = 0; y_index < number_of_bins.second; y_index++) {
                for (unsigned x_index = 0; x_index < number_of_bins.first; x_index++) {
                    point bin_position(x_index * m_bin_dimensions.x(),
                                       y_index * m_bin_dimensions.y());
                    m_density.bin_insert(bin_position, m_bin_dimensions, 0, 0);
                }
            }

            auto cell_geometries = m_placement.cells_properties().geometries();
            for (auto cell_geometry_it = cell_geometries.first; cell_geometry_it != cell_geometries.second; cell_geometry_it++) {
                density_map::box cell_boundaries;
                boost::geometry::envelope(*cell_geometry_it, cell_boundaries);
                std::vector<entity::entity> cell_bins;
                m_density.intersecting_bins(cell_boundaries, cell_bins);

                for (auto & bin : cell_bins) {
                    point bin_position = m_density.bin_position(bin);
                    point bin_dimension = m_density.bin_dimension(bin);
                    box bin_boundaries(bin_position, point(bin_position.x() + bin_dimension.x(), bin_position.y() + bin_dimension.y()));
                    multi_polygon intersection;
                    boost::geometry::intersection(bin_boundaries, *cell_geometry_it, intersection);
                    double intersection_area = boost::geometry::area(intersection);
                    m_density.add_movable_utilization(bin, intersection_area);
                }

            }
        }

        double abu::measure_abu_for_range(double range, std::vector<double> &utilizations) {
            size_t number_of_bins = m_density.bins_system().entities().size();

            double local_abu = 0;
            int clip_index = range * number_of_bins;
            for (size_t bin_index = number_of_bins-1; bin_index > number_of_bins-1-clip_index; bin_index--) {
                local_abu += utilizations.at(bin_index);
            }
            local_abu = (clip_index) ? local_abu / clip_index : utilizations.at(number_of_bins - 1);
            return local_abu;
        }
    }
}
