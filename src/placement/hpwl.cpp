#include "hpwl.h"

#include "placement.h"
#include "../interconnection/hpwl.h"

namespace ophidian {
namespace placement {

hpwl::hpwl(const placement & place) {

    m_value = 0;
    for(auto net_pair : place.netlist().net_system())
    {
        hpwl net_hpwl(place, net_pair.first);
        m_value += net_hpwl.value();
    }
}

hpwl::hpwl(const placement & place, const entity::entity & net) {
    auto net_pins = place.netlist().net_pins(net);
    std::vector<geometry::point<double> > pin_positions(net_pins.size());
    pin_positions.resize(0);
    for(auto pin : net_pins)
        pin_positions.push_back(place.pin_position(pin));
    m_value = interconnection::hpwl(pin_positions);
}


}
}
