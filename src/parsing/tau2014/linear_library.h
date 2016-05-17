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

#ifndef OPHIDIAN_PARSING_TAU2014_LINEAR_LIBRARY_H
#define OPHIDIAN_PARSING_TAU2014_LINEAR_LIBRARY_H

#include <string>
#include <unordered_map>
#include <vector>
#include <limits>

namespace ophidian {
namespace parsing {
namespace tau2014 {

class linear_library
{
public:
    struct pin;
    struct test {
        int from;
        int to;
        std::string setup_or_hold;
        std::string type;
        double fall[3];
        double rise[3];
    };

    struct arc {
        int from;
        int to;
        double rise_delay[3];
        double fall_delay[3];
        double rise_slew[3];
        double fall_slew[3];
        std::string unateness;
    };

    struct pin {
        std::string name;
        std::string direction;
        double rise_capacitance{std::numeric_limits<double>::max()};
        double fall_capacitance{std::numeric_limits<double>::max()};
        bool clock{false};
    };

    struct cell {
        std::string name;
        std::vector<pin> input_pins;
        std::vector<pin> output_pins;
        std::unordered_map< std::string, std::unordered_map< std::string, arc > > arcs;// (input pin => (output pin => arc) )
        std::size_t clock_pin_id{std::numeric_limits<std::size_t>::max()};
        std::size_t data_pin_id{std::numeric_limits<std::size_t>::max()};
        std::vector<test> setup;
        std::vector<test> hold;
    };

private:
    using CellMap = std::unordered_map< std::string, cell >;
    CellMap m_cells;
public:
    linear_library(const std::string & filename);
    virtual ~linear_library();

    const CellMap & cells() const {
        return m_cells;
    }
};

}
}
}


#endif // OPHIDIAN_PARSING_TAU2014_LINEAR_LIBRARY_H
