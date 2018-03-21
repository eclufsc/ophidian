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

#include "VerilogParser.h"

#include <vector>
#include <unordered_map>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif
#include <verilog_parser.h>
#include <verilog_ast_util.h>
#ifdef __cplusplus
}
#endif

namespace ophidian
{
namespace parser
{
namespace
{
    using ModuleDeclaration = ast_module_declaration;
    using ModuleInst = ast_module_instantiation;
    using ModuleInstance = ast_module_instance;
    using ListElement = ast_list_element;
    using PortDeclaration = ast_port_declaration;
    using Identifier = ast_identifier;
    using PortConnection = ast_port_connection;
    using Expression = ast_expression;
    using NetDeclaration = ast_net_declaration;
    using PortDirection = ast_port_direction;
}     // namespace

    Verilog::Module * Verilog::addModule(const std::string & name)
    {
        Module module(name);

        mModules.push_back(module);

        return &mModules.back();
    }

    const std::list<Verilog::Module> & Verilog::modules() const
    {
        return mModules;
    }

    Verilog * VerilogParser::readStream(std::istream & in) const
    {
        verilog_parser_init();
        std::vector<char> buffer((std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());
        int result = verilog_parse_string(buffer.data(), buffer.size());

        if(result) {
            return nullptr;
        }

        auto      inp = std::make_unique<Verilog>();
        Verilog * verilog = inp.get();
        auto      source = yy_verilog_source_tree;
        ModuleDeclaration * vModule =
            static_cast<ModuleDeclaration *>(ast_list_get(source->modules, 0));
        auto module = verilog->addModule(vModule->identifier->identifier);

        std::unordered_map<std::string, Verilog::Net *> netMapping;
        for(ListElement * i = vModule->net_declarations->head; i; i = i->next)
        {
            NetDeclaration *  verilogNet = static_cast<NetDeclaration *>(i->data);
            const std::string netName(verilogNet->identifier->identifier);
            if(netMapping.count(netName) == 0) {
                netMapping[netName] = module->addNet(netName);
            }
        }

        for(ListElement * i = vModule->module_ports->head; i; i = i->next)
        {
            PortDeclaration * vPort = static_cast<PortDeclaration *>(i->data);

            Verilog::Port::Direction direction;
            switch(vPort->direction)
            {
            case PORT_INPUT:
                direction = Verilog::Port::Direction::INPUT;

            case PORT_OUTPUT:
                direction = Verilog::Port::Direction::OUTPUT;

            case PORT_INOUT:
                direction = Verilog::Port::Direction::INOUT;

            case PORT_NONE:
                direction = Verilog::Port::Direction::NONE;
            }

            for(ListElement * portNameWalker = vPort->port_names->head; portNameWalker;
                portNameWalker = portNameWalker->next)
            {
                Identifier identifier = static_cast<Identifier>(portNameWalker->data);
                module->addPort(direction, identifier->identifier);
            }
        }

        std::unordered_map<std::string, Verilog::Module *> moduleMapping;
        for(ListElement * i = vModule->module_instantiations->head; i; i = i->next)
        {
            ModuleInst *      vInstanciation = static_cast<ModuleInst *>(i->data);
            const std::string moduleId = vInstanciation->module_identifer->identifier;
            if(moduleMapping.count(moduleId) == 0) {
                moduleMapping[moduleId] = module->addModule(moduleId);
            }
            Verilog::Module * theModule = moduleMapping.at(moduleId);
            for(ListElement * j = vInstanciation->module_instances->head; j; j = j->next)
            {
                ModuleInstance *    vInstance = static_cast<ModuleInstance *>(j->data);
                Verilog::Instance * instance = module->addInstance(
                    theModule,
                    vInstance->instance_identifier->identifier);
                for(ListElement * k = vInstance->port_connections->head; k; k = k->next)
                {
                    PortConnection *      vConnection = static_cast<PortConnection *>(k->data);
                    Expression *          exp = vConnection->expression;
                    const std::string     portName = vConnection->port_name->identifier;
                    const std::string     netName = exp->primary->value.identifier->identifier;
                    const Verilog::Port * port = nullptr;
                    for(auto const & currPort : theModule->ports())
                    {
                        if(currPort.name() == portName) {
                            port = &currPort;
                            break;
                        }
                    }
                    if(!port) {
                        port = theModule->addPort(Verilog::Port::Direction::NONE, portName);
                    }
                    auto netIt = netMapping.find(netName);
                    if(netIt == netMapping.end()) {
                        netIt =
                            netMapping.insert(
                                netIt,
                                std::make_pair(netName, module->addNet(netName)));
                    }
                    const Verilog::Net * net = netIt->second;
                    instance->mapPort(port, net);
                }
            }
        }

        // TODO: verify how to free the syntax tree.
        yy_preproc = nullptr;
        yy_verilog_source_tree = nullptr;

        ast_free_all();

        return inp.release();
    }

    Verilog * VerilogParser::readFile(const std::string & filename) const
    {
        std::ifstream input(filename.c_str(), std::ifstream::in);
        if(!input.good()) {
            return nullptr;
        }

        return readStream(input);
    }

    Verilog::Module::Module(const std::string & name)
    {
        mName = name;
    }

    Verilog::Port * Verilog::Module::addPort(
        Verilog::Port::Direction direction,
        const std::string name)
    {
        Port p(direction, name);

        mPorts.push_back(p);

        return &mPorts.back();
    }

    Verilog::Net * Verilog::Module::addNet(const std::string & name)
    {
        Net n(name);

        mNets.push_back(n);

        return &mNets.back();
    }

    Verilog::Module * Verilog::Module::addModule(const std::string & name)
    {
        Module m(name);

        mModules.push_back(m);

        return &mModules.back();
    }

    Verilog::Instance * Verilog::Module::addInstance(
        Verilog::Module * module,
        const std::string & name)
    {
        Instance inst(module, name);

        mInstances.push_back(inst);

        return &mInstances.back();
    }

    const std::string & Verilog::Module::name() const
    {
        return mName;
    }

    const std::list<Verilog::Port> & Verilog::Module::ports() const
    {
        return mPorts;
    }

    const std::list<Verilog::Net> & Verilog::Module::nets() const
    {
        return mNets;
    }

    const std::list<Verilog::Module> & Verilog::Module::modules() const
    {
        return mModules;
    }

    const std::list<Verilog::Instance> & Verilog::Module::instances() const
    {
        return mInstances;
    }

    Verilog::Port::Port(Verilog::Port::Direction direction, const std::string & name):
            mDirection(direction),
            mName(name)
    {
    }

    Verilog::Port::Direction Verilog::Port::direction() const
    {
        return mDirection;
    }

    const std::string & Verilog::Port::name() const
    {
        return mName;
    }

    bool Verilog::Port::operator==(const Verilog::Port & o) const
    {
        return mName == o.mName && mDirection == o.mDirection;
    }

    Verilog::Net::Net(const std::string & name):
            mName(name)
    {
    }

    const std::string & Verilog::Net::name() const
    {
        return mName;
    }

    bool Verilog::Net::operator==(const Verilog::Net & o) const
    {
        return mName == o.mName;
    }

    Verilog::Instance::Instance(Verilog::Module * module, const std::string name):
            mModule(module),
            mName(name)
    {
    }

    Verilog::Module * Verilog::Instance::module() const
    {
        return mModule;
    }

    const std::string & Verilog::Instance::name() const
    {
        return mName;
    }

    void Verilog::Instance::mapPort(const Verilog::Port * port, const Verilog::Net * net)
    {
        mPortMapping[port] = net;
    }

    const std::map<const Verilog::Port *,
        const Verilog::Net *> & Verilog::Instance::portMapping() const
    {
        return mPortMapping;
    }
}     // namespace parser
}     // namespace ophidian
