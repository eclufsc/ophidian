#ifndef OPHIDIAN_GEOMETRY_MODELS_H
#define OPHIDIAN_GEOMETRY_MODELS_H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

namespace ophidian {
namespace geometry {

using Point = boost::geometry::model::d2::point_xy<double>;
using Segment = boost::geometry::model::segment<Point>;
using Linestring = boost::geometry::model::linestring<Point>;
using Box = boost::geometry::model::box<Point>;
using Polygon = boost::geometry::model::polygon<Point>;
using MultiPolygon = boost::geometry::model::multi_polygon<Polygon>;

template<class Geometry>
Geometry make(const std::vector<Point> & points) {
    Geometry geometry;
    for (auto point : points) {
        boost::geometry::append(geometry, point);
    }
    return geometry;
}

template<class MultiGeometry, class PartGeometry>
MultiGeometry makeMulti(const std::vector<PartGeometry> & parts) {
    MultiGeometry multiGeometry;
    for (auto part : parts) {
        multiGeometry.push_back(part);
    }
    return multiGeometry;
}

}
}

#endif // OPHIDIAN_GEOMETRY_MODELS_H
