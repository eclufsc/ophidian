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

#include "Timer.h"

namespace ophidian::timing{
    OpenTimer::OpenTimer(ophidian::design::Design & design, std::string early_lib_path, std::string late_lib_path, std::string sdc_path, std::string spef_path):m_timer(), m_design(design){
        m_timer.read_celllib(late_lib_path, ot::Split::MAX);
        m_timer.read_celllib(early_lib_path, ot::Split::MIN);

        for(auto cell_it = m_design.netlist().begin_cell_instance(); cell_it != m_design.netlist().end_cell_instance(); cell_it++){
            auto cell_name = m_design.netlist().name(*cell_it);
            auto std_cell = m_design.netlist().std_cell(*cell_it);
            auto std_cell_name = design.standard_cells().name(std_cell);
            m_timer.insert_gate(cell_name, std_cell_name);
        }

        for(auto input_pad_it = m_design.netlist().begin_input_pad(); input_pad_it != m_design.netlist().end_input_pad(); input_pad_it++){
            auto input_pin = m_design.netlist().pin(*input_pad_it);
            auto input_pad_name = m_design.netlist().name(input_pin);
            m_timer.insert_primary_input(input_pad_name);
        }

        for(auto output_pad_it = m_design.netlist().begin_output_pad(); output_pad_it != m_design.netlist().end_output_pad(); output_pad_it++){
            auto output_pin = m_design.netlist().pin(*output_pad_it);
            auto output_pad_name = m_design.netlist().name(output_pin);
            m_timer.insert_primary_output(output_pad_name);
        }

        for(auto net_it = m_design.netlist().begin_net(); net_it != m_design.netlist().end_net(); net_it++){
            auto net_name = m_design.netlist().name(*net_it);
            m_timer.insert_net(net_name);
            for(auto net_pin : m_design.netlist().pins(*net_it)){
                auto pin_name = m_design.netlist().name(net_pin);
                m_timer.connect_pin(pin_name, net_name);
            }
        }

        m_timer.read_spef(spef_path);
        m_timer.read_sdc(sdc_path);
    }

    float OpenTimer::at(const std::string& pin_name, split_type_type split_type, transition_type transition_type){
        return *m_timer.at(pin_name, split_type, transition_type);
    }

    float OpenTimer::rat(const std::string& pin_name, split_type_type split_type, transition_type transition_type){
        return *m_timer.rat(pin_name, split_type, transition_type);
    }

    float OpenTimer::slew(const std::string& pin_name, split_type_type split_type, transition_type transition_type){
        return *m_timer.slew(pin_name, split_type, transition_type);
    }

    float OpenTimer::slack(const std::string& pin_name, split_type_type split_type, transition_type transition_type){
        return *m_timer.slack(pin_name, split_type, transition_type);
    }

    float OpenTimer::load(const std::string& pin_name, split_type_type split_type, transition_type transition_type){
        return *m_timer.load(pin_name, split_type, transition_type);
    }

    float OpenTimer::leakage_power(){
        return *m_timer.leakage_power();
    }

    float OpenTimer::tns(){
        return *m_timer.tns();
    }

    float OpenTimer::wns(){
        return *m_timer.wns();
    }

    void OpenTimer::update_timing(){
        m_timer.update_timing();
    }

    void OpenTimer::cppr(bool mode){
        m_timer.cppr(mode);
    }

    void OpenTimer::insert_net(std::string net_name){
        m_design.netlist().add_net(net_name);
        m_timer.insert_net(net_name);
    }

    void OpenTimer::insert_cell_instance(std::string cell_instance, std::string standard_cell){
        auto std_cell = m_design.standard_cells().add_cell(standard_cell);
        auto instance = m_design.netlist().add_cell_instance(cell_instance);
        m_design.netlist().connect(instance, std_cell);
        m_timer.insert_gate(cell_instance, standard_cell);
    }

    void OpenTimer::repower_cell_instance(std::string cell_instance, std::string standard_cell){
        auto std_cell = m_design.standard_cells().find_cell(standard_cell);
        auto instance = m_design.netlist().find_cell_instance(cell_instance);
        m_design.netlist().connect(instance, std_cell);
        m_timer.repower_gate(cell_instance, standard_cell);
    }

    void OpenTimer::remove_net(std::string net_name){
        auto net = m_design.netlist().find_net(net_name);
        auto net_pins = m_design.netlist().pins(net);
        for(auto pin : net_pins){
            m_design.netlist().disconnect(pin);
            m_timer.disconnect_pin(m_design.netlist().name(pin));
        }
        m_design.netlist().erase(net);
        m_timer.remove_net(net_name);
    }

    void OpenTimer::remove_cell_instance(std::string cell_instance_name){
        auto cell_instance = m_design.netlist().find_cell_instance(cell_instance_name);
        auto cell_pins = m_design.netlist().pins(cell_instance);
        for(auto pin : cell_pins){
            m_design.netlist().disconnect(pin);
            m_timer.disconnect_pin(m_design.netlist().name(pin));
            m_design.netlist().erase(pin);
        }
        m_design.netlist().erase(cell_instance);
        m_timer.remove_gate(cell_instance_name);
    }

    void OpenTimer::disconnect_pin(std::string pin_instance_name){
        auto pin = m_design.netlist().find_pin_instance(pin_instance_name);
        m_design.netlist().disconnect(pin);
        m_timer.disconnect_pin(pin_instance_name);
    }

    void OpenTimer::connect_pin(std::string pin_instance_name, std::string net_name){
        auto pin = m_design.netlist().find_pin_instance(pin_instance_name);
        auto net = m_design.netlist().find_net(net_name);
        m_design.netlist().connect(net, pin);
        m_timer.connect_pin(pin_instance_name, net_name);
    }
}
