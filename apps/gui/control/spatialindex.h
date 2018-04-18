#ifndef SPATIALINDEX_H
#define SPATIALINDEX_H

#include <vector>
#include <ophidian/geometry/Models.h>
#include <model/forms.h>

typedef std::pair<ophidian::geometry::Box, Quad> RTreeNode;

class RTreeNodeComparator
{
public:
    //! Copy Operator
    /*!
       \brief Copy the quad from node2 to node1.
       \param node1 RTree node.
       \param node2 RTree node.
     */
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
    //! SpatialIndex Constructor
    /*!
       \brief Constructs a SpatialIndex
     */
    SpatialIndex();

    //! Clean the RTree
    /*!
       \brief Removes all nodes from the RTree.
     */
    void clear();

    //! Creates a RTree node
    /*!
       \brief Create a Node using the Quad and insert it into the RTree.
       \param quad Quad of a Cell.
       \param box Box of the quad.
     */
    void quadCreate(const Quad & quad, const ophidian::geometry::Box & box);

    //! Removes a RTree node
    /*!
       \brief Remove a Node of a quad.
       \param quad Quad of a Cell.
     */
    void quadRemove(const Quad & quad);

    //! Quads from a area
    /*!
       \brief Select all quads that were in the specified area.
       \param box A area of the circuit.
       \return Vector containing the quads in the area.
     */
    std::vector<Quad> quadsContaining(const ophidian::geometry::Box & box) const;

    //! Quads from a point
    /*!
       \brief Find all quads that have intersection with the point.
       \param point A circuit coordinate.
       \return Vector containing the quads at intersection.
     */
    std::vector<Quad> quadsContaining(const ophidian::geometry::Point & point) const;

    //! Quad from a point
    /*!
       \brief Find the first quad that have intersection with the point.
       \param point A circuit coordinate.
       \return The Quad at intersection.
     */
    Quad quadContaining(const ophidian::geometry::Point & point) const;

    //! Is there a quad in this coordinate?
    /*!
       \brief Verify whether there is a quad in the coordinate.
       \param p A circuit coordinate.
       \return True if the quad exist, otherwise false.
     */
    bool hasQuad(const ophidian::geometry::Point & point) const;

private:
    RTree mIndex;
};

#endif // SPATIALINDEX_H
