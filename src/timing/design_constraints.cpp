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

