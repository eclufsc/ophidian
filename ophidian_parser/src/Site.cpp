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

#include <ophidian/parser/Site.h>

namespace ophidian::parser
{
    const Site::string_type& Site::name() const noexcept
    {
        return m_name;
    }

    const Site::string_type& Site::class_name() const noexcept
    {
        return m_class_name;
    }

    const Site::micrometer_type& Site::width() const noexcept
    {
        return m_width;
    }

    const Site::micrometer_type& Site::height() const noexcept
    {
        return m_height;
    }

    const Site::symmetry_type& Site::symmetry() const noexcept
    {
        return m_symmetry;
    }

    std::ostream& operator<<(std::ostream& os, const Site& site)
    {
        auto symmetry_string = [&](){
            using namespace std::literals;

            auto st = ""s;
            if(site.m_symmetry.is_x_symmetric){
                st += "X";
            }
            if(site.m_symmetry.is_y_symmetric){
                st += "Y";
            }
            if(site.m_symmetry.is_90_symmetric){
                st += "90";
            }

            return st;
        };

        os << "{name: " << site.m_name
            << ", class: " << site.m_class_name
            << ", width: " << site.m_width
            << ", height: " << site.m_height
            << ", symmetry:" << symmetry_string()
            << "}";

        return os;
    }

    bool Site::operator==(const Site& rhs) const noexcept
    {
        return m_name == rhs.m_name &&
            m_class_name == rhs.m_class_name &&
            m_width == rhs.m_width &&
            m_height == rhs.m_height &&
            m_symmetry == rhs.m_symmetry;
    }

    bool Site::Symmetry::operator==(const Site::Symmetry& rhs) const noexcept
    {
        return is_x_symmetric == rhs.is_x_symmetric &&
            is_y_symmetric == rhs.is_y_symmetric &&
            is_90_symmetric == rhs.is_90_symmetric;
    }
}
