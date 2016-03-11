//
// Created by renan on 3/10/16.
//

#ifndef OPENEDA_STWL_ESTIMATION_H
#define OPENEDA_STWL_ESTIMATION_H

#include "interconnection_estimation.h"
#include "flute.h"

namespace openeda {
    namespace interconnection_estimation {
        class stwl_estimation : public interconnection_estimation {
        public:
            stwl_estimation(netlist::netlist &netlist, placement::placement &placement) : interconnection_estimation(
                    netlist, placement) {
                readLUT();
            }

            virtual ~stwl_estimation() { }


            virtual double estimate(entity::entity net);
        };
    }
}


#endif //OPENEDA_STWL_ESTIMATION_H
