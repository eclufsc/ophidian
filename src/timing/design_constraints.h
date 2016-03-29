/*
 * design_constraints.h
 *
 *  Created on: Mar 28, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_DESIGN_CONSTRAINTS_H_
#define SRC_TIMING_DESIGN_CONSTRAINTS_H_

#include <vector>
#include <string>

namespace openeda {
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
    default_design_constraints();
    const design_constraints dc() const {
        return m_dc;
    }
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_DESIGN_CONSTRAINTS_H_ */
