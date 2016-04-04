/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
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
    default_design_constraints(const netlist::netlist & netlist);
    const design_constraints dc() const {
        return m_dc;
    }
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_DESIGN_CONSTRAINTS_H_ */
