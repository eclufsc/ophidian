#ifndef OPHIDIAN_GEOMETRY_OPERATIONS_H
#define OPHIDIAN_GEOMETRY_OPERATIONS_H

#include <boost/geometry/algorithms/intersection.hpp>

#include "Models.h"

namespace ophidian {
namespace geometry {

template<class Geometry>
void translate(const Geometry & geometry, const Point & translationPoint, Geometry & result) {
	boost::geometry::strategy::transform::translate_transformer<double, 2, 2> translate(translationPoint.x(), translationPoint.y());
	boost::geometry::transform(geometry, result, translate);
}

template<class Geometry>
void scale(const Geometry & geometry, const Point & scalePoint, Geometry & result) {
    boost::geometry::strategy::transform::scale_transformer<double, 2, 2> scale(scalePoint.x(), scalePoint.y());
    boost::geometry::transform(geometry, result, scale);
}

template<class Geometry>
void rotate(const Geometry & geometry, double degree, Geometry & result) {
	boost::geometry::strategy::transform::rotate_transformer<boost::geometry::degree, double, 2, 2> rotate(degree);
	boost::geometry::transform(geometry, result, rotate);
}

template<class Geometry1, class Geometry2, class GeometryOut>
void intersection(const Geometry1 & geometry1, const Geometry2 & geometry2, GeometryOut & result) {
    boost::geometry::intersection(geometry1, geometry2, result);
}

}
}

#endif // OPHIDIAN_GEOMETRY_OPERATIONS_H
