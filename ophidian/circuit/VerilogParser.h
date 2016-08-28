#ifndef VERILOGPARSER_H
#define VERILOGPARSER_H

#include "Netlist.h"

namespace ophidian {
namespace circuit {

class VerilogParser
{
public:
    VerilogParser();
    Netlist *createNetlist(std::istream & input);
private:
    std::map<std::string, Cell> cells_;
    std::map<std::string, Pin> pins_;
    std::map<std::string, Net> nets_;
    Cell createCell(Netlist *nl, const std::string & name);
    Pin createPin(Netlist *nl, const std::string & name);
    Net createNet(Netlist *nl, const std::string & name);
};

}
}

#endif // VERILOGPARSER_H
