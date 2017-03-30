#ifndef ophidian_PROBLEM_B_H
#define ophidian_PROBLEM_B_H

#include "../src/geometry/geometry.h"
#include "../src/interconnection/hpwl.h"
#include "../src/interconnection/stwl.h"

using namespace ophidian;

namespace problem_b{
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

void problem_b_ood(placement::placement & m_placement, Metric &metric){
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

    metric.start();
    for(auto net : m_nets){
      std::vector<geometry::point<double>> pin_positions;
      for(auto pin : net.pins())
        pin_positions.push_back(static_cast<PinPlacement*>(pin)->pin_position());
      interconnection::hpwl(pin_positions);
      interconnection::stwl(pin_positions);
    }
    metric.end();
    metric.print_result();
}

void problem_b_dod(placement::placement & m_placement, Metric &metric){
    netlist::netlist netlist = m_placement.netlist();
    m_placement.set_all_pin_positions();

    metric.start();
    for(auto net : netlist.net_system()){
      std::vector<geometry::point<double>> pin_positions;
      for(auto pin : netlist.net_pins(net))
        pin_positions.push_back(m_placement.get_pin_position(pin));
      interconnection::hpwl(pin_positions);
      interconnection::stwl(pin_positions);
    }
    metric.end();
    metric.print_result();
}
}

#endif //ophidian_PROBLEM_B_H
