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

#ifndef SRC_TIMING_DESIGN_CONSTRAINTS_H_
#define SRC_TIMING_DESIGN_CONSTRAINTS_H_

#include <vector>
#include <string>

#include "../netlist/netlist.h"

namespace ophidian {
namespace timing {

struct dc_driving_cell {
    std::string port_name;
    std::string lib_cell;
    std::string pin_name;
    double slew_fall;
    double slew_rise;
};

struct dc_input_delays {
    double delay;
    std::string clock;
    std::string port_name;
};

struct dc_clock {
    std::string name;
    double period;
    std::string port_name;
};

struct dc_output_delay {
    double delay;
    std::string clock;
    std::string port_name;
};

struct dc_output_load {
    double pin_load;
    std::string port_name;
};

struct design_constraints {
    dc_clock clock;
    std::vector<dc_input_delays> input_delays;
    std::vector<dc_driving_cell> input_drivers;
    std::vector<dc_output_delay> output_delays;
    std::vector<dc_output_load> output_loads;
};

class default_design_constraints {
    design_constraints m_dc;
public:
    default_design_constraints(){}
    default_design_constraints(const netlist::netlist & netlist);
    const design_constraints dc() const {
        return m_dc;
    }
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_DESIGN_CONSTRAINTS_H_ */
