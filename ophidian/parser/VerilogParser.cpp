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
#include <3rdparty/verilog-parser/src/verilog_parser.h>
#include <3rdparty/verilog-parser/src/verilog_ast_util.h>
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

} // namespace

Verilog::Module* Verilog::addModule(const std::string &name)
{
    Module module(name);
    modules_.push_back(module);
    return &modules_.back();
}

const std::list<Verilog::Module> &Verilog::modules() const
{
    return modules_;
}

struct VerilogParser::Impl
{
    Impl()
    {
        directionMapping[PORT_INPUT] = Verilog::PortDirection::INPUT;
        directionMapping[PORT_OUTPUT] = Verilog::PortDirection::OUTPUT;
        directionMapping[PORT_INOUT] = Verilog::PortDirection::INOUT;
        directionMapping[PORT_NONE] = Verilog::PortDirection::NONE;
    }

    std::map<PortDirection, Verilog::PortDirection> directionMapping;
};

VerilogParser::VerilogParser() :
    this_(new Impl)
{

}

VerilogParser::~VerilogParser()
{

}

Verilog* VerilogParser::readStream(std::istream &in)
{
    verilog_parser_init();
    std::vector<char> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    int result = verilog_parse_string(buffer.data(), buffer.size());

    if (result)
    {
        return nullptr;
    }

    auto inp = std::make_unique<Verilog>();
    Verilog* verilog = inp.get();
    auto source = yy_verilog_source_tree;
    ModuleDeclaration* vModule = static_cast<ModuleDeclaration*>(ast_list_get(source->modules, 0));
    auto module = verilog->addModule(vModule->identifier->identifier);

    std::unordered_map<std::string, Verilog::Net*> netMapping;
    for(ListElement * i = vModule->net_declarations->head; i; i = i->next)
    {
        NetDeclaration* verilogNet = static_cast<NetDeclaration*>(i->data);
        const std::string netName(verilogNet->identifier->identifier);
        if(netMapping.count(netName) == 0)
        {
            netMapping[netName] = module->addNet(netName);
        }
    }

    for( ListElement* i = vModule->module_ports->head; i; i = i->next)
    {
        PortDeclaration* vPort = static_cast<PortDeclaration*>(i->data);
        auto direction = this_->directionMapping.at(vPort->direction);
        for(ListElement* portNameWalker = vPort->port_names->head; portNameWalker; portNameWalker = portNameWalker->next)
        {
            Identifier identifier = static_cast<Identifier>(portNameWalker->data);
            module->addPort(direction, identifier->identifier);
        }
    }

    std::unordered_map<std::string, Verilog::Module*> moduleMapping;
    for(ListElement* i =  vModule->module_instantiations->head; i; i = i->next)
    {
        ModuleInst* vInstanciation = static_cast<ModuleInst*>(i->data);
        const std::string moduleId = vInstanciation->module_identifer->identifier;
        if(moduleMapping.count(moduleId) == 0)
        {
            moduleMapping[moduleId] = module->addModule(moduleId);
        }
        Verilog::Module * theModule = moduleMapping.at(moduleId);
        for(ListElement* j = vInstanciation->module_instances->head; j; j = j->next)
        {
            ModuleInstance* vInstance = static_cast<ModuleInstance*>(j->data);
            Verilog::Instance * instance = module->addInstance(theModule, vInstance->instance_identifier->identifier);
            for(ListElement * k =  vInstance->port_connections->head; k; k = k->next)
            {
                PortConnection* vConnection = static_cast<PortConnection*>(k->data);
                Expression* exp = vConnection->expression;
                const std::string portName = vConnection->port_name->identifier;
                const std::string netName = exp->primary->value.identifier->identifier;
                const Verilog::Port * port = nullptr;
                for(auto const & currPort : theModule->ports())
                {
                    if(currPort.name() == portName)
                    {
                        port = &currPort;
                        break;
                    }
                }
                if(!port)
                {
                    port = theModule->addPort(Verilog::PortDirection::NONE, portName);
                }
                auto netIt = netMapping.find(netName);
                if(netIt == netMapping.end())
                {
                    netIt = netMapping.insert(netIt, std::make_pair(netName, module->addNet(netName)));
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

Verilog *VerilogParser::readFile(const std::string &filename)
{
    std::ifstream input(filename.c_str(), std::ifstream::in);
    if(!input.good())
    {
        return nullptr;
    }
    return readStream(input);
}

Verilog::Module::Module(const std::string &name)
{
    name_ = name;
}

Verilog::Port* Verilog::Module::addPort(Verilog::PortDirection direction, const std::string name)
{
    Port p(direction, name);
    ports_.push_back(p);
    return &ports_.back();
}

Verilog::Net *Verilog::Module::addNet(const std::string &name)
{
    Net n(name);
    nets_.push_back(n);
    return &nets_.back();
}

Verilog::Module *Verilog::Module::addModule(const std::string &name)
{
    Module m(name);
    modules_.push_back(m);
    return &modules_.back();
}

Verilog::Instance *Verilog::Module::addInstance(Verilog::Module *module, const std::string &name)
{
    Instance inst(module, name);
    instances_.push_back(inst);
    return &instances_.back();
}

const std::string &Verilog::Module::name() const
{
    return name_;

}

const std::list<Verilog::Port> &Verilog::Module::ports() const
{
    return ports_;
}

const std::list<Verilog::Net> &Verilog::Module::nets() const
{
    return nets_;
}

const std::list<Verilog::Module> &Verilog::Module::modules() const
{
    return modules_;
}

const std::list<Verilog::Instance> &Verilog::Module::instances() const
{
    return instances_;
}

Verilog::Port::Port(Verilog::PortDirection direction, const std::string &name) :
    direction_(direction),
    name_(name)
{
}

Verilog::PortDirection Verilog::Port::direction() const
{
    return direction_;
}

const std::string &Verilog::Port::name() const
{
    return name_;
}

bool Verilog::Port::operator==(const Verilog::Port &o) const
{
    return name_ == o.name_ && direction_ == o.direction_;
}

Verilog::Net::Net(const std::string &name) :
    name_(name)
{

}

const std::string &Verilog::Net::name() const
{
    return name_;
}

bool Verilog::Net::operator==(const Verilog::Net &o) const
{
    return name_ == o.name_;
}

Verilog::Instance::Instance(Verilog::Module *module, const std::string name) :
    module_(module),
    name_(name)
{

}

Verilog::Module *Verilog::Instance::module() const
{
    return module_;
}

const std::string &Verilog::Instance::name() const
{
    return name_;
}

void Verilog::Instance::mapPort(const Verilog::Port *port, const Verilog::Net *net)
{
    portMapping_[port] = net;
}

const std::map<const Verilog::Port *, const Verilog::Net *> &Verilog::Instance::portMapping() const
{
    return portMapping_;
}

} // namespace parser
} // namespace ophidian

