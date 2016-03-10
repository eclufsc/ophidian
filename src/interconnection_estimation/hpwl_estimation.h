//
// Created by renan on 3/10/16.
//

#ifndef OPENEDA_HPWL_ESTIMATION_H
#define OPENEDA_HPWL_ESTIMATION_H

#include "interconnection_estimation.h"

namespace openeda {
namespace interconnection_estimation {
    class hpwl_estimation : public interconnection_estimation {
    public:
        hpwl_estimation(netlist::netlist & netlist, placement::placement & placement);
        ~hpwl_estimation();

        double estimate(entity::entity net);
    };
}
}


#endif //OPENEDA_HPWL_ESTIMATION_H
