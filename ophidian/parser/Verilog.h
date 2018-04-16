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

#ifndef VERILOG_H
#define VERILOG_H

#include <list>
#include <map>
#include <istream>

namespace ophidian
{
namespace parser
{
    class Verilog
    {
    public:
        // Class member types
        class Module;

        template <class T> using container_type = std::vector<T>;

        using module_type                       = Module;
        using module_container_type             = container_type<module_type>;

        // Class constructors
        Verilog() = default;

        Verilog(const Verilog&) = delete;
        Verilog& operator=(const Verilog&) = delete;

        Verilog(Verilog&&) = default;
        Verilog& operator=(Verilog&&) = default;

        Verilog(const std::string& verilog_file);
        Verilog(const std::vector<std::string>& verilog_files); 

        Verilog(std::istream& verilog_stream);
        Verilog(std::vector<std::istream>& verilog_streams); 

        //Class member functions
        void read_file(const std::string& verilog_file);
        void read_stream(std::istream& verilog_stream);

        const module_container_type& modules() const noexcept;

    private:
        module_container_type m_modules;
    };

    class Verilog::Module
    {
    public:
        // Class member types
        class Port;
        class Net;
        class Module_instance;

        template <class T> using container_type = std::vector<T>;

        using string_type = std::string;

        using module_type = Module;
        using module_container_type = container_type<module_type>;

        using port_type = Port;
        using port_container_type = container_type<port_type>;

        using net_type = Net;
        using net_container_type = container_type<net_type>;

        using module_instance_type = Module_instance;
        using module_instance_container_type = container_type<module_instance_type>;

        // Class constructors
        Module() = delete;

        Module(const Module&) = delete;
        Module& operator=(const Module&) = delete;

        Module(Module&&) = default;
        Module& operator=(Module&&) = default;

        Module(const string_type& name);
        Module(string_type&& name);

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        Module(Arg1&& name, Arg2&& ports, Arg3&& nets, Arg4&& modules, Arg5&& module_instances):
            m_name{std::forward<Arg1>(name)},
            m_ports{std::forward<Arg2>(ports)},
            m_nets{std::forward<Arg3>(nets)},
            m_modules{std::forward<Arg4>(modules)},
            m_module_instances{std::forward<Arg5>(module_instances)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const port_container_type& ports() const noexcept;

        const net_container_type& nets() const noexcept;

        const module_container_type& modules() const noexcept;

        const module_instance_container_type& module_instances() const noexcept;

    private:
        string_type                    m_name;
        port_container_type            m_ports;
        net_container_type             m_nets;
        module_container_type          m_modules;
        module_instance_container_type m_module_instances;
    };

    class Verilog::Module::Port
    {
    public:
        // Class member types
        enum class Direction : int {
            INPUT, OUTPUT, INOUT, NONE
        };

        using string_type = std::string;
        using direction_type = Direction;

        // Class constructors
        Port() = delete;

        Port(const Port&) = default;
        Port& operator=(const Port&) = default;

        Port(Port&&) = default;
        Port& operator=(Port&&) = default;

        template<class Arg1, class Arg2>
        Port(Arg1&& name, Arg2&& direction):
            m_name{std::forward<Arg1>(name)},
            m_direction{std::forward<Arg2>(direction)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const direction_type& direction() const noexcept;

        bool operator==(const Port& rhs) const noexcept;

    private:
        string_type    m_name;
        direction_type m_direction;
    };

    class Verilog::Module::Net
    {
    public:
        // Class member types
        using string_type = std::string;

        // Class constructors
        Net() = delete;

        Net(const Net&) = default;
        Net& operator=(const Net&) = default;

        Net(Net&&) = default;
        Net& operator=(Net&&) = default;

        Net(const string_type& name);
        Net(string_type&& name);

        // Class member functions
        const string_type& name() const noexcept;

        bool operator==(const Net& rhs) const noexcept;

    private:
        string_type m_name;
    };

    class Verilog::Module::Module_instance
    {
    public:
        // Class member types
        using string_type = std::string;
        using module_type = Module;

        // Class constructors
        Module_instance() = delete;

        Module_instance(const Module_instance&) = default;
        Module_instance& operator=(const Module_instance&) = default;

        Module_instance(Module_instance&&) = default;
        Module_instance& operator=(Module_instance&&) = default;

        Module_instance(const std::string& name, const Module& module);
        Module_instance(std::string&& name, const Module& module);

        // Class member functions
        const string_type& name() const noexcept;

        const module_type& module() const noexcept;

    private:
        string_type  m_name;
        const module_type* m_module;
    };
}     // namespace parser
}     // namespace ophidian

#endif // VERILOGPARSER_H
