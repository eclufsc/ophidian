//
// Created by renan on 3/10/16.
//

#include "stwl_estimation.h"

namespace openeda {
    namespace interconnection_estimation {

        double stwl_estimation::estimate(entity::entity net) {
            auto pins = m_netlist.net_pins(net);
            unsigned numpins = pins.size();
            unsigned *x = new unsigned[numpins];
            unsigned *y = new unsigned[numpins];

            for (size_t pin_id = 0; pin_id < pins.size(); pin_id++) {
                auto pin = pins.at(pin_id);
                auto pin_position = m_placement.pin_position(pin);
                x[pin_id] = (unsigned)(std::max(pin_position.x(), 0.0));
                y[pin_id] = (unsigned)(std::max(pin_position.y(), 0.0));
            }

            auto tree = flute(numpins, x, y, ACCURACY);

            return tree.length;
        }
    }
}
