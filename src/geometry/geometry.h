/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#ifndef OPHIDIAN_SRC_GEOMETRY_H
#define OPHIDIAN_SRC_GEOMETRY_H

#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/algorithms/equals.hpp>

#include <boost/geometry/io/wkt/wkt.hpp>

#include <limits>
#include <cmath>

namespace ophidian {
namespace geometry {
template<class Coordinate_Type>
using point = typename boost::geometry::model::d2::point_xy<Coordinate_Type>;
template<class Point_Type>
using polygon = typename boost::geometry::model::polygon<Point_Type>;
template<class Polygon_Type>
using multi_polygon = typename boost::geometry::model::multi_polygon<Polygon_Type>;
template<class Point_Type>
using box = typename boost::geometry::model::box<Point_Type>;

template<class Geometry, class CoordinateType>
void translate(Geometry geometry, point<CoordinateType> point,
		Geometry & result) {
	boost::geometry::strategy::transform::translate_transformer<CoordinateType,
			2, 2> translate(point.x(), point.y());
	boost::geometry::transform(geometry, result, translate);
//	boost::geometry::correct(result);
}

template<class Geometry, class CoordinateType>
void scale(Geometry geometry, point<CoordinateType> point,
                Geometry & result) {
        boost::geometry::strategy::transform::scale_transformer<CoordinateType,
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

using boost::geometry::correct;
using boost::geometry::append;
using boost::geometry::envelope;


}
}

#endif //OPHIDIAN_SRC_GEOMETRY_H
