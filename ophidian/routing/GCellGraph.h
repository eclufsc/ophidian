#ifndef OPHIDIAN_ROUTING_GCELLGRAPH_H
#define OPHIDIAN_ROUTING_GCELLGRAPH_H

#include <ophidian/routing/Library.h>
#include <ophidian/util/GridGraph_3D.h>

namespace ophidian::routing{


class GCellGraph :: public GridGraph_3D
{
    // Constructors
    //! Construct GCellGraph
    GCellGraph() = delete;

    //! coppy constructor
    GCellGraph(const Library &) = delete;
    GCellGraph & operator =(const GCellGraph &) = delete;

    //! Move Constructor
    GCellGraph(GCellGraph &&) = default;
    GCellGraph& operator=(GCellGraph &&) = default;

    GCellGraph(const Library &library);


};



}// end namespace

#endif // OPHIDIAN_ROUTING_GCELLGRAPH_H