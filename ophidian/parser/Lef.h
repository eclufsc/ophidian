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

#ifndef OPHIDIAN_PARSER_LEF_H
#define OPHIDIAN_PARSER_LEF_H

// std headers
#include <string>
#include <vector>
#include <map>

// ophidian headers
#include "Site.h"
#include "Layer.h"
#include "Macro.h"
#include "Via.h"
#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class Lef
    {
    public:
        // Class member typer
        template <class T> using container_type = std::vector<T>;

        template <class T> using point_type     = geometry::Point<T>;
        template <class T> using box_type       = geometry::Box<T>;

        using site_type                         = Site;
        using site_container_type               = container_type<site_type>;

        using layer_type                        = Layer;
        using layer_container_type              = container_type<layer_type>;

        using macro_type                        = Macro;
        using macro_container_type              = container_type<macro_type>;

        using via_type                          = Via;
        using via_container_type                = container_type<via_type>;

        using micrometer_type                   = util::micrometer_t;
        using micrometer_point_type             = point_type<micrometer_type>;
        using micrometer_box_type               = box_type<micrometer_type>;

        using scalar_type                       = util::database_unit_scalar_t;
        using scalar_point_type                 = point_type<scalar_type>;
        using scalar_box_type                   = box_type<scalar_type>;

        // Class constructors
        Lef() = default;

        Lef(const Lef&) = default;
        Lef& operator=(const Lef&) = default;

        Lef(Lef&&) = default;
        Lef& operator=(Lef&&) = default;

        Lef(const std::string& lef_file);
        Lef(const std::vector<std::string>& lef_files);

        //Class member functions
        void read_file(const std::string& lef_file);

        const site_container_type& sites() const noexcept;

        const layer_container_type& layers() const noexcept;

        const macro_container_type& macros() const noexcept;

        const via_container_type& vias() const noexcept;

        const scalar_type& micrometer_to_dbu_ratio() const noexcept;

    private:
        site_container_type  m_sites{};
        layer_container_type m_layers{};
        macro_container_type m_macros{};
        via_container_type   m_vias{};
        scalar_type          m_micrometer_to_dbu_ratio{0.0d};
    };
}

#endif /* OPHIDIAN_PARSER_LEF_H */
