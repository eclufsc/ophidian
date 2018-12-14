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

#ifndef OPHIDIAN_PARSER_GROUP_H
#define OPHIDIAN_PARSER_GROUP_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * @brief Type to represent a circuit component.
     *
     * This is the data necessary to identify a given
     * component and it's characteristics.
     */
    class Group
    {
    public:
        template <class T> using container_type = std::vector<T>;

        using name_type                = std::string;

        using members_container_type  = container_type<name_type>;

        // Class constructors

        Group() = default;

        Group(const Group&) = default;
        Group& operator=(Group&) = default;

        Group(Group&&) = default;
        Group& operator=(Group&&) = default;

        template<class Arg1, class Arg2>
        Group(Arg1&& name, Arg2&& members):
            m_name{std::forward<Arg1>(name)},
            m_members{std::forward<Arg2>(members)}
        {}

        Group(const name_type & name);

        Group(name_type&& name);

        void add_member(const name_type & member) noexcept;

        // Class member functions
        const name_type& name() const noexcept;

        const members_container_type & members() const noexcept;

        bool operator==(const Group& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Group& group);

    private:
        name_type                m_name{}; 
        members_container_type   m_members{};
    };
}

#endif
