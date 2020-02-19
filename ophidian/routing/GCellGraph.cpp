#include <ophidian/routing/GCellGraph.h>

namespace ophidian::routing{

GCellGraph::GCellGraph(const ophidian::routing::Library &library) : 
    GridGraph_3D(library.get_GCell_x_axis().size(), library.get_GCell_y_axis().size(), library.index(library.highest_layer()) )
{

}

}