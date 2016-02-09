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

            void name(entity::entity site, std::string name);

            void dimensions(entity::entity site, point dimensions);
        };
    }
}



#endif //OPENEDA_SITES_H
