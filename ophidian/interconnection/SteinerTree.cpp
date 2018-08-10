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

#include "SteinerTree.h"
#include <ophidian/geometry/Distance.h>
#include <numeric>
#include "ToEps.h"
#include <lemon/graph_to_eps.h>

namespace ophidian::interconnection
{
    using dbu_t = util::database_unit_t;
    using DbuPoint = geometry::Point<dbu_t>;

    namespace
    {
        inline DbuPoint convert(const lemon::dim2::Point<double> & p)
        {
            return DbuPoint{dbu_t{p.x}, dbu_t{p.y}};
        }

        inline lemon::dim2::Point<double> convert(const DbuPoint & p)
        {
            return {units::unit_cast<double>(p.x()), units::unit_cast<double>(p.y())};
        }
    }     // namespace

    uint32_t SteinerTree::size(Segment) const
    {
        return lemon::countEdges(mGraph);
    }

    uint32_t SteinerTree::size(Point) const
    {
        return lemon::countNodes(mGraph);
    }

namespace
{
    SteinerTree::GraphType::Node findNodeWithPositionEqualsTo(
        const DbuPoint & position,
        const SteinerTree::GraphType::NodeMap<lemon::dim2::Point<double>> & mPosition,
        const SteinerTree::GraphType & mGraph)
    {
        for(SteinerTree::GraphType::NodeIt i(mGraph); mGraph.valid(i); ++i)
        {
            if(geometry::ManhattanDistance(position, convert(mPosition[i])) == dbu_t{0.0}) {
                return i;
            }
        }

        return lemon::INVALID;
    }
}     // namespace
      //

    std::unique_ptr<SteinerTree> SteinerTree::create()
    {
        return std::unique_ptr<SteinerTree>{new SteinerTree};
    }

    SteinerTree::Point SteinerTree::add(const DbuPoint & position)
    {
        GraphType::Node node = findNodeWithPositionEqualsTo(position, mPosition, mGraph);
        if(node == lemon::INVALID) {
            node = mGraph.addNode();
            mPosition[node] = convert(position);
        }

        return Point(node);
    }

    SteinerTree::Segment SteinerTree::add(
        const SteinerTree::Point & p1,
        const SteinerTree::Point & p2)
    {
        auto    edge = mGraph.addEdge(p1.mEl, p2.mEl);
        Segment result(edge);

        mLength += length(result);

        return result;
    }

    SteinerTree::Point SteinerTree::u(const SteinerTree::Segment & segment) const
    {
        return Point(mGraph.u(segment.mEl));
    }

    SteinerTree::Point SteinerTree::v(const SteinerTree::Segment & segment) const
    {
        return Point(mGraph.v(segment.mEl));
    }

    DbuPoint SteinerTree::position(const SteinerTree::Point & p) const
    {
        return convert(mPosition[p.mEl]);
    }

    dbu_t SteinerTree::length(const SteinerTree::Segment & segment) const
    {
        const auto kU = mGraph.u(segment.mEl);
        const auto kV = mGraph.v(segment.mEl);

        return geometry::ManhattanDistance(convert(mPosition[kU]), convert(mPosition[kV]));
    }

    dbu_t SteinerTree::length() const
    {
        return mLength;
    }

    std::pair<SteinerTree::PointIterator, SteinerTree::PointIterator> SteinerTree::points() const
    {
        PointIterator first {Point {GraphType::NodeIt {mGraph}
                             }
        };
        PointIterator second {Point {static_cast<GraphType::Node>(lemon::INVALID)}
        };

        return std::make_pair(first, second);
    }

    std::pair<SteinerTree::PointSegmentsIterator,
        SteinerTree::PointSegmentsIterator> SteinerTree::segments(const SteinerTree::Point & point)
    const
    {
        PointSegmentsIterator first {{mGraph, point.mEl}
        };
        PointSegmentsIterator second {lemon::INVALID};

        return std::make_pair(first, second);
    }

    std::pair<SteinerTree::SegmentIterator,
        SteinerTree::SegmentIterator> SteinerTree::segments() const
    {
        SegmentIterator first {GraphType::EdgeIt {mGraph}
        };
        SegmentIterator second {lemon::INVALID};

        return std::make_pair(first, second);
    }

    SteinerTree::PointIterator::PointIterator(const SteinerTree::Point & p):
            mPoint(p)
    {
    }

    SteinerTree::PointSegmentsIterator::PointSegmentsIterator(GraphType::IncEdgeIt it):
            mIt(it)
    {
    }

    SteinerTree::SegmentIterator::SegmentIterator(GraphType::EdgeIt it):
            mIt(it)
    {
    }

namespace
{
    class SteinerTreeToEps :
        public SteinerTree::Attorney
    {
    public:
        static void run(const SteinerTree & tree, const std::string & filename)
        {
            const SteinerTree::GraphType & graph = SteinerTree::Attorney::graph(tree);
            auto & coords = SteinerTree::Attorney::position(tree);

            SteinerTree::GraphType::NodeMap<int> color(graph);
            lemon::Palette palette;
            int i = 0;
            for(SteinerTree::GraphType::NodeIt curr(graph); curr != lemon::INVALID; ++curr)
            {
                color[curr] = 1 + (i % 4);
                ++i;
            }
            lemon::graphToEps(
                graph,
                filename).coords(coords).nodeColors(lemon::composeMap(palette, color)).run();
        }
    };
}

    template <>
    void ToEps::run<SteinerTree>(const SteinerTree & tree, const std::string & filename)
    {
        SteinerTreeToEps::run(tree, filename);
    }

    geometry::Segment<geometry::Point<dbu_t>> make_segment(
        const interconnection::SteinerTree & tree,
        const interconnection::SteinerTree::Segment & segment)
    {
        return geometry::make<geometry::Segment, geometry::Point, dbu_t>(
            {tree.position(tree.u(segment)),
             tree.position(tree.v(segment))});
    }

}
