#ifndef SPATIALINDEX_H
#define SPATIALINDEX_H

#include <vector>

#include <ophidian/geometry/Models.h>
#include <Model/forms.h>

typedef std::pair<ophidian::geometry::Box, Quad> RTreeNode;

class RTreeNodeComparator
{
public:
    bool operator()(const RTreeNode & node1, const RTreeNode & node2) const;
};

typedef boost::geometry::index::rtree<
            RTreeNode,
            boost::geometry::index::rstar<16>,
            boost::geometry::index::indexable<RTreeNode>,
            RTreeNodeComparator
        > RTree;

class SpatialIndex
{
public:
    SpatialIndex();

    void clear();

    void quadCreate(const Quad & quad, const ophidian::geometry::Box & box);
    void quadRemove(const Quad & quad);

    std::vector<Quad> quadsContaining(const ophidian::geometry::Point & point) const;
    Quad quadContaining(const ophidian::geometry::Point & point) const;

    bool hasQuad(const ophidian::geometry::Point & point) const;

private:
    RTree mIndex;
};

#endif // SPATIALINDEX_H
