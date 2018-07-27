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

#ifndef OPHIDIAN_PARSER_SDCPARSER_H
#define OPHIDIAN_PARSER_SDCPARSER_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "ParserException.h"

namespace ophidian
{
namespace parser
{

class DesignConstraints
{
public:
    template <class T> using container_type = std::vector<T>;
    using string_type = std::string;

    struct Clock {
        double period;
        string_type name;
        string_type port_name;
    };

    struct InputDelays {
        double delay;
        string_type clock;
        string_type port_name;
    };

    struct DrivingCell {
        string_type port_name;
        string_type lib_cell;
        string_type pin_name;
        double slew_fall;
        double slew_rise;
    };

    struct OutputDelay {
        double delay;
        string_type clock;
        string_type port_name;
    };

    struct OutputLoad {
        double pin_load;
        string_type port_name;
    };

    using clock_type        = Clock;
    using input_delays_type = InputDelays;
    using driving_cell_type = DrivingCell;
    using output_delay_type = OutputDelay;
    using output_load_type  = OutputLoad;

    DesignConstraints() = default;
    ~DesignConstraints() = default;

    clock_type                        m_clock;
    container_type<input_delays_type> m_input_delays;
    container_type<driving_cell_type> m_input_drivers;
    container_type<output_delay_type> m_output_delays;
    container_type<output_load_type>  m_output_loads;
};

class SDCSimple
{
public:
    SDCSimple() = default;
    ~SDCSimple() = default;

    std::shared_ptr<DesignConstraints> constraints() const;
};

class SDCParser
{
public:
    SDCParser() = default;
    ~SDCParser() = default;

    std::shared_ptr<DesignConstraints> readFile(const std::string & filename) const throw(InexistentFile);
};

}   // namespace parser
}   // namespace ophidian

#endif // OPHIDIAN_PARSER_SDCPARSER_H
