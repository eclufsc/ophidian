/*
 * Copyright 2018 Ophidian
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

#ifndef OPHIDIAN_PARSER_VIA_H
#define OPHIDIAN_PARSER_VIA_H

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class Via{
    public:
        template <class T> using container_type     = std::vector<T>;

        template <class T> using point_type         = geometry::Point<T>;
        template <class T> using box_type           = geometry::Box<T>;
        template <class K, class V> using map_type  = std::map<K,V>;

        using string_type                           = std::string;
        using micrometer_type                       = util::micrometer_t;
        using micrometer_point_type                 = point_type<micrometer_type>;
        using micrometer_box_type                   = box_type<micrometer_type>;

        using layer_container_type                  = container_type<micrometer_box_type>;
        using layer_map_type                        = map_type<string_type, layer_container_type>;

        // Class constructors
        Via() = delete;

        Via(const Via&) = default;
        Via& operator=(const Via&) = default;

        Via(Via&&) = default;
        Via& operator=(Via&&) = default;

        Via(const string_type& name):
            m_name{name}
        {}

        Via(string_type&& name):
            m_name(std::move(name))
        {}

        //class members
        const string_type& name() const noexcept;

        void addLayer(string_type layer, layer_container_type boxes) noexcept;

        const layer_map_type& layers() const noexcept;

    private:
        string_type m_name;
        layer_map_type m_layers;
    };
}

#endif
