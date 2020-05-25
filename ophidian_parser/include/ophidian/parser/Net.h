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

#ifndef OPHIDIAN_PARSER_NET_H
#define OPHIDIAN_PARSER_NET_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class Net
    {
    public:
        // Class member types
        using name_type = std::string;

        using pin_name_type = std::string;

        using component_name_type = std::string;

        using pin_type = std::pair<component_name_type, pin_name_type>;

        using pin_container_type = std::vector<pin_type>;

        // Class constructors
        Net() = delete;

        Net(const Net &) = default;
        Net & operator=(const Net &) = default;

        Net(Net &&) = default;
        Net & operator=(Net &&) = default;

        template<class Arg1, class Arg2>
        Net(Arg1&& name, Arg2&& pins):
            m_name{std::forward<Arg1>(name)},
            m_pins{std::forward<Arg2>(pins)}
        { }

        template<class Arg1, class Arg2, class Arg3>
        Net(Arg1&& name, Arg2&& pins, Arg3&& min_layer):
            m_name{std::forward<Arg1>(name)},
            m_pins{std::forward<Arg2>(pins)},
            m_min_layer{std::forward<Arg3>(min_layer)}
        { }

        const name_type& name() const noexcept;

        const pin_container_type& pins() const noexcept;

        const name_type & min_layer() const noexcept;

        bool operator==(const Net& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Net& net);

    private:
        name_type m_name;
        pin_container_type m_pins;
        name_type m_min_layer;
    };
}

#endif
