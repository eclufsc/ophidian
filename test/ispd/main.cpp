#include <iostream>
#include <memory>
#include <chrono>
#include <papi.h>

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
#include "../src/geometry/geometry.h"
#include "../src/floorplan/floorplan.h"
#include "../src/floorplan/lefdef2floorplan.h"

using namespace ophidian;
//--------------------------------OOD HPWL STWL--------------------------------
/*
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

    long long counters[3];
    int PAPI_events[] = {
        PAPI_L1_TCM,
        PAPI_L2_TCM,
        PAPI_L3_TCM};
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_start_counters( PAPI_events, 3 );

    //auto time_start = std::chrono::high_resolution_clock::now();
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
//    auto time_end = std::chrono::high_resolution_clock::now();
//    auto total_time = time_end - time_start;
//    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count()<<" ms"<<std::endl;

    PAPI_read_counters( counters, 3 );
    std::cout<<"Total misses: "<<(counters[0]+counters[1]+counters[2])<<std::endl;
    return 0;
}
*/




//--------------------------------DOD HPWL STWL--------------------------------
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

    long long counters[3];
    int PAPI_events[] = {
        PAPI_L1_TCM,
        PAPI_L2_TCM,
        PAPI_L3_TCM};
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_start_counters( PAPI_events, 3 );

//    auto time_start = std::chrono::high_resolution_clock::now();
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
//    auto time_end = std::chrono::high_resolution_clock::now();
//    auto total_time = time_end - time_start;
//    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count()<<" ms"<<std::endl;

    PAPI_read_counters( counters, 3 );
    std::cout<<"Total misses: "<<(counters[0]+counters[1]+counters[2])<<std::endl;
    return 0;
}
*/





//--------------------------------LEGALIZATION DOD --------------------------------
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
    floorplan::floorplan m_floorplan;

    placement::def2placement(*def, m_placement);
    placement::lef2library(*lef, m_placement_lib);
    floorplan::lefdef2floorplan(*lef, *def, m_floorplan);

    netlist::netlist netlist = m_placement.netlist();

    geometry::point<double> m_chip_boundaries = m_floorplan.chip_boundaries();


    auto time_start = std::chrono::high_resolution_clock::now();
    bool placemente_is_legal = true;
    for(auto position : m_placement.cell_properties().positions()){
        if(position.x() < 0 || position.y() < 0 || position.x() > m_chip_boundaries.x() || position.y() > m_chip_boundaries.y()){
            placemente_is_legal = false;
            break;
        }
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto total_time = time_end - time_start;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count()<<" ms"<<std::endl;

//    if(placemente_is_legal)
//        std::cout<<"The placement is legal."<<std::endl;
//    else
//        std::cout<<"The placement is Ilegal."<<std::endl;
    return 0;
}
*/




//--------------------------------LEGALIZATION OOD --------------------------------
class Pin{
public:
    Pin(){}
};

class StdCell{
public:
    StdCell(){}
};

//netlist cell
class Cell{
    std::string m_name;
    StdCell * m_standard_cell;
    std::vector<Pin*> m_pins;
public:
    Cell(const std::string & name, std::size_t pins_size): m_name(name){m_pins.reserve(pins_size);}
};

class PlacementCell : public Cell{
    using point = geometry::point<double>;
    using polygon = geometry::polygon<point>;
    using multipolygon = geometry::multi_polygon<polygon>;

    multipolygon m_geometry;
    point m_position;
    bool m_fixed;
public:
    PlacementCell(const std::string & name, const bool fixed, std::size_t pins_size):Cell(name, pins_size), m_fixed(m_fixed){}
    void set_geometry(multipolygon geometry){m_geometry = geometry;}
    void set_position(const point & position){m_position = position;}
    const point & get_position(){return m_position;}
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
    floorplan::floorplan m_floorplan;

    placement::def2placement(*def, m_placement);
    placement::lef2library(*lef, m_placement_lib);
    floorplan::lefdef2floorplan(*lef, *def, m_floorplan);

    netlist::netlist netlist = m_placement.netlist();

    geometry::point<double> m_chip_boundaries = m_floorplan.chip_boundaries();

    std::vector<PlacementCell> m_cells;
    m_cells.reserve(netlist.cell_system().size());

    for(auto cell : netlist.cell_system()){
        PlacementCell p_cell(netlist.cell_name(cell), m_placement.cell_fixed(cell), netlist.cell_pins(cell).size());
        p_cell.set_geometry(m_placement.cell_geometry(cell));
        p_cell.set_position(m_placement.cell_position(cell));
        m_cells.push_back(p_cell);
    }

    auto time_start = std::chrono::high_resolution_clock::now();
    bool placemente_is_legal = true;
    for(auto cell : m_cells){
        auto position = cell.get_position();
        //std::cout<<position.x()<<" "<<position.y()<<std::endl;
        if(position.x() < 0 || position.y() < 0 || position.x() > m_chip_boundaries.x() || position.y() > m_chip_boundaries.y()){
            placemente_is_legal = false;
            break;
        }
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto total_time = time_end - time_start;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count()<<" ms"<<std::endl;

//    if(placemente_is_legal)
//        std::cout<<"The placement is legal."<<std::endl;
//    else
//        std::cout<<"The placement is Ilegal."<<std::endl;
    return 0;
}

