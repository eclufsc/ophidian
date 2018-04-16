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


#include <iostream>
#include <vector>
#include <fstream>
#include <utility>
#include <unordered_map>

#ifdef __cplusplus
extern "C" {
#endif
#include <verilog_parser.h>
#include <verilog_ast_util.h>
#ifdef __cplusplus
}
#endif

#include "Verilog.h"
#include "ParserException.h"

namespace ophidian
{
namespace parser
{
    Verilog::Verilog(const std::string& verilog_file):
        m_modules{}
    {
        read_file(verilog_file);
    }

    Verilog::Verilog(const std::vector<std::string>& verilog_files):
        m_modules{}
    {
        for(const auto& file_name : verilog_files){
            read_file(file_name);
        }
    }

    Verilog::Verilog(std::istream& verilog_stream):
        m_modules{}
    {
        read_stream(verilog_stream);
    }

    Verilog::Verilog(std::vector<std::istream>& verilog_streams):
        m_modules{}
    {
        for(auto& verilog_stream : verilog_streams){
            read_stream(verilog_stream);
        }
    }

    void Verilog::read_file(const std::string& verilog_file)
    {
        auto input = std::ifstream{verilog_file};
        if(input.is_open()) {
            read_stream(input);
            input.close();
        }
        else {
            throw exceptions::InexistentFile();
        }
    }

    void Verilog::read_stream(std::istream& verilog_stream)
    {
        verilog_parser_init();

        std::vector<char> buffer((std::istreambuf_iterator<char>(verilog_stream)),
            std::istreambuf_iterator<char>());

        int result = verilog_parse_string(buffer.data(), buffer.size());

        if(result) {
            throw exceptions::VerilogRuntimeException();
        }

        auto source_tree = yy_verilog_source_tree;

        verilog_resolve_modules(source_tree);

        for(auto module_it = source_tree->modules->head; module_it; module_it = module_it->next) {

            auto module = static_cast<ast_module_declaration*>(module_it->data);

            auto ports = Verilog::Module::container_type<Verilog::Module::Port>{};

            for(auto port_it = module->module_ports->head; port_it; port_it = port_it->next){
                
                auto port = static_cast<ast_port_declaration*>(port_it->data);

                auto direction = Verilog::Module::Port::Direction{};
                switch(port->direction)
                {
                case PORT_INPUT:
                    direction = Verilog::Module::Port::Direction::INPUT;

                case PORT_OUTPUT:
                    direction = Verilog::Module::Port::Direction::OUTPUT;

                case PORT_INOUT:
                    direction = Verilog::Module::Port::Direction::INOUT;

                case PORT_NONE:
                    direction = Verilog::Module::Port::Direction::NONE;
                }

                for(auto name_it = port->port_names->head; name_it; name_it = name_it->next)
                {
                    auto port_identifier = static_cast<ast_identifier>(name_it->data);
                    auto name = port_identifier->identifier;

                    ports.emplace_back(std::move(name), std::move(direction));
                }
            }

            m_modules.emplace_back(
                std::string(module->identifier->identifier),
                std::move(ports), 
                std::vector<Verilog::Module::Net>{}, 
                std::vector<Verilog::Module>{}, 
                std::vector<Verilog::Module::Module_instance>{}
            );
        }
    }

    const Verilog::container_type<Verilog::Module>& Verilog::modules() const noexcept
    {
        return m_modules;
    }

    Verilog::Module::Module(const Verilog::Module::string_type& name):
        m_name{name},
        m_ports{},
        m_nets{},
        m_modules{},
        m_module_instances{}
    {}

    Verilog::Module::Module(Verilog::Module::string_type&& name):
        m_name{std::move(name)},
        m_ports{},
        m_nets{},
        m_modules{},
        m_module_instances{}
    {}

    const Verilog::Module::string_type& Verilog::Module::name() const noexcept
    {
        return m_name;
    }

    const Verilog::Module::container_type<Verilog::Module::Port> & Verilog::Module::ports() const noexcept
    {
        return m_ports;
    }

    const Verilog::Module::container_type<Verilog::Module::Net>& Verilog::Module::nets() const noexcept
    {
        return m_nets;
    }

    const Verilog::Module::container_type<Verilog::Module>& Verilog::Module::modules() const noexcept
    {
        return m_modules;
    }

    const Verilog::Module::container_type<Verilog::Module::Module_instance>& Verilog::Module::module_instances() const noexcept
    {
        return m_module_instances;
    }

    const Verilog::Module::Port::direction_type& Verilog::Module::Port::direction() const noexcept
    {
        return m_direction;
    }

    const Verilog::Module::Port::string_type& Verilog::Module::Port::name() const noexcept
    {
        return m_name;
    }

    bool Verilog::Module::Port::operator==(const Verilog::Module::Port& rhs) const noexcept
    {
        return m_name == rhs.m_name && m_direction == rhs.m_direction;
    }

    Verilog::Module::Net::Net(const Verilog::Module::Net::string_type& name):
        m_name{name}
    {}

    Verilog::Module::Net::Net(Verilog::Module::Net::string_type&& name):
        m_name{std::move(name)}
    {}

    const Verilog::Module::Net::string_type& Verilog::Module::Net::name() const noexcept
    {
        return m_name;
    }

    bool Verilog::Module::Net::operator==(const Verilog::Module::Net& rhs) const noexcept
    {
        return m_name == rhs.m_name;
    }

    Verilog::Module::Module_instance::Module_instance(
        const Verilog::Module::Module_instance::string_type& name, 
        const Verilog::Module::Module_instance::module_type& module):
        m_name{name},
        m_module{&module}
    {}

    Verilog::Module::Module_instance::Module_instance(
        Verilog::Module::Module_instance::string_type&& name, 
        const Verilog::Module::Module_instance::module_type& module):
        m_name{std::move(name)},
        m_module{&module}
    {}

    const Verilog::Module::Module_instance::string_type& Verilog::Module::Module_instance::name() const noexcept
    {
        return m_name;
    }

    const Verilog::Module::Module_instance::module_type& Verilog::Module::Module_instance::module() const noexcept
    {
        return *m_module;
    }
}     // namespace parser
}     // namespace ophidian
