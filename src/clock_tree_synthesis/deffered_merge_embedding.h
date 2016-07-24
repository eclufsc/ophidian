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

#ifndef DEFFERED_MERGE_EMBEDDING_H
#define DEFFERED_MERGE_EMBEDDING_H

#include "clock_topology.h"
#include "../geometry/geometry.h"
#include "lemon/connectivity.h"
#include "boost/geometry.hpp"

namespace ophidian {
namespace clock_tree_synthesis {
class deffered_merge_embedding
{
public:
    using point = geometry::point<double>;
    using segment = geometry::segment<point>;
    using box = geometry::box<point>;

    struct trr {
    private:
        segment m_core;
        double m_radius;
        box m_box;
    public:
        trr() {

        }

        trr(segment core, double radius) : m_core(core), m_radius(radius) {
            std::vector<point> trr_points;
            if (m_core.first.x() <= m_core.second.x()) {
                if (m_core.first.y() <= m_core.second.y()) {
                    trr_points.push_back({m_core.first.x() - m_radius, m_core.first.y()});
                    trr_points.push_back({m_core.first.x(), m_core.first.y() - m_radius});
                    trr_points.push_back({m_core.second.x() + m_radius, m_core.second.y()});
                    trr_points.push_back({m_core.second.x(), m_core.second.y() + m_radius});
                } else {
                    trr_points.push_back({m_core.first.x() - m_radius, m_core.first.y()});
                    trr_points.push_back({m_core.first.x(), m_core.first.y() + m_radius});
                    trr_points.push_back({m_core.second.x() + m_radius, m_core.second.y()});
                    trr_points.push_back({m_core.second.x(), m_core.second.y() - m_radius});
                }
            } else {
                if (m_core.second.y() <= m_core.first.y()) {
                    trr_points.push_back({m_core.second.x() - m_radius, m_core.second.y()});
                    trr_points.push_back({m_core.second.x(), m_core.second.y() - m_radius});
                    trr_points.push_back({m_core.first.x() + m_radius, m_core.first.y()});
                    trr_points.push_back({m_core.first.x(), m_core.first.y() + m_radius});
                } else {
                    trr_points.push_back({m_core.second.x() - m_radius, m_core.second.y()});
                    trr_points.push_back({m_core.second.x(), m_core.second.y() + m_radius});
                    trr_points.push_back({m_core.first.x() + m_radius, m_core.first.y()});
                    trr_points.push_back({m_core.first.x(), m_core.first.y() - m_radius});
                }
            }

            double min_x = std::numeric_limits<double>::max();
            double min_y = std::numeric_limits<double>::max();
            double max_x = std::numeric_limits<double>::min();
            double max_y = std::numeric_limits<double>::min();
            for (point & trr_point : trr_points) {
                point rotated_point;
                //rotate(trr_point, 45.0, rotated_point, true);
                geometry::rotate(trr_point, 45.0, rotated_point);
                min_x = std::min(rotated_point.x(), min_x);
                min_y = std::min(rotated_point.y(), min_y);
                max_x = std::max(rotated_point.x(), max_x);
                max_y = std::max(rotated_point.y(), max_y);
            }
            m_box = box({min_x, min_y}, {max_x, max_y});
        }

//        trr(segment core, double radius) : m_core(core), m_radius(radius) {
//            point base_point = (m_core.first.y() <= m_core.second.y()) ? m_core.first : m_core.second;
//            point top_point = (m_core.first.y() > m_core.second.y()) ? m_core.first : m_core.second;
//            point min_corner(base_point.x(), base_point.y() - m_radius);
//            point max_corner(top_point.x(), top_point.y() + m_radius);
//            point rotated_min_corner;
//            rotate(min_corner, 45, rotated_min_corner, true);
//            //geometry::rotate(min_corner, 45, rotated_min_corner);
//            point rotated_max_corner;
//            //geometry::rotate(max_corner, 45, rotated_max_corner);
//            rotate(max_corner, 45, rotated_max_corner, true);
//            m_box = box(rotated_min_corner, rotated_max_corner);

////            segment rotated_segment;
////            geometry::rotate(core, 45.0, rotated_segment);
////            point min_corner(std::min(rotated_segment.first.x() - radius, rotated_segment.second.x() - radius), std::min(rotated_segment.first.y() - radius, rotated_segment.second.y() - radius));
////            point max_corner(std::max(rotated_segment.first.x() + radius, rotated_segment.second.x() + radius), std::max(rotated_segment.first.y() + radius, rotated_segment.second.y() + radius));
////            m_box = box(min_corner, max_corner);
//        }

        void rotate(point original_point, double angle, point & rotated_point, bool clockwise) {
            if (clockwise) {
                rotated_point.x((original_point.x() + original_point.y()) / std::sqrt(2));
                rotated_point.y((original_point.x() - original_point.y()) / std::sqrt(2));
            } else {
                rotated_point.x((original_point.x() - original_point.y()) / std::sqrt(2));
                rotated_point.y((original_point.x() + original_point.y()) / std::sqrt(2));
            }
        }

        segment intersection(const trr & other_trr) {
            box intersection_box;
            boost::geometry::intersection(m_box, other_trr.m_box, intersection_box);
            segment merge_segment(intersection_box.min_corner(), intersection_box.max_corner());
//            point rotated_point1;
//            point rotated_point2;
//            rotate(merge_segment.first, 45.0, rotated_point1, false);
//            rotate(merge_segment.second, 45.0, rotated_point2, false);
//            segment rotated_merge_segment(rotated_point1, rotated_point2);
            segment rotated_merge_segment;
            geometry::rotate(merge_segment, 45.0, rotated_merge_segment);
            return rotated_merge_segment;
        }

        segment intersection(const segment & merging_segment) {
            trr other_trr(merging_segment, 0);
            return intersection(other_trr);
        }
    };

private:
    double m_resistance_per_unit;
    double m_capacitance_per_unit;

    void build_tree_of_segments(std::vector<clock_topology::node> & sorted_nodes, clock_topology::graph_t::NodeMap<segment> & merging_segments, clock_topology::graph_t::NodeMap<trr> & trrs, clock_topology & topology);
    void find_exact_locations(std::vector<clock_topology::node> & sorted_nodes, const clock_topology::graph_t::NodeMap<segment> &merging_segments, clock_topology & topology);
    void update_node_delay_and_capacitance(clock_topology::node node, clock_topology::graph_t::NodeMap<segment> &merging_segments, clock_topology &topology);

    std::pair<double, double> calculate_edge_length(clock_topology::node node_a, clock_topology::node node_b, clock_topology &topology);
    double distance(const segment & merging_segment, const point & target_point);
    double distance(const segment & merging_segment1, const segment & merging_segment2);
public:
    deffered_merge_embedding(double resistance_per_unit = 1.0, double capacitance_per_unit = 1.0);
    ~deffered_merge_embedding();

    void run_deffered_merge_embedding(clock_topology & topology);
};
}
}

#endif // DEFFERED_MERGE_EMBEDDING_H
