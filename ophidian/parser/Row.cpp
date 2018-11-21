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

#include "Row.h"

namespace ophidian::parser
{
    const Row::name_type& Row::name() const noexcept
    {
        return m_name;
    }

    const Row::site_type& Row::site() const noexcept
    {
        return m_site;
    }

    const Row::database_unit_point_type& Row::origin() const noexcept
    {
        return m_origin;
    }

    const Row::database_unit_point_type& Row::step() const noexcept
    {
        return m_step;
    }

    const Row::scalar_point_type& Row::num() const noexcept
    {
        return m_num;
    }

    bool Row::operator==(const Row& rhs) const noexcept
    {
        return m_name == rhs.m_name &&
            m_site == rhs.m_site &&
            m_origin.x() == rhs.m_origin.x() &&
            m_origin.y() == rhs.m_origin.y() &&
            m_step.x() == rhs.m_step.x() &&
            m_step.y() == rhs.m_step.y() &&
            m_num.x() == rhs.m_num.x() &&
            m_num.y() == rhs.m_num.y();
    }

    std::ostream& operator<<(std::ostream& os,const Row& row)
    {
        os << "{name: " << row.m_name
            << ", site: " << row.m_site
            << ", origin: )" << row.m_origin.x()
            << ", " << row.m_origin.y() << ")"
            << ", step: (" << row.m_step.x()
            << ", " << row.m_step.y() << ")"
            << ", num: (" << row.m_num.x()
            << ", " << row.m_num.y() << ")"
            << "}";

        return os;
    }

}
