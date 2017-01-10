#include <iostream>
#include <memory>

#include "../src/parsing/def.h"
#include "../src/parsing/lef.h"
#include "../src/parsing/verilog.h"
#include "../src/netlist/verilog2netlist.h"
#include "../src/placement/def2placement.h"
#include "../src/netlist/netlist.h"
#include "../src/standard_cell/standard_cells.h"
#include "../src/placement/placement.h"
#include "../src/placement/library.h"

using namespace ophidian;


int main(int argc, char **argv) {
    std::unique_ptr<parsing::def> def;
    std::unique_ptr<parsing::lef> lef;
    std::unique_ptr<parsing::verilog> v;
    v.reset(new parsing::verilog("../input_files/simple.v"));
    def.reset(new parsing::def("../input_files/simple.def"));
    lef.reset(new parsing::lef("../input_files/simple.lef"));


    standard_cell::standard_cells m_std_cells;
    netlist::netlist m_netlist{&m_std_cells};

    netlist::verilog2netlist(*v, m_netlist);

    placement::library m_placement_lib{&m_std_cells};

    placement::placement m_placement{&m_netlist, &m_placement_lib};

    placement::def2placement(*def, m_placement);

    return 0;
}
