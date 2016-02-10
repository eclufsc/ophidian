//
// Created by renan on 2/9/16.
//

#ifndef OPENEDA_ROWS_H
#define OPENEDA_ROWS_H

#include <vector_property.h>
#include "entity.h"
#include "../geometry/geometry.h"

namespace openeda {
    namespace floorplan {
        class rows {
            using point = geometry::point<double>;

            entity::system & m_system;

            entity::vector_property<entity::entity> m_sites;
            entity::vector_property<unsigned> m_number_of_sites;
            entity::vector_property<point> m_origins;

        public:
            rows(entity::system & system);

            entity::entity site(entity::entity row) const {
                return m_sites[m_system.lookup(row)];
            }

            unsigned number_of_sites(entity::entity row) const {
                return m_number_of_sites[m_system.lookup(row)];
            }

            point origin(entity::entity row) const {
                return m_origins[m_system.lookup(row)];
            }

            std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > sites() const {
                return std::make_pair(m_sites.begin(), m_sites.end());
            }

            std::pair< std::vector<unsigned>::const_iterator, std::vector<unsigned>::const_iterator > number_of_sites() const {
                return std::make_pair(m_number_of_sites.begin(), m_number_of_sites.end());
            }

            std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > origins() const {
                return std::make_pair(m_origins.begin(), m_origins.end());
            }

            void site(entity::entity row, entity::entity site);

            void number_of_sites(entity::entity row, unsigned number_of_sites);

            void origin(entity::entity row, point origin);
        };
    }
}


#endif //OPENEDA_ROWS_H
