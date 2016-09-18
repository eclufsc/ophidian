#ifndef VERILOGPARSER_H
#define VERILOGPARSER_H

#include <istream>
#include <list>
#include <map>

namespace ophidian
{
namespace parser
{

class Verilog
{
public:
    enum class PortDirection
    {
        INPUT,
        OUTPUT,
        INOUT
    };

    class Port
    {
    public:
        Port(PortDirection direction, const std::string &name);
        PortDirection direction() const;
        const std::string &name() const;
    private:
        PortDirection direction_;
        std::string name_;
    };

    class Net
    {
    public:
        Net(const std::string & name);
        const std::string& name() const;
    private:
        std::string name_;
    };

    class Module;
    class Instance
    {
    public:
        Instance(Module * module, const std::string name);
        Module * module() const;
        const std::string & name() const;
        void mapPort(Port* port, Net * net);
        const std::map<Port*, Net*> & portMapping() const;

    private:
        Module * module_;
        std::string name_;
        std::map<Port*, Net*> portMapping_;
    };

    class Module
    {
    public:
        Module(const std::string& name);

        Port *addPort(Verilog::PortDirection direction, const std::string name);
        Net *addNet(const std::string & name);
        Module *addModule(const std::string & name);
        Instance *addInstance(Module * module, const std::string & name);

        const std::string & name() const;
        const std::list<Port> & ports() const;
        const std::list<Net> & nets() const;
        const std::list<Module> &modules() const;
        const std::list<Instance> & instances() const;
    private:
        std::string name_;

        std::list<Port> ports_;
        std::list<Net> nets_;
        std::list<Module> modules_;
        std::list<Instance> instances_;
    };


    Module* addModule(const std::string & name);
    const std::list<Module> & modules() const;

private:
    std::list<Module> modules_;
};

class VerilogParser
{
public:
    VerilogParser();

    Verilog *readStream(std::istream & in);
};

}
}

#endif // VERILOGPARSER_H
