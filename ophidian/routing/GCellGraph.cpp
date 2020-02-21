#include <ophidian/routing/GCellGraph.h>

namespace ophidian::routing{

GCellGraph::GCellGraph(GCellGraph::unit_container_type x, GCellGraph::unit_container_type y, GCellGraph::index_type z):
    ophidian::util::GridGraph_3D(x.size()-1,y.size()-1,z)
{
    for (index_type x_it = 0; x_it < x.size() -1; ++x_it) {
        for (index_type y_it = 0; y_it < y.size() -1; ++y_it) {
            for (index_type z_it = 0; z_it < z; ++z_it) {
                auto gcell = m_gcells.add();
                m_gcell_node[gcell] = node(x_it, y_it, z_it);
            }

            auto min_corner = point_type{x.at(x_it), y.at(y_it)}; 
            auto max_corner = point_type{x.at(x_it + 1), y.at(y_it + 1)};
            m_gcell_box.emplace(std::make_pair( std::make_pair(x_it,y_it) , box_type(min_corner, max_corner) ));
        }
    }
}

GCellGraph::gcell_container_type::const_iterator GCellGraph::begin_gcell() const noexcept
{
    return m_gcells.begin();
}

GCellGraph::gcell_container_type::const_iterator GCellGraph::end_gcell() const noexcept
{
    return m_gcells.end();
}


}