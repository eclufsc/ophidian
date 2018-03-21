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

    template <class Geometry, typename T>
    Geometry translate(const Geometry & geometry, Point<T> & translationPoint);

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

    //!Class multibox using geometry::Box
    template <typename T>
    class MultiBox
    {
    public:

        //!Standar Constructors
        MultiBox() = default;
        MultiBox(const MultiBox<T> &) = default;
        MultiBox(MultiBox<T> &&) = default;
        MultiBox<T>& operator= ( const MultiBox<T> & ) = default;
        MultiBox<T>& operator= ( MultiBox<T> && ) = default;

        //!Constructor receiving a vector of geometry::Box
        MultiBox(const std::vector<geometry::Box<T>> & boxes):
                mBoxes(boxes)
        {
        }

        //!Push back a geometry::Box
        void push_back(const Box<T> & box)
        {
            mBoxes.push_back(box);
        }

        //!Non-const iterator begin
        typename
        std::vector<Box<T>>::iterator begin()
        {
            return mBoxes.begin();
        }

        //!Non-const iterator end
        typename
        std::vector<Box<T>>::iterator end()
        {
            return mBoxes.end();
        }

        //!Const iterator begin
        typename
        std::vector<Box<T>>::const_iterator begin() const
        {
            return mBoxes.begin();
        }

        //!Const iterator end
        typename
        std::vector<Box<T>>::const_iterator end() const
        {
            return mBoxes.end();
        }

        //!Operator overloading for comparison of two multibox objects
        bool operator==(const MultiBox & other) const
        {
            for(auto box1 : this->mBoxes)
            {
                for(auto box2 : other.mBoxes)
                {
                    bool comparison = (box1.min_corner().x() == box2.min_corner().x()) &&
                                      (box1.min_corner().y() == box2.min_corner().y()) &&
                                      (box1.max_corner().x() == box2.max_corner().x()) &&
                                      (box1.max_corner().y() == box2.max_corner().y());
                    if(!comparison) {
                        return false;
                    }
                }
            }

            return true;
        }

        //!Operator overload for difference between two multibox objects
        bool operator!=(const MultiBox<T> & other) const
        {
            return !(*this == other);
        }

        MultiBox<T> translate(Point<T> translationPoint)
        {
            std::vector<Box<T>> translatedBoxes;
            translatedBoxes.reserve(mBoxes.size());
            for(auto box : mBoxes)
            {
                Box<T> translatedBox = geometry::translate(box, translationPoint);
                translatedBoxes.push_back(translatedBox);
            }

            return MultiBox{translatedBoxes};
        }

    private:
        std::vector<Box<T>> mBoxes;
    };
}     //namespace geometry
}     //namespace ophidian

#endif // OPHIDIAN_GEOMETRY_MODELS_H
