#include "VerilogParser.h"
#include <3rdparty/verilog-parser/src/verilog_parser.h>

namespace ophidian
{
namespace parser
{

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

VerilogParser::VerilogParser()
{

}

Verilog* VerilogParser::readStream(std::istream &in)
{
    auto input = std::make_unique<Verilog>();

    return input.release();
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

Verilog::Net::Net(const std::string &name) :
    name_(name)
{

}

const std::string &Verilog::Net::name() const
{
    return name_;
}

Verilog::Instance::Instance(Verilog::Module *module, const std::string name):
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

void Verilog::Instance::mapPort(Verilog::Port *port, Verilog::Net *net)
{
    portMapping_[port] = net;
}

const std::map<Verilog::Port *, Verilog::Net *> &Verilog::Instance::portMapping() const
{
    return portMapping_;
}

}
}

