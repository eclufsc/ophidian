//
// Created by renan on 2/9/16.
//

#include "sites.h"

namespace openeda {
    namespace floorplan {

        sites::sites(entity::system &system) : m_system(system) {
            m_system.register_property(&m_names);
            m_system.register_property(&m_dimensions);
        }

        void sites::name(entity::entity site, std::string name) {
            m_names[m_system.lookup(site)] = name;
        }

        void sites::dimensions(entity::entity site, sites::point dimensions) {
            m_dimensions[m_system.lookup(site)] = dimensions;
        }
    }
}