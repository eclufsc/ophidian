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

//! Create new geometry
/*!
 *  \brief Creates a new geometry from a vector of points. Can be used for Linestring and Polygon. Should not be used for Segment and Box.
 * \param points Vector of points of the geometry
 * \return The created geometry
 */
template<class Geometry>
Geometry make(const std::vector<Point> & points) {
    Geometry geometry;
    for (auto point : points) {
        boost::geometry::append(geometry, point);
    }
    return geometry;
}

//! Create new multi geometry
/*!
 *  \brief Creates a new multi geometry from a vector of points. Can be used for MultiPolygon, but could be extended for other multi geometries, such as a MultiBox.
 * \param parts The geometries of the multi geometry
 * \return The created multi geometry
 */
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
