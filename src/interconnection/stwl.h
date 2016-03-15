//
// Created by renan on 3/10/16.
//

#ifndef OPENEDA_STWL_ESTIMATION_H
#define OPENEDA_STWL_ESTIMATION_H

#include "../geometry/geometry.h"

namespace openeda {
namespace interconnection {
double stwl(const std::vector<geometry::point<double> >&points);
}
}


#endif //OPENEDA_STWL_ESTIMATION_H
