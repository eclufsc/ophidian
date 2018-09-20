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

#ifndef OPHIDIAN_INTERCONNECTION_STEINERTREE_H
#define OPHIDIAN_INTERCONNECTION_STEINERTREE_H

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>
#include <lemon/smart_graph.h>
#include <lemon/maps.h>
#include <lemon/dim2.h>
#include <cstdint>
#include <memory>
#include <iterator>

namespace ophidian::interconnection
{
    //! Rectilinear Steiner Tree

    /*!
       A graph G(V,E), where V is the set of Steiner Points and E is the set of segments.
       One can iterate over its Steiner Points and Segments.
       Also, one can query the length of a Segment as well as the total length of the Steiner Tree.
     */
    class SteinerTree
    {
    public:
        using GraphType = lemon::SmartGraph;

        using dbu_t = util::database_unit_t;
        using DbuPoint = geometry::Point<dbu_t>;
        using segment_type = geometry::Segment<dbu_t>;

        //! Lemon Graph Element Wrapper

        /*!
           Helper class to wrap a lemon graph element.
         */
        template <class T>
        class GraphElementWrapper
        {
        public:

            inline GraphElementWrapper()
            {
            }

            inline virtual ~GraphElementWrapper()
            {
            }

            inline bool operator==(const GraphElementWrapper<T> & o) const
            {
                return mEl == o.mEl;
            }

        protected:

            inline GraphElementWrapper(T element):
                    mEl(element)
            {
            }

            T mEl;
        };

        //! Steiner Tree Attorney

        /*!
           Helper class to break the visibility of some Steiner Tree members. Be careful.
         */
        class Attorney
        {
        public:

            inline virtual ~Attorney()
            {
            }

        protected:

            //! Getter for the graph object

            /*!
               Returns a const reference to the Steiner Tree's graph object.
             */
            inline static const SteinerTree::GraphType & graph(const SteinerTree & tree)
            {
                return tree.mGraph;
            }

            //! Getter for the position property

            /*!
               Returns a const reference to the Steiner Tree's position map.
             */
            inline static const GraphType::NodeMap<lemon::dim2::Point<double>> & position(
                const SteinerTree & tree)
            {
                return tree.mPosition;
            }

            inline Attorney()
            {
            }

        private:

            Attorney(const Attorney & o) = delete;

            Attorney & operator=(const Attorney & o) = delete;
        };

        friend class Attorney;

        //! Steiner Point

        /*!
           A handler for a Steiner Point.
         */
        class Point final :
            public GraphElementWrapper<GraphType::Node>
        {
        public:
            friend class SteinerTree;
            using Parent = GraphElementWrapper<GraphType::Node>;

        private:
            using Parent::Parent;
        };

        class PointSegmentsIterator;

        //! Segment

        /*!
           A handler for a Segment.
         */
        class Segment final :
            public GraphElementWrapper<GraphType::Edge>
        {
        public:
            friend class SteinerTree;
            friend class PointSegmentsIterator;
            using Parent = GraphElementWrapper<GraphType::Edge>;

        private:
            using Parent::Parent;
        };

        //! Point Iterator

        /*!
           The iterator for the Steiner Points.
         */
        class PointIterator
        {
        public:
            friend class SteinerTree;

            using difference_type = uint32_t;
            using value_type = Point;
            using pointer = const Point *;
            using reference = Point &;
            using iterator_category = std::forward_iterator_tag;

            inline PointIterator & operator++()
            {
                GraphType::next(mPoint.mEl);

                return *this;
            }

            inline bool operator==(const PointIterator & o) const
            {
                return mPoint.mEl == o.mPoint.mEl;
            }

            inline bool operator!=(const PointIterator & o) const
            {
                return !((*this) == o);
            }

            inline const Point & operator*() const
            {
                return mPoint;
            }

        private:

            PointIterator(const Point & p);

            Point mPoint;
        };

        //! Point Segments Iterator

        /*!
           The iterator for iterating over the Steiner Segments connected to a given Steiner Point.
         */
        class PointSegmentsIterator
        {
        public:
            friend class SteinerTree;

            using difference_type = uint32_t;
            using value_type = Segment;
            using pointer = const Segment *;
            using reference = Segment &;
            using iterator_category = std::forward_iterator_tag;

            inline const PointSegmentsIterator & operator++()
            {
                ++mIt;

                return *this;
            }

            inline bool operator==(const PointSegmentsIterator & o) const
            {
                return mIt == o.mIt;
            }

            inline bool operator!=(const PointSegmentsIterator & o) const
            {
                return !((*this) == o);
            }

            inline Segment operator*() const
            {
                return Segment(static_cast<GraphType::Edge>(mIt));
            }

        private:

            PointSegmentsIterator(GraphType::IncEdgeIt it);

            GraphType::IncEdgeIt mIt;
        };

        //! Segment Iterator

        /*!
           The iterator for the Segments.
         */
        class SegmentIterator
        {
        public:
            friend class SteinerTree;

            using difference_type = uint32_t;
            using value_type = Segment;
            using pointer = const Segment *;
            using reference = Segment &;
            using iterator_category = std::forward_iterator_tag;

            inline const SegmentIterator & operator++()
            {
                ++mIt;

                return *this;
            }

            inline bool operator==(const SegmentIterator & o) const
            {
                return mIt == o.mIt;
            }

            inline bool operator!=(const SegmentIterator & o) const
            {
                return !((*this) == o);
            }

            inline Segment operator*() const
            {
                return Segment(static_cast<GraphType::Edge>(mIt));
            }

        private:

            SegmentIterator(GraphType::EdgeIt it);

            GraphType::EdgeIt mIt;
        };

        //! Create Steiner Tree

        /*!
           \brief Forces the creation of a Steiner Tree on the Heap and returns an std::unique_ptr<>
           . This way, a Steiner Tree can't be copied, since the copy of a graph is not a trivial operation.
         */
        static std::unique_ptr<SteinerTree> create();

        //! Add Steiner Point

        /*!
           \brief Creates a Steiner point given a position.
           \param position The position.
           \return A handler for the created Steiner Point.
         */
        Point add(const DbuPoint & position);

        //! Add Segment

        /*!
           \brief Creates a Segment between two Steiner Points.
           \param p1 The first Steiner Point.
           \param p2 The second Steiner Point.
           \return A handler for the created Segment.
         */
        Segment add(const Point & p1, const Point & p2);

        //! Number of Segments

        /*!
           \return The number of segments.
         */
        uint32_t size(Segment) const;

        //! Number of Steiner Points

        /*!
           \return The number of Steiner Points.
         */
        uint32_t size(Point) const;

        //! Steiner Point of a Segment.

        /*!
           \param segment The handler for a Segment.
           \return One of the Steiner Points of \p segment.
         */
        Point u(const Segment & segment) const;

        //! Steiner Point of a Segment.

        /*!
           \param segment The handler for a Segment.
           \return One of the Steiner Points of \p segment.
           \remarks This returns the opposite Steiner point of u(\p segment).
         */
        Point v(const Segment & segment) const;

        //! Position of a Steiner Point

        /*!
           \param p The handler for a Steiner Point.
           \return The position of \p.
         */
        DbuPoint position(const Point & p) const;

        //! Length of a Segment.

        /*!
           \param segment The handler for a Segment.
           \return The rectilinear length of \p segment.
         */
        dbu_t length(const Segment & segment) const;

        //! Length of the Steiner Tree.

        /*!
           \return The total length of the Steiner Tree.
         */
        dbu_t length() const;

        //! Steiner Points

        /*!
           \return Pair of Steiner Point iterators pointing to begin and end.
         */
        std::pair<PointIterator, PointIterator> points() const;

        //! Segments of a Steiner Point

        /*!
           \param point The handler for a Steiner Point.
           \return Pair of Steiner Point Segments Iterators.
         */
        std::pair<PointSegmentsIterator, PointSegmentsIterator> segments(const Point & point) const;

        //! Segments

        /*!
           \return Pair of Segments Iterators.
         */
        std::pair<SegmentIterator, SegmentIterator> segments() const;

    private:

        //! Default Constructor

        /*!
           \brief Constructs an empty Steiner Tree.
         */
        SteinerTree() = default;

        GraphType                                      mGraph{};
        GraphType::NodeMap<lemon::dim2::Point<double>> mPosition{mGraph};
        dbu_t                                          mLength{0.0};
    };

    geometry::Segment<geometry::Point<SteinerTree::dbu_t>> make_segment(
        const interconnection::SteinerTree & tree,
        const interconnection::SteinerTree::Segment & segment);
}

#endif // OPHIDIAN_INTERCONNECTION_STEINERTREE_H
