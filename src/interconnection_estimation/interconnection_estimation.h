//
// Created by renan on 3/10/16.
//

#ifndef OPENEDA_INTERCONNECTION_ESTIMATION_H
#define OPENEDA_INTERCONNECTION_ESTIMATION_H

#include "netlist.h"
#include "placement.h"

namespace openeda {
    namespace interconnection_estimation {
        class interconnection_estimation {
        protected:
            using point = geometry::point<double>;

            netlist::netlist & m_netlist;
            placement::placement & m_placement;

        public:
            interconnection_estimation(netlist::netlist & netlist, placement::placement & placement);
            virtual ~interconnection_estimation();

            virtual double estimate(entity::entity net) = 0;
        };
    }
}


#endif //OPENEDA_INTERCONNECTION_ESTIMATION_H
