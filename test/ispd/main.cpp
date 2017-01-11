#include <iostream>
#include <memory>
#include <chrono>

#include "../src/parsing/def.h"
#include "../src/parsing/lef.h"
#include "../src/parsing/verilog.h"
#include "../src/netlist/verilog2netlist.h"
#include "../src/placement/def2placement.h"
#include "../src/placement/lef2library.h"
#include "../src/netlist/netlist.h"
#include "../src/standard_cell/standard_cells.h"
#include "../src/placement/placement.h"
#include "../src/placement/library.h"
#include "../src/interconnection/hpwl.h"
#include "../src/interconnection/stwl.h"

using namespace ophidian;


int main(int argc, char **argv) {
    std::string circuit_name = argv[1];

    std::unique_ptr<parsing::def> def;
    std::unique_ptr<parsing::lef> lef;
    std::unique_ptr<parsing::verilog> v;
    v.reset(new parsing::verilog("./benchmarks/"+circuit_name+"/"+circuit_name+".v"));
    def.reset(new parsing::def("./benchmarks/"+circuit_name+"/"+circuit_name+".def"));
    lef.reset(new parsing::lef("./benchmarks/"+circuit_name+"/"+circuit_name+".lef"));



    standard_cell::standard_cells m_std_cells;
    netlist::netlist m_netlist{&m_std_cells};
    netlist::verilog2netlist(*v, m_netlist);
    placement::library m_placement_lib{&m_std_cells};
    placement::placement m_placement{&m_netlist, &m_placement_lib};

    placement::def2placement(*def, m_placement);
    placement::lef2library(*lef, m_placement_lib);

    netlist::netlist netlist = m_placement.netlist();

    auto time_start = std::chrono::high_resolution_clock::now();
    for(auto net : netlist.net_system()){
        //std::cout<<netlist.net_name(net)<<":";
        std::vector<geometry::point<double>> pin_positions;
        auto pin_vector = netlist.net_pins(net);
        for(auto pin : pin_vector)
            pin_positions.push_back(m_placement.pin_position(pin));
        interconnection::hpwl(pin_positions);
        interconnection::stwl(pin_positions);
        //std::cout<<" hpwl= "<<interconnection::hpwl(pin_positions);
        //std::cout<<", stwl= "<<interconnection::stwl(pin_positions)<<std::endl;
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto total_time = time_end - time_start;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count()<<" ms"<<std::endl;
    return 0;
}
