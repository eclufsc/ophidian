// #include <ophidian/routing/GCellGraph.h>

// namespace ophidian::routing{

// GCellGraph::GCellGraph():
//     ophidian::util::GridGraph_3D()
// {
// }

// // void GCellGraph::initialize(GCellGraph::index_type x, GCellGraph::index_type y, GCellGraph::index_type z)
// // {
// //     ophidian::util::GridGraph_3D::initialize(x,y,z);
// // }

// void GCellGraph::add_gcell(GCellGraph::index_type x, GCellGraph::index_type y, GCellGraph::index_type z, GCellGraph::point_type min_corner, GCellGraph::point_type max_corner)
// {
//     auto gcell = m_gcells.add();
//     m_gcell_min_corners[gcell] = min_corner;
//     m_gcell_max_corners[gcell] = max_corner;
//     m_gcell_node[gcell] = node(x, y, z);
// }


// GCellGraph::point_type GCellGraph::min_corner(const GCellGraph::gcell_type & gcell) const 
// {
//     return m_gcell_min_corners[gcell];
// }

// GCellGraph::point_type GCellGraph::max_corner(const GCellGraph::gcell_type & gcell) const 
// {
//     return m_gcell_max_corners[gcell];
// }



// GCellGraph::gcell_container_type::const_iterator GCellGraph::begin_gcell() const noexcept
// {
//     return m_gcells.begin();
// }

// GCellGraph::gcell_container_type::const_iterator GCellGraph::end_gcell() const noexcept
// {
//     return m_gcells.end();
// }


// }