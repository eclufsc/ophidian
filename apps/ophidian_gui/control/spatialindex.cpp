#include "spatialindex.h"

bool RTreeNodeComparator::operator()(const RTreeNode & node1, const RTreeNode & node2) const
{
    return node1.second == node2.second;
}


SpatialIndex::SpatialIndex()
{

}

void SpatialIndex::clear()
{
    mIndex.clear();
}

void SpatialIndex::quadCreate(const Quad & quad, const ophidian::geometry::Box & box)
{
    mIndex.insert(RTreeNode(box, quad));
}

void SpatialIndex::quadRemove(const Quad & quad)
{
    std::vector<RTreeNode> toRemove;

    mIndex.query(boost::geometry::index::covered_by(mIndex.bounds()), std::back_inserter(toRemove));

    auto begin = std::remove_if(toRemove.begin(), toRemove.end(), [quad](const RTreeNode & node)->bool
    {
        return !(node.second == quad);
    });

    toRemove.erase(begin, toRemove.end());
    mIndex.remove(toRemove.begin(), toRemove.end());
}

std::vector<Quad> SpatialIndex::quadsContaining(const ophidian::geometry::Box & box) const
{
    std::vector<RTreeNode> result;

    mIndex.query(boost::geometry::index::intersects(box), std::back_inserter(result));

    std::vector<Quad> quads(result.size());

    for(std::size_t i = 0; i < result.size(); ++i)
    {
        quads[i] = result[i].second;
    }

    return quads;
}

std::vector<Quad> SpatialIndex::quadsContaining(const ophidian::geometry::Point & point) const
{
    std::vector<RTreeNode> result;

    mIndex.query(boost::geometry::index::contains(point), std::back_inserter(result));

    std::vector<Quad> quads(result.size());

    for(std::size_t i = 0; i < result.size(); ++i)
    {
        quads[i] = result[i].second;
    }

    return quads;
}

Quad SpatialIndex::quadContaining(const ophidian::geometry::Point & point) const
{
    return quadsContaining(point).front();
}

bool SpatialIndex::hasQuad(const ophidian::geometry::Point & point) const
{
    return !quadsContaining(point).empty();
}
