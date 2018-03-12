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
        template <class Geometry>
        Geometry translate(const Geometry & geometry, Point & translationPoint)
        {
            Geometry result;

            boost::geometry::strategy::transform::translate_transformer <double, 2, 2> translate(
                translationPoint.x(),
                translationPoint.y());
            boost::geometry::transform(geometry, result, translate);

            return result;
        }

//! Scaling operation

/*!
 * \brief Scales a geometry by a given point
 * \param geometry Geometry to be scaled
 * \param scalingPoint Point representing the scaling to be applied to geometry
 * \param result Geometry representing the scaled geometry
 */
        template <class Geometry>
        Geometry scale(const Geometry & geometry, Point & scalingPoint)
        {
            Geometry result;

            boost::geometry::strategy::transform::scale_transformer <double, 2, 2> scale(
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
        template <class Geometry>
        void rotate(const Geometry & geometry, double degree, Geometry & result)
        {
            boost::geometry::strategy::transform::rotate_transformer <boost::geometry::degree,
                double, 2, 2> rotate(degree);
            boost::geometry::transform(geometry, result, rotate);
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
    }     // namespace geometry
}     // namespace ophidian

#endif // OPHIDIAN_GEOMETRY_OPERATIONS_H