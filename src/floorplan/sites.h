//
// Created by renan on 2/9/16.
//

#ifndef OPENEDA_SITES_H
#define OPENEDA_SITES_H

#include <vector_property.h>
#include "entity.h"
#include "../geometry/geometry.h"

namespace openeda {
    namespace floorplan {
        class sites {
            using point = geometry::point<double>;

            entity::system & m_system;

            entity::vector_property<std::string> m_names;
            entity::vector_property<point> m_dimensions;

        public:
            sites(entity::system & system);

            std::string name(entity::entity site) const {
                return m_names[m_system.lookup(site)];
            }

            point dimensions(entity::entity site) const {
                return m_dimensions[m_system.lookup(site)];
            }

            std::pair< std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator > names() const {
                return std::make_pair(m_names.begin(), m_names.end());
            }

            std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > dimensions() const {
                return std::make_pair(m_dimensions.begin(), m_dimensions.end());
            }

            void name(entity::entity site, std::string name);

            void dimensions(entity::entity site, point dimensions);
        };
    }
}



#endif //OPENEDA_SITES_H
