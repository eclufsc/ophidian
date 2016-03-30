/*
 * simple_design_constraint.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: csguth
 */

#include "simple_design_constraint.h"

namespace openeda {
namespace timing {

simple_design_constraint::simple_design_constraint() {
	m_dc.clock.name = "mclock";
	m_dc.clock.period = 80.0;
	m_dc.clock.port_name = "iccad_clk";

	dc_input_delays inp1_delay;
	inp1_delay.clock = "mclk";
	inp1_delay.delay = 0.0;
	inp1_delay.port_name = "inp1";

	dc_input_delays inp2_delay(inp1_delay);
	inp2_delay.port_name = "inp2";

	m_dc.input_delays.push_back(inp1_delay);
	m_dc.input_delays.push_back(inp2_delay);

	dc_driving_cell inp1_driving_cell;
	inp1_driving_cell.lib_cell = "INV_X80";
	inp1_driving_cell.pin_name = "o";
	inp1_driving_cell.port_name = "inp1";
	inp1_driving_cell.slew_fall = 10.0;
	inp1_driving_cell.slew_rise = 10.0;

	dc_driving_cell inp2_driving_cell(inp1_driving_cell);
	inp2_driving_cell.port_name = "inp2";

	m_dc.input_drivers.push_back(inp1_driving_cell);
	m_dc.input_drivers.push_back(inp2_driving_cell);


	dc_output_delay out_delay;
	out_delay.clock = "mclk";
	out_delay.delay = 0.0;
	out_delay.port_name = "out";

	m_dc.output_delays.push_back(out_delay);

	dc_output_load out_load;
	out_load.pin_load = 4.0;
	out_load.port_name = "out";
	m_dc.output_loads.push_back(out_load);
}

simple_design_constraint::~simple_design_constraint() {
}

} /* namespace timing */
} /* namespace openeda */
