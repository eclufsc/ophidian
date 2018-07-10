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

#include "Models.h"
#include "MultiBox.h"


namespace ophidian
{
namespace geometry
{
    //! Translation operation

    /*!
     * \brief Translates a geometry by a given point
     * \param geometry Geometry to be translated
     * \param translationPoint Point representing the translation to be applied to geometry
     * \param result Geometry representing the translated geometry
     */
    template <template <typename> class Geometry, template <typename> class Point_, class T>
    Geometry<Point_<T>> translate(const Geometry<Point_<T>> & geometry, Point_<T> & translationPoint)
    {
        namespace bg = boost::geometry;

        auto result = Geometry<Point_<T>>{};

        auto translate = bg::strategy::transform::translate_transformer<T, 2, 2>{translationPoint.x(), translationPoint.y()};
        bg::transform(geometry, result, translate);

        return result;
    }

    template <typename T>
    MultiBox<T> translate(const MultiBox<T> & geometry, Point<T> translation_point)
    {
        auto translated_boxes = std::vector<Box<T>>{};

        translated_boxes.reserve(geometry.size());

        for(const auto & box : geometry)
        {
            auto translated_box = translate(box, translation_point);

            translated_boxes.push_back(std::move(translated_box));
        }

        return MultiBox<T>{std::move(translated_boxes)};
    }

    //! Scaling operation

    /*!
     * \brief Scales a geometry by a given point
     * \param geometry Geometry to be scaled
     * \param scalingPoint Point representing the scaling to be applied to geometry
     * \param result Geometry representing the scaled geometry
     */
    // template <class Geometry, class T>
    // Geometry scale(const Geometry & geometry, Point<T> & scalingPoint)
    // {
    //     Geometry result;
    //
    //     boost::geometry::strategy::transform::scale_transformer<T, 2, 2> scale(
    //         scalingPoint.x(),
    //         scalingPoint.y());
    //     boost::geometry::transform(geometry, result, scale);
    //
    //     return result;
    // }

    //! Rotate operation

    /*!
     * \brief Rotates a geometry by a given amount of degrees
     * \param geometry Geometry to be rotated
     * \param degree Amount of rotation degrees
     * \param result Geometry representing the rotated geometry
     */
    // template <class Geometry>
    // void rotate(const Geometry & geometry, double degree, Geometry & result)
    // {
    //     boost::geometry::strategy::transform::rotate_transformer<boost::geometry::degree, double, 2,
    //         2> rotate(degree);
    //     boost::geometry::transform(geometry, result, rotate);
    // }

    //! Intersection operation

    /*!
     * \brief Calculates the intersection between two geometries
     * \param geometry1 First geometry to calculate the intersection
     * \param geometry2 Second geometry to calculate the intersection
     * \param result Geometry representing the intersection of geometry1 and geometry2. Should be a collection of geometries, a multi geometry or a Box in the case of Box intersection
     */
    // template <class Geometry1, class Geometry2, class GeometryOut>
    // void intersection(
    //     const Geometry1 & geometry1,
    //     const Geometry2 & geometry2,
    //     GeometryOut & result)
    // {
    //     boost::geometry::intersection(geometry1, geometry2, result);
    // }
}     // namespace geometry
}     // namespace ophidian

#endif // OPHIDIAN_GEOMETRY_OPERATIONS_H
