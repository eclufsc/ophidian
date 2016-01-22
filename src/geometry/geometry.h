//
// Created by renan on 1/14/16.
//

#ifndef OPENEDA_GEOMETRY_POINT_H
#define OPENEDA_GEOMETRY_POINT_H

#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

namespace openeda {
    namespace geometry {
        template <class Coordinate_Type>
        using point = typename boost::geometry::model::d2::point_xy<Coordinate_Type>;
        template <class Point_Type>
        using polygon = typename boost::geometry::model::polygon<Point_Type>;
        template <class Polygon_Type>
        using multi_polygon = typename boost::geometry::model::multi_polygon<Polygon_Type>;
    }
}

#endif //OPENEDA_POINT_H
