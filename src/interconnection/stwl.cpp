//
// Created by renan on 3/10/16.
//

#include "stwl.h"
#include "hpwl.h"
#include "flute.h"

namespace openeda {
    namespace interconnection {
        double stwl(const std::vector<geometry::point<double> >&points) {
            if(points.size() <= 3)
                return hpwl(points);
            std::vector<unsigned> X(points.size());
            std::vector<unsigned> Y(points.size());
            X.resize(0); Y.resize(0);
            for (auto & p : points) {
                X.push_back(p.x());
                Y.push_back(p.y());
            }
            auto tree = flute(points.size(), X.data(), Y.data(), ACCURACY);
            delete tree.branch;
            return tree.length;
        }
    }
}
