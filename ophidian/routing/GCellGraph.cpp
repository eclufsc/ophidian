#include <ophidian/routing/GCellGraph.h>

namespace ophidian::routing{


GCellGraph::GCellGraph(GCellGraph::unit_container_type x, GCellGraph::unit_container_type y, GCellGraph::index_type z, GCellGraph::scalar_container_type capacity):
    ophidian::util::GridGraph_3D(x.size()-1,y.size()-1,z), m_nodes_to_gcell(m_graph)
{
    for (index_type x_it = 0; x_it < x.size() -1; ++x_it) {
        for (index_type y_it = 0; y_it < y.size() -1; ++y_it) {
            for (index_type z_it = 0; z_it < z; ++z_it) {
                auto gcell = m_gcells.add();
                auto node_inst = node(x_it, y_it, z_it);
                m_gcell_node[gcell] = node_inst;
                m_nodes_to_gcell[node_inst] = gcell;
                if(z_it > capacity.size()-1 )
                {
                    //if capacity is not provided for this z the last capacity is replicated for this layer
                    m_gcell_capacity[gcell] = capacity[capacity.size()-1];
                }else{
                    m_gcell_capacity[gcell] = capacity[z_it];
                }
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


GCellGraph::gcell_type GCellGraph::gcell(GCellGraph::index_type x, GCellGraph::index_type y, GCellGraph::index_type z) const
{
    auto n = node(x,y,z);
    return m_nodes_to_gcell[n];
}

GCellGraph::box_type GCellGraph::box(const GCellGraph::gcell_type& gcell)
{
    auto node = m_gcell_node[gcell];
    auto index = position(node);
    auto x = index.get<0>();
    auto y = index.get<1>();
    return m_gcell_box[std::make_pair(x,y)];
}

GCellGraph::scalar_type GCellGraph::capacity(const GCellGraph::gcell_type& gcell)
{
    return m_gcell_capacity[gcell];
}

GCellGraph::scalar_type GCellGraph::demand(const GCellGraph::gcell_type& gcell)
{
    return m_gcell_demand[gcell];
}

void GCellGraph::increase_demand(const GCellGraph::gcell_type& gcell)
{
    m_gcell_demand[gcell] += 1;
}

// GCellGraph::gcell_type& gcell GCellGraph::intersect(const GCellGraph::box_type box, const GCellGraph::layer_type & layer)
// {
//     return 0;
// }


}