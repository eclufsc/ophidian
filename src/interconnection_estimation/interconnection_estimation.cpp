//
// Created by renan on 3/10/16.
//

#include "interconnection_estimation.h"

namespace openeda {
    namespace interconnection_estimation {

        interconnection_estimation::interconnection_estimation(netlist::netlist & netlist, placement::placement &placement)
                : m_netlist(netlist), m_placement(placement) {

        }

        interconnection_estimation::~interconnection_estimation() {

        }
    }
}
