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

#include "subrows.h"

namespace ophidian {
    namespace placement {
        namespace legalization {

            void subrows::begin(entity::entity subrow, double begin) {
                m_begin[m_system.lookup(subrow)] = begin;
            }

            void subrows::end(entity::entity subrow, double end) {
                m_end[m_system.lookup(subrow)] = end;
            }

            void subrows::row(entity::entity subrow, entity::entity row) {
                m_row[m_system.lookup(subrow)] = row;
            }

            void subrows::create_subrows(floorplan::floorplan *floorplan, placement * placement) {
                for (auto row : floorplan->rows_system()) {
                    auto subrow = m_system.create();
                    this->begin(subrow, floorplan->row_origin(row.first).x());
                    this->end(subrow, floorplan->row_origin(row.first).x() + floorplan->row_dimensions(row.first).x() - 1);
                    this->row(subrow, row.first);
                    point subrow_min_corner = floorplan->row_origin(row.first);
                    point subrow_max_corner(floorplan->row_origin(row.first).x() + floorplan->row_dimensions(row.first).x() - 1, floorplan->row_origin(row.first).y() + floorplan->row_dimensions(row.first).y() - 1);
                    box subrow_box(subrow_min_corner, subrow_max_corner);
                    subrows_rtree.insert(std::make_pair(subrow_box, subrow));
                }

                for (auto cell : placement->netlist().cell_system()) {
                    if (placement->cell_fixed(cell.first)) {
                        auto cell_geometry = placement->cell_geometry(cell.first);
                        for (auto cell_polygon : cell_geometry) {
                            box cell_rectangle;
                            boost::geometry::envelope(cell_polygon, cell_rectangle);
                            std::vector<rtree_node> intersecting_nodes;
                            subrows_rtree.query(boost::geometry::index::intersects(cell_rectangle), std::back_inserter(intersecting_nodes));
                            for (auto node : intersecting_nodes) {
                                auto subrow = node.second;
                                auto row = this->row(subrow);

                                auto left_subrow = m_system.create();
                                this->begin(left_subrow, this->begin(subrow));
                                this->end(left_subrow, cell_rectangle.min_corner().x() - 1);
                                this->row(left_subrow, row);
                                point left_subrow_min_corner(this->begin(left_subrow), floorplan->row_origin(row).y());
                                point left_subrow_max_corner(cell_rectangle.min_corner().x() - 1, floorplan->row_origin(row).y() + floorplan->row_dimensions(row).y() - 1);
                                box left_subrow_box(left_subrow_min_corner, left_subrow_max_corner);
                                subrows_rtree.insert(std::make_pair(left_subrow_box, left_subrow));

                                auto right_subrow = m_system.create();
                                this->begin(right_subrow, cell_rectangle.max_corner().x());
                                this->end(right_subrow, this->end(subrow));
                                this->row(right_subrow, this->row(subrow));
                                point right_subrow_min_corner(cell_rectangle.max_corner().x(), floorplan->row_origin(row).y());
                                point right_subrow_max_corner(this->end(subrow), floorplan->row_origin(row).y() + floorplan->row_dimensions(row).y() - 1);
                                box right_subrow_box(right_subrow_min_corner, right_subrow_max_corner);
                                subrows_rtree.insert(std::make_pair(right_subrow_box, right_subrow));

                                m_system.destroy(subrow);
                                subrows_rtree.remove(node);
                            }
                        }
                    }
                }
            }

            entity::entity subrows::find_subrow(point coordinate) {
                std::vector<rtree_node> intersecting_nodes;
                subrows_rtree.query(boost::geometry::index::intersects(coordinate), std::back_inserter(intersecting_nodes));
                if (intersecting_nodes.empty()) {
                    throw subrow_not_found();
                }
                assert(intersecting_nodes.size() == 1);
                return intersecting_nodes.front().second;
            }
        }
    }
}
