//
// Created by renan on 2/9/16.
//

#include "floorplan.h"

namespace openeda {
namespace floorplan {

    floorplan::floorplan(point chip_boundaries)
            : m_sites(m_sites_system), m_rows(m_rows_system), m_chip_boundaries(chip_boundaries) {

    }

    entity::entity floorplan::site_insert(std::string name, floorplan::point dimensions) {
        entity::entity site = m_sites_system.create();
        m_sites.name(site, name);
        m_sites.dimensions(site, dimensions);
        return site;
    }

    void floorplan::site_destroy(entity::entity site) {
        m_sites_system.destroy(site);
    }

    entity::entity floorplan::row_insert(entity::entity site, unsigned number_of_sites, floorplan::point origin) {
        entity::entity row = m_rows_system.create();
        m_rows.site(row, site);
        m_rows.number_of_sites(row, number_of_sites);
        m_rows.origin(row, origin);
        return row;
    }

    void floorplan::row_destroy(entity::entity row) {
        m_rows_system.destroy(row);
    }

    floorplan::point floorplan::row_dimensions(entity::entity row) const {
        auto row_site = m_rows.site(row);
        unsigned number_of_sites = m_rows.number_of_sites(row);
        point site_dimensions = m_sites.dimensions(row_site);
        return {site_dimensions.x() * number_of_sites, site_dimensions.y()};
    }
}
}
