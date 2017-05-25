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
	for (auto point : points)
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
template<class MultiGeometry, class PartGeometry>
MultiGeometry makeMulti(const std::vector<PartGeometry> & parts) {
	MultiGeometry multiGeometry;
	for (auto part : parts)
	{
		multiGeometry.push_back(part);
	}
	return multiGeometry;
}

//!Class multibox using geometry::Box
class MultiBox
{
public:
	//!Standard constructor
	MultiBox() {

	}

	//!Constructor receiving a vector of geometry::Box
	MultiBox(const std::vector<geometry::Box> & boxes)
		: boxes_(boxes) {

	}

	//!Copy constructor
	MultiBox(const MultiBox & otherBox)
		: boxes_(otherBox.boxes_) {

	}

	//!Push back a geometry::Box
	void push_back(const geometry::Box & box) {
		boxes_.push_back(box);
	}

	//!Non-const iterator begin
	std::vector<geometry::Box>::iterator begin() {
		return boxes_.begin();
	}

	//!Non-const iterator end
	std::vector<geometry::Box>::iterator end() {
		return boxes_.end();
	}

	//!Const iterator begin
	std::vector<geometry::Box>::const_iterator begin() const {
		return boxes_.begin();
	}

	//!Const iterator end
	std::vector<geometry::Box>::const_iterator end() const {
		return boxes_.end();
	}

	//!Operator overloading for comparison of two multibox objects
	bool operator==(const MultiBox & other) const {
		for (auto box1 : this->boxes_)
		{
			for (auto box2 : other.boxes_)
			{
				bool comparison = (box1.min_corner().x() == box2.min_corner().x()) && (box1.min_corner().y() == box2.min_corner().y())
				                  && (box1.max_corner().x() == box2.max_corner().x()) && (box1.max_corner().y() == box2.max_corner().y());
				if (!comparison)
				{
					return false;
				}
			}
		}
		return true;
	}

	//!Operator overload for difference between two multibox objects
	bool operator!=(const MultiBox & other) const {
		return !(*this==other);
	}

private:
	std::vector<geometry::Box> boxes_;
};


} //namespace geometry
} //namespace ophidian

#endif // OPHIDIAN_GEOMETRY_MODELS_H
