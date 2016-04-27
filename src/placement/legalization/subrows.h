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

#ifndef OPHIDIAN_SUBROWS_H
#define OPHIDIAN_SUBROWS_H

#include <boost/geometry/index/rtree.hpp>
#include "floorplan.h"
#include <placement.h>

namespace ophidian {
    namespace placement {
        namespace legalization {
            using point = geometry::point<double>;
            using box = geometry::box<point>;
            using rtree_node = std::pair<box, entity::entity>;
            using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;
            class subrows {

                entity::system m_system;

                entity::vector_property<double> m_begin;
                entity::vector_property<double> m_end;
                entity::vector_property<entity::entity> m_row;

                rtree subrows_rtree;
            public:

                subrows() {
                    m_system.register_property(&m_begin);
                    m_system.register_property(&m_end);
                    m_system.register_property(&m_row);
                }

                std::size_t count() {
                    return m_system.size();
                }

                void create_subrows(floorplan::floorplan * floorplan, placement * placement);

                double begin(entity::entity subrow) {
                    return m_begin[m_system.lookup(subrow)];
                }
                void begin(entity::entity subrow, double begin);

                double end(entity::entity subrow) {
                    return m_end[m_system.lookup(subrow)];
                }
                void end(entity::entity subrow, double end);

                entity::entity row(entity::entity subrow) {
                    return m_row[m_system.lookup(subrow)];
                }
                void row(entity::entity subrow, entity::entity row);

                entity::entity find_subrow(point coordinate);

            };

            class subrow_not_found : public std::exception {
            public:
                const char * what() const throw() {
                    return "No subrow found in the given coordinate";
                }
            };
        }
    }
}


#endif //OPHIDIAN_SUBROWS_H
