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

#ifndef OPHIDIAN_GEOMETRY_DISTANCE_H
#define OPHIDIAN_GEOMETRY_DISTANCE_H

#include "Models.h"

namespace ophidian
{
namespace geometry
{
    class ManhattanDistance
    {
    public:

        //! Construct ManhattanDistance

        /*!
         * \brief Empty default constructor
         */
        ManhattanDistance()
        {
        }

        //! Distance operator

        /*!
         * \brief Calculates the Manhattan distance between two points
         * \param point1 First point to calculate the distance
         * \param point2 Second point to calculate the distance
         * \return Manhattan distance between point1 and point2
         */
        double operator()(const Point<double> & point1, const Point<double> & point2);
    };

    class EuclideanDistance
    {
    public:

        //! Construct EuclideanDistance

        /*!
         * \brief Empty default constructor
         */
        EuclideanDistance()
        {
        }

        //! Distance operator

        /*!
         * \brief Calculates the Euclidean distance between two points
         * \param point1 First point to calculate the distance
         * \param point2 Second point to calculate the distance
         * \return Euclidean distance between point1 and point2
         */
        double operator()(const Point<double> & point1, const Point<double> & point2);
    };
}     // namespace geometry
}     // namespace ophidian

#endif // OPHIDIAN_GEOMETRY_DISTANCE_H