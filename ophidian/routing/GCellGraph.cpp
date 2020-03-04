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

            //generate RTree;
            // using rtree_node_type       = std::pair<box_scalar_type, std::pair<index_type, index_type>>;
            // using rtree_type            = boost::geometry::index::rtree<rtree_node_type, boost::geometry::index::rstar<16> >;
            auto min_corner_d = point_scalar_type{units::unit_cast<double>(x.at(x_it)), units::unit_cast<double>(y.at(y_it))}; 
            auto max_corner_d = point_scalar_type{units::unit_cast<double>(x.at(x_it + 1)), units::unit_cast<double>(y.at(y_it + 1))};

            auto box = box_scalar_type{min_corner_d, max_corner_d};
            m_grid.insert(std::make_pair(box, std::make_pair(x_it,y_it)));
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

void GCellGraph::intersect(GCellGraph::gcell_container_type& gcells, const GCellGraph::box_type box, const GCellGraph::index_type layer)
{
    namespace bgi = boost::geometry::index;
    auto min_corner_d = point_scalar_type{units::unit_cast<double>(box.min_corner().x()), units::unit_cast<double>(box.min_corner().y())}; 
    auto max_corner_d = point_scalar_type{units::unit_cast<double>(box.max_corner().x()), units::unit_cast<double>(box.max_corner().y())};
    auto box_d = box_scalar_type{min_corner_d, max_corner_d};

    // using rtree_node_type       = std::pair<box_scalar_type, std::pair<index_type, index_type>>;
    std::vector<rtree_node_type> results;
    m_grid.query(bgi::intersects(box_d), std::back_inserter(results));

    for(auto r : results)
    {
        auto node = r.second;
        gcells.push_back(gcell(node.first, node.second, layer));
    }
}


}