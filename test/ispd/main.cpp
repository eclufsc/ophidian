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
#include "../geometry/geometry.h"

using namespace ophidian;

//--------------------------------OOD--------------------------------

class Pin;

class Net{
    std::string m_name;
    std::vector<Pin*> m_pins;
public:
    Net(std::string &name):m_name(name){}
    std::string name(){return m_name;}
    void reserve(std::size_t size){m_pins.reserve(size);}
    void add_pin(Pin * pin){m_pins.push_back(pin);}
    const std::vector<Pin*> & pins() const{return m_pins;}
};

class Pin{
    std::string m_name;
    std::unique_ptr<Net> m_net_ptr;
public:
    Pin(std::string name, Net * net):m_name(name){m_net_ptr.reset(net);}
};

class PinPlacement : public Pin{
    geometry::point<double> m_position;
public:
    PinPlacement(std::string name, Net * net): Pin(name, net){}
    void set_position(const geometry::point<double> &position){m_position = position;}
    const geometry::point<double> & pin_position(){return m_position;}
};

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

    std::vector<Net> m_nets;
    m_nets.reserve(netlist.net_system().size());
    for(auto net : netlist.net_system()){
      std::string net_name = netlist.net_name(net);
      Net net_object(net_name);
      m_nets.push_back(net_object);
      for(auto pin : netlist.net_pins(net)){
        std::string pin_name = netlist.pin_name(pin);
        PinPlacement * pin_object = new PinPlacement(pin_name, &net_object);
        pin_object->set_position(m_placement.pin_position(pin));
        m_nets.back().add_pin(pin_object);
      }
    }





    auto time_start = std::chrono::high_resolution_clock::now();
    for(auto net : m_nets){
        //std::cout<<net.name()<<":";
        std::vector<geometry::point<double>> pin_positions;
        for(auto pin : net.pins())
            pin_positions.push_back(static_cast<PinPlacement*>(pin)->pin_position());
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

//--------------------------------DOD--------------------------------

/*
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

    m_placement.set_all_pin_positions();

    auto time_start = std::chrono::high_resolution_clock::now();
    for(auto net : netlist.net_system()){
//        std::cout<<netlist.net_name(net)<<":";
        std::vector<geometry::point<double>> pin_positions;
        for(auto pin : netlist.net_pins(net))
            pin_positions.push_back(m_placement.get_pin_position(pin));
        interconnection::hpwl(pin_positions);
        interconnection::stwl(pin_positions);
//        std::cout<<" hpwl= "<<interconnection::hpwl(pin_positions);
//        std::cout<<", stwl= "<<interconnection::stwl(pin_positions)<<std::endl;
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto total_time = time_end - time_start;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count()<<" ms"<<std::endl;
    return 0;
}
*/
