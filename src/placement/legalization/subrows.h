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

                entity::system & m_system;

                entity::vector_property<double> m_begin;
                entity::vector_property<double> m_end;
                entity::vector_property<entity::entity> m_row;

                rtree subrows_rtree;
            public:

                subrows(entity::system & system)
                    : m_system(system){
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
