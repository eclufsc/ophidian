/*
 * Copyright 2017 Ophidian
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

#include <ophidian/floorplan/Floorplan.h>
#include "iostream"

namespace ophidian::floorplan
{
    // Element access
    Floorplan::point_type& Floorplan::chip_origin() noexcept
    {
        return m_chip_origin;
    }

    const Floorplan::point_type& Floorplan::chip_origin() const noexcept
    {
        return m_chip_origin;
    }

    Floorplan::point_type& Floorplan::chip_upper_right_corner() noexcept
    {
        return m_chip_upper_right_corner;
    }

    const Floorplan::point_type& Floorplan::chip_upper_right_corner() const noexcept
    {
        return m_chip_upper_right_corner;
    }

    Floorplan::point_type& Floorplan::origin(const Floorplan::row_type & row)
    {
        return m_row_origins[row];
    }

    const Floorplan::point_type& Floorplan::origin(const Floorplan::row_type & row) const
    {
        return m_row_origins[row];
    }

    Floorplan::point_type Floorplan::upper_right_corner(const Floorplan::row_type & row) const
    {
        auto site = m_row_site_types[row];

        util::database_unit_scalar_t numSites = m_row_number_of_sites[row];

        Floorplan::point_type uRCorner = m_site_dimensions[site];

        return Floorplan::point_type{uRCorner.x() * numSites + origin(row).x(), uRCorner.y() + origin(row).y()};
    }

    Floorplan::site_name_type& Floorplan::name(const Floorplan::site_type & site)
    {
        return m_site_names[site];
    }

    const Floorplan::site_name_type& Floorplan::name(const Floorplan::site_type & site) const
    {
        return m_site_names[site];
    }

    Floorplan::point_type& Floorplan::dimension(const Floorplan::site_type & site)
    {
        return m_site_dimensions[site];
    }

    const Floorplan::point_type& Floorplan::dimension(const Floorplan::site_type & site) const
    {
        return m_site_dimensions[site];
    }

    Floorplan::site_type Floorplan::find(const Floorplan::site_name_type& siteName) const
    {
        return m_name_to_site.at(siteName);
    }

    Floorplan::row_size_type& Floorplan::number_of_sites(const Floorplan::row_type & row)
    {
        return m_row_number_of_sites[row];
    }

    const Floorplan::row_size_type& Floorplan::number_of_sites(const Floorplan::row_type & row) const
    {
        return m_row_number_of_sites[row];
    }

    Floorplan::site_type Floorplan::site(const Floorplan::row_type & row) const
    {
        return m_row_site_types[row];
    }

    // Iterators
    ophidian::util::Range<Floorplan::SitesIterator> Floorplan::range_site() const
    {
        return ophidian::util::Range<Floorplan::SitesIterator>(m_sites.begin(), m_sites.end());
    }

    ophidian::util::Range<Floorplan::RowsIterator> Floorplan::range_row() const
    {
        return util::Range<Floorplan::RowsIterator>(m_rows.begin(), m_rows.end());
    }

    // Modifiers
    Floorplan::site_type Floorplan::add_site(const Floorplan::site_name_type & name, const Floorplan::point_type & loc)
    {
        auto site = m_sites.add();

        m_site_names[site] = name;
        m_name_to_site[name] = site;
        m_site_dimensions[site] = loc;

        return site;
    }

    Floorplan::row_type Floorplan::add_row(
        const Floorplan::point_type & loc,
        const Floorplan::row_size_type& num,
        const Floorplan::site_type & site)
    {
        auto row = m_rows.add();

        m_row_origins[row] = loc;
        m_row_number_of_sites[row] = num;
        m_row_site_types[row] = site;

        return row;
    }

    void Floorplan::erase(const Floorplan::site_type& site)
    {
        m_name_to_site.erase(name(site));
        m_sites.erase(site);
    }

    void Floorplan::erase(const Floorplan::row_type & row)
    {
        m_rows.erase(row);
    }
}
