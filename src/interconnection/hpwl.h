//
// Created by renan on 3/10/16.
//

#ifndef OPENEDA_HPWL_ESTIMATION_H
#define OPENEDA_HPWL_ESTIMATION_H

#include "../geometry/geometry.h"

namespace openeda {
namespace interconnection {
double hpwl(const std::vector<geometry::point<double> > & points);
}
}


#endif //OPENEDA_HPWL_ESTIMATION_H
