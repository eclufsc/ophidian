//
// Created by renan on 3/10/16.
//

#include "hpwl.h"

namespace openeda {
    namespace interconnection {
        double hpwl(const std::vector<geometry::point<double> >& points) {
            geometry::point<double> lower( std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
            geometry::point<double> upper( std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() );
            for(auto & p : points)
            {
                lower.x(std::min(lower.x(), p.x()));
                lower.y(std::min(lower.y(), p.y()));
                upper.x(std::max(upper.x(), p.x()));
                upper.y(std::max(upper.y(), p.y()));
            }
            return openeda::geometry::manhattan_distance(lower, upper);
        }
    }
}
