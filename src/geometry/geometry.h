//
// Created by renan on 1/14/16.
//

#ifndef OPENEDA_GEOMETRY_POINT_H
#define OPENEDA_GEOMETRY_POINT_H

#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/algorithms/equals.hpp>

#include <boost/geometry/io/wkt/wkt.hpp>

#include <cmath>

namespace ophidian {
namespace geometry {
template<class Coordinate_Type>
using point = typename boost::geometry::model::d2::point_xy<Coordinate_Type>;
template<class Point_Type>
using polygon = typename boost::geometry::model::polygon<Point_Type>;
template<class Polygon_Type>
using multi_polygon = typename boost::geometry::model::multi_polygon<Polygon_Type>;

template<class Geometry, class CoordinateType>
void translate(Geometry geometry, point<CoordinateType> point,
		Geometry & result) {
	boost::geometry::strategy::transform::translate_transformer<CoordinateType,
			2, 2> translate(point.x(), point.y());
	boost::geometry::transform(geometry, result, translate);
//	boost::geometry::correct(result);
}

template<class CoordinateType>
CoordinateType manhattan_distance(const point<CoordinateType> & p1, const point<CoordinateType> & p2)
{
	return std::abs(p2.y()-p1.y())+std::abs(p2.x()-p1.x());
}


using boost::geometry::equals;
using boost::geometry::wkt;

}
}

#endif //OPENEDA_POINT_H
