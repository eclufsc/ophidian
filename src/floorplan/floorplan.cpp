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

#include "floorplan.h"

namespace ophidian {
namespace floorplan {

    floorplan::floorplan()
            : m_sites(m_sites_system), m_rows(m_rows_system) {

    }

    entity_system::entity floorplan::site_insert(std::string name, floorplan::point dimensions) {
        auto result = m_name2site.find(name);
        if (result != m_name2site.end())
            return result->second;

        entity_system::entity site = m_sites_system.create();
        m_name2site[name] = site;
        m_sites.name(site, name);
        m_sites.dimensions(site, dimensions);
        return site;
    }

    void floorplan::site_destroy(entity_system::entity site) {
        m_sites_system.destroy(site);
    }

    entity_system::entity floorplan::row_insert(std::string site_name, unsigned number_of_sites, floorplan::point origin) {
        auto site = site_insert(site_name, {0, 0});
        return row_insert(site, number_of_sites, origin);
    }

    entity_system::entity floorplan::row_insert(entity_system::entity site, unsigned number_of_sites, floorplan::point origin) {
        entity_system::entity row = m_rows_system.create();
        m_rows.site(row, site);
        m_rows.number_of_sites(row, number_of_sites);
        m_rows.origin(row, origin);
        return row;
    }

    void floorplan::row_destroy(entity_system::entity row) {
        m_rows_system.destroy(row);
    }

    floorplan::point floorplan::row_dimensions(entity_system::entity row) const {
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
