#ifndef ophidian_EXPERIMENT_H
#define ophidian_EXPERIMENT_H

#include "../src/netlist/verilog2netlist.h"
#include "../src/netlist/netlist.h"
#include "../src/parsing/def.h"
#include "../src/parsing/lef.h"
#include "../src/parsing/verilog.h"
#include "../src/placement/def2placement.h"
#include "../src/placement/lef2library.h"
#include "../src/placement/placement.h"
#include "../src/placement/library.h"
#include "../src/standard_cell/standard_cells.h"

using namespace ophidian;

struct Experiment{
    std::string circuit_name;
    std::unique_ptr<parsing::def> def;
    std::unique_ptr<parsing::lef> lef;
    std::unique_ptr<parsing::verilog> verilog;
    std::unique_ptr<standard_cell::standard_cells> m_std_cells;
    std::unique_ptr<netlist::netlist> m_netlist;
    std::unique_ptr<placement::library> m_placement_lib;
    std::unique_ptr<placement::placement> m_placement;

    Experiment(std::string circuit_name):circuit_name(circuit_name){
        def.reset(new parsing::def("./benchmarks/"+circuit_name+"/"+circuit_name+".def"));
        lef.reset(new parsing::lef("./benchmarks/"+circuit_name+"/"+circuit_name+".lef"));
        verilog.reset(new parsing::verilog("./benchmarks/"+circuit_name+"/"+circuit_name+".v"));

        m_std_cells.reset(new standard_cell::standard_cells());
        m_netlist.reset(new netlist::netlist(m_std_cells.get()));
        netlist::verilog2netlist(*verilog, *m_netlist);
        m_placement_lib.reset(new placement::library(m_std_cells.get()));
        m_placement.reset(new placement::placement(m_netlist.get(), m_placement_lib.get()));
        placement::def2placement(*def, *m_placement);
        placement::lef2library(*lef, *m_placement_lib);
    }
};

#endif //ophidian_EXPERIMENT_H
