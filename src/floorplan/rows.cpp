//
// Created by renan on 2/9/16.
//

#include "rows.h"

namespace openeda {
    namespace floorplan {

        rows::rows(entity::system &system) : m_system(system) {
            m_system.register_property(&m_sites);
            m_system.register_property(&m_number_of_sites);
            m_system.register_property(&m_origins);
        }

        void rows::site(entity::entity row, entity::entity site) {
            m_sites[m_system.lookup(row)] = site;
        }

        void rows::number_of_sites(entity::entity row, unsigned number_of_sites) {
            m_number_of_sites[m_system.lookup(row)] = number_of_sites;
        }

        void rows::origin(entity::entity row, rows::point origin) {
            m_origins[m_system.lookup(row)] = origin;
        }
    }
}