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

#include "design_constraints.h"

namespace ophidian {
namespace timing {

default_design_constraints::default_design_constraints(const netlist::netlist &netlist)
{
    m_dc.clock.name = "mclk";
    m_dc.clock.port_name = "iccad_clk";
    m_dc.clock.period = 80.0;

    for(auto PI(netlist.PI_begin()); PI != netlist.PI_end(); ++PI)
    {
        if(netlist.pin_name(*PI) == m_dc.clock.port_name) continue;
        dc_input_delays delay;
        delay.clock = "mclk";
        delay.delay = 0.0;
        delay.port_name = netlist.pin_name(*PI);

        dc_driving_cell driver;
        driver.lib_cell = "INV_X80";
        driver.pin_name = "o";
        driver.port_name = netlist.pin_name(*PI);
        driver.slew_fall = 10.0;
        driver.slew_rise = 10.0;

        m_dc.input_delays.push_back(delay);
        m_dc.input_drivers.push_back(driver);
    }



    for(auto PO(netlist.PO_begin()); PO != netlist.PO_end(); ++PO)
    {
        dc_output_delay delay;
        delay.clock = "mclk";
        delay.delay = 0.0;
        delay.port_name = netlist.pin_name(*PO);

        dc_output_load load;
        load.pin_load = 4.0;
        load.port_name = netlist.pin_name(*PO);

        m_dc.output_delays.push_back(delay);
        m_dc.output_loads.push_back(load);
    }

}

} /* namespace timing */
} /* namespace ophidian */

