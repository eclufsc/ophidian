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

#ifndef OPHIDIAN_TIMING_OPENTIMER_H
#define OPHIDIAN_TIMING_OPENTIMER_H

#include <ophidian/design/Design.h>
#include <ot/timer/timer.hpp>

namespace ophidian::timing
{
    class OpenTimer{
    public:
        using transition_type = ot::Tran;
        using split_type_type = ot::Split;


//Constructors
        OpenTimer() = delete;

        OpenTimer(const OpenTimer&) = default;
        OpenTimer& operator=(const OpenTimer&) = default;

        OpenTimer(OpenTimer&&) = default;
        OpenTimer& operator=(OpenTimer&&) = default;

        OpenTimer(ophidian::design::Design & design, std::string early_lib_path, std::string late_lib_path, std::string sdc_path, std::string spef_path);

//Timing info queries
        /*
        Function: at
        Report the arrival time of a pin by a given transition type and split type.
        */
        float at(const std::string& pin_name, split_type_type split_type, transition_type transition_type);

        /*
        Function: rat
        Report the required arrival time of a pin by a given transition type and split type.
        */
        float rat(const std::string& pin_name, split_type_type split_type, transition_type transition_type);

        /*
        Function: slew
        Report the slew of a pin by a given transition type and split type.
        */
        float slew(const std::string& pin_name, split_type_type split_type, transition_type transition_type);

        /*
        Function: slack
        Report the slack of a pin by a given transition type and split type.
        */
        float slack(const std::string& pin_name, split_type_type split_type, transition_type transition_type);

        /*
        Function: load
        Report the load of a pin by a given transition type and split type.
        */
        float load(const std::string& pin_name, split_type_type split_type, transition_type transition_type);

        /*
        Function: leakage_power
        Return the leakage_power of design.
        */
        float leakage_power();

        /*
        Function: tns
        Update the total negative slack for any transition and timing split.
        The procedure applies the parallel reduction to compute the value.
        */
        float tns();

        /*
        Function: wns
        Update the total negative slack for any transition and timing split.
        The procedure apply the parallel reduction to compute the value.
        */
        float wns();

//Design modifiers
        /*
        Function: update_timing
        Perform comprehensive timing update:
        (1) grpah-based timing (GBA)
        (2) path-based timing (PBA)
        */
        void update_timing();

        /*
        Function: cppr
        Perform timing analisys considering CPPR.
        ref: https://sites.google.com/view/tau-contest-2019
        */
        void cppr(bool mode);

        /*
        Function: insert_net
        Given a net name insert it in Open Timer and also in Design netlist.
        */
        void insert_net(std::string net_name);

        /*
        Function: insert_cell_instance
        Given a standard cell name and cell instance name, insert these in OpenTimer and also in netlist.
        */
        void insert_cell_instance(std::string cell_instance, std::string standard_cell);

        /*
        Function: repower_cell_instance
        Given cell instance name repower this to a standard cell name.
        */
        void repower_cell_instance(std::string cell_instance, std::string standard_cell);

        /*
        Function: remove_net
        Given a net name remove it from netlist and Open Timer.
        */
        void remove_net(std::string net_name);

        /*
        Function: remove_cell_instance
        Given a name remove this cell instance from netlist and Open Timer.
        */
        void remove_cell_instance(std::string cell_instance_name);

        /*
        Function: disconnect_pin_instance
        Disconnect a pin_instance from netlist and Open Timer.
        */
        void disconnect_pin(std::string pin_instance_name);

        /*
        Function: connect_pin_instance
        Connect a pin_instance with a given netname in netlist and Open Timer.
        */
        void connect_pin(std::string pin_instance_name, std::string net_name);

    private:
        ot::Timer m_timer;
        ophidian::design::Design & m_design;
    };
}

#endif // OPHIDIAN_TIMING_OPENTIMER_H
