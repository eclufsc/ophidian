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

#include "Group.h"

namespace ophidian::parser
{
    void Group::add_member(std::string member) noexcept
    {
        m_members.push_back(member);
    }

    const Group::name_type& Group::name() const noexcept
    {
        return m_name;
    }

    const Group::members_container_type& Group::members() const noexcept
    {
        return m_members;
    }

    bool Group::operator==(const Group& rhs) const noexcept
    {
        return m_name == rhs.m_name 
                && this->m_members.size() == rhs.m_members.size() 
                && std::is_permutation(this->m_members.begin(), this->m_members.end(), rhs.m_members.begin());
    }

    std::ostream& operator<<(std::ostream& os, const Group& group)
    {
        os << "{name: " << group.m_name;

        os << ", members: {";
        for (auto member : group.members()) {
            os << member << " ";
        }
        os << "}";
        
        os << "}";

        return os;
    }
}
