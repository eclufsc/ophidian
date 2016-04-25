//
// Created by renan on 2/9/16.
//

#include "floorplan.h"

namespace openeda {
namespace floorplan {

    floorplan::floorplan()
            : m_sites(m_sites_system), m_rows(m_rows_system) {

    }

    entity::entity floorplan::site_insert(std::string name, floorplan::point dimensions) {
        auto result = m_name2site.find(name);
        if (result != m_name2site.end())
            return result->second;

        entity::entity site = m_sites_system.create();
        m_name2site[name] = site;
        m_sites.name(site, name);
        m_sites.dimensions(site, dimensions);
        return site;
    }

    void floorplan::site_destroy(entity::entity site) {
        m_sites_system.destroy(site);
    }

    entity::entity floorplan::row_insert(std::string site_name, unsigned number_of_sites, floorplan::point origin) {
        auto site = site_insert(site_name, {0, 0});
        return row_insert(site, number_of_sites, origin);
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

    void floorplan::chip_boundaries(floorplan::point chip_boundaries) {
        m_chip_boundaries = chip_boundaries;
    }

    void floorplan::chip_origin(floorplan::point chip_origin) {
        m_chip_origin = chip_origin;
    }
}
}
