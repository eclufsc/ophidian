/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_GEOMETRY_MODELS_H
#define OPHIDIAN_GEOMETRY_MODELS_H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

namespace ophidian
{
namespace geometry
{
    template <typename T>
    using Point = boost::geometry::model::d2::point_xy<T>;

    template <typename T>
    using Box = boost::geometry::model::box<Point<T>>;

    template <typename T>
    using Segment = boost::geometry::model::segment<Point<T>>;

    template <typename T>
    using Linestring = boost::geometry::model::linestring<Point<T>>;

    template <typename T>
    using Polygon = boost::geometry::model::polygon<Point<T>>;

    template <typename T>
    using MultiPolygon = boost::geometry::model::multi_polygon<Polygon<T>>;

    //! Create new geometry

    /*!
     *  \brief Creates a new geometry from a vector of points. Can be used for Linestring and Polygon. Should not be used for Segment and Box.
     * \param points Vector of points of the geometry
     * \return The created geometry
     */
    template <template <typename> class Geometry, typename T>
    Geometry<T> make(const std::vector<Point<T>> & points)
    {
        Geometry<T> geometry;

        for(auto point : points)
        {
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
    template <class MultiGeometry, class PartGeometry>
    MultiGeometry makeMulti(const std::vector<PartGeometry> & parts)
    {
        MultiGeometry multiGeometry;

        for(auto part : parts)
        {
            multiGeometry.push_back(part);
        }

        return multiGeometry;
    }
}     //namespace geometry
}     //namespace ophidian

#endif // OPHIDIAN_GEOMETRY_MODELS_H
