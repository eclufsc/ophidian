//
// Created by renan on 3/10/16.
//

#include "hpwl_estimation.h"

namespace openeda {
    namespace interconnection_estimation {

        hpwl_estimation::hpwl_estimation(netlist::netlist & netlist, placement::placement &placement)
            : interconnection_estimation(netlist, placement) {

        }

        hpwl_estimation::~hpwl_estimation() {

        }

        double hpwl_estimation::estimate(entity::entity net) {
            auto net_pins = m_netlist.net_pins(net);
            std::vector<double> x_positions(net_pins.size());
            std::vector<double> y_positions(net_pins.size());
            x_positions.resize(0);
            y_positions.resize(0);
            for(auto pin : net_pins)
            {
                auto pos = m_placement.pin_position(pin);
                x_positions.push_back(pos.x());
                y_positions.push_back(pos.y());
            }

            point lower{*std::min_element(x_positions.begin(), x_positions.end()), *std::min_element(y_positions.begin(), y_positions.end())};
            point upper{*std::max_element(x_positions.begin(), x_positions.end()), *std::max_element(y_positions.begin(), y_positions.end())};
            double hpwl = openeda::geometry::manhattan_distance(lower, upper);
            return hpwl;
        }
    }
}
