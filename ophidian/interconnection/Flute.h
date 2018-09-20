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

#ifndef OPHIDIAN_INTERCONNECTION_FLUTE_H
#define OPHIDIAN_INTERCONNECTION_FLUTE_H

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>
#include <vector>
#include <memory>

namespace ophidian::interconnection
{
    class SteinerTree;

    //! FLUTE: Fast Lookup Table Based Technique for RSMT Construction and Wirelength Estimation

    /*!
       From: http://home.eng.iastate.edu/~cnchu/flute.html
     */
    class Flute final
    {
    public:
        using dbu_t = util::database_unit_t;
        using Point = geometry::Point<dbu_t>;

        //! Singleton instantiation
        static Flute & instance();

        ~Flute();

        //! Create Steiner Tree

        /*!
            \brief Creates a Steiner Tree from a container of geometry::Point using FLUTE algorithm.
            \param container A container of geometry::Point.
            \return A std::unique_ptr to the created Steiner Tree.
         */
        template <template <typename> class Container_type>
        inline std::unique_ptr<SteinerTree> create(const Container_type<Point> & container)
        {
            const uint32_t kSize = container.size();

            if(kSize == 1) {
                return trivialSteinerTree(*std::begin(container));
            }
            else if(kSize == 2) {
                typename Container_type<Point>::const_iterator it(container.begin());
                typename Container_type<Point>::const_iterator it2(it);
                ++it2;

                return singleSegment(*it, *it2);
            }
            std::vector<unsigned> X(kSize);
            std::vector<unsigned> Y(kSize);
            X.resize(0);
            Y.resize(0);

            auto offset = Point{dbu_t{0.0}, dbu_t{0.0}};

            for(const auto & point : container)
            {
                offset.x(std::min(offset.x(), point.x()));
                offset.y(std::min(offset.y(), point.y()));
            }

            offset.x(-offset.x());
            offset.y(-offset.y());

            for(const auto & point : container)
            {
                X.push_back(static_cast<unsigned>(std::round(units::unit_cast<double>(point.x()) + units::unit_cast<double>(offset.x()))));
                Y.push_back(static_cast<unsigned>(std::round(units::unit_cast<double>(point.y()) + units::unit_cast<double>(offset.y()))));
            }

            return callFlute(X, Y, offset);
        }

    private:
        std::unique_ptr<SteinerTree> singleSegment(
            const Point & p1,
            const Point & p2);

        std::unique_ptr<SteinerTree> trivialSteinerTree(const Point & p);

        std::unique_ptr<SteinerTree> callFlute(
            const std::vector<unsigned> & X,
            const std::vector<unsigned> & Y,
            const Point & offset);

        Flute();

        Flute(const Flute & o) = delete;

        Flute & operator=(const Flute & o) = delete;
    };
}

#endif // OPHIDIAN_INTERCONNECTION_FLUTE_H
