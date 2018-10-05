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

#ifndef OPHIDIAN_GEOMETRY_OPERATIONS_H
#define OPHIDIAN_GEOMETRY_OPERATIONS_H

#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

namespace ophidian::geometry
{
    template <template <typename> class Geometry, template <typename> class Point_, class T>
    Geometry<Point_<T>> translate(const Geometry<Point_<T>> & geometry, Point_<T> & translationPoint)
    {
        namespace bg = boost::geometry;

        auto result = Geometry<Point_<T>>{};

        auto translate = bg::strategy::transform::translate_transformer<T, 2, 2>{translationPoint.x(), translationPoint.y()};
        bg::transform(geometry, result, translate);

        return result;
    }

    template <template <typename> class Point_, class T>
    Point_<T> translate(const Point_<T> & geometry, Point_<T> & translationPoint)
    {
        namespace bg = boost::geometry;

        auto result = Point_<T>{};

        auto translate = bg::strategy::transform::translate_transformer<T, 2, 2>{translationPoint.x(), translationPoint.y()};
        bg::transform(geometry, result, translate);

        return result;
    }

    //! Scaling operation

    /*!
     * \brief Scales a geometry by a given point
     * \param geometry Geometry to be scaled
     * \param scalingPoint Point representing the scaling to be applied to geometry
     * \param result Geometry representing the scaled geometry
     */
    template <template <typename> class Geometry, template <typename> class Point_, class T>
    Geometry<Point_<T>> scale(const Geometry<Point_<T>> & geometry, Point_<T> & scalingPoint)
    {
        Geometry<Point_<T>> result;

        boost::geometry::strategy::transform::scale_transformer<T, 2, 2> scale(
            scalingPoint.x(),
            scalingPoint.y());
        boost::geometry::transform(geometry, result, scale);

        return result;
    }

    template <template <typename> class Point_, class T>
    Point_<T> scale(const Point_<T> & geometry, Point_<T> & scalingPoint)
    {
        Point_<T> result;

        boost::geometry::strategy::transform::scale_transformer<T, 2, 2> scale(
            scalingPoint.x(),
            scalingPoint.y());
        boost::geometry::transform(geometry, result, scale);

        return result;
    }

    //! Rotate operation

    /*!
     * \brief Rotates a geometry by a given amount of degrees
     * \param geometry Geometry to be rotated
     * \param degree Amount of rotation degrees
     * \param result Geometry representing the rotated geometry
     */
    template <template <typename> class Geometry, template <typename> class Point_, class T>
    Geometry<Point_<T>> rotate(const Geometry<Point_<T>> & geometry, double degree)
    {
        Geometry<Point_<T>> result;
        boost::geometry::strategy::transform::rotate_transformer<boost::geometry::degree, double, 2,
            2> rotate(degree);
        boost::geometry::transform(geometry, result, rotate);
        return result;
    }

    template <template <typename> class Point_, class T>
    Point_<T> rotate(const Point_<T> & geometry, double degree)
    {
        Point_<T> result;
        boost::geometry::strategy::transform::rotate_transformer<boost::geometry::degree, double, 2,
            2> rotate(degree);
        boost::geometry::transform(geometry, result, rotate);
        return result;
    }

    //! Intersection operation

    /*!
     * \brief Calculates the intersection between two geometries
     * \param geometry1 First geometry to calculate the intersection
     * \param geometry2 Second geometry to calculate the intersection
     * \param result Geometry representing the intersection of geometry1 and geometry2. Should be a collection of geometries, a multi geometry or a Box in the case of Box intersection
     */
    template <class Geometry1, class Geometry2, class GeometryOut>
    void intersection(
        const Geometry1 & geometry1,
        const Geometry2 & geometry2,
        GeometryOut & result)
    {
        boost::geometry::intersection(geometry1, geometry2, result);
    }
}

#endif // OPHIDIAN_GEOMETRY_OPERATIONS_H
