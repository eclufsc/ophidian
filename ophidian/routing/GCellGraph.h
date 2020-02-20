#ifndef OPHIDIAN_ROUTING_GCELLGRAPH_H
#define OPHIDIAN_ROUTING_GCELLGRAPH_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
// #include <ophidian/entity_system/Aggregation.h>

#include <ophidian/routing/Library.h>
#include <ophidian/util/GridGraph_3D.h>

namespace ophidian::routing{


class GCell :
    public entity_system::EntityBase
{
public:
    using entity_system::EntityBase::EntityBase;
};


class GCellGraph : public ophidian::util::GridGraph_3D
{
    using index_type            = ophidian::util::GridGraph_3D::index_type;
    using node_type             = ophidian::util::GridGraph_3D::node_type;
    using point_type            = ophidian::util::LocationDbu;
    using gcell_type            = GCell;
    using gcell_container_type  = std::vector<gcell_type>;

    // Constructors
    //! Construct GCellGraph
    GCellGraph() = delete;

    //! coppy constructor
    GCellGraph(const GCellGraph &) = delete;
    GCellGraph & operator =(const GCellGraph &) = delete;

    //! Move Constructor
    GCellGraph(GCellGraph &&) = default;
    GCellGraph& operator=(GCellGraph &&) = default;

    GCellGraph(index_type x, index_type y, index_type z);

    void add_gcell(index_type x, index_type y, index_type z, point_type min_corner, point_type max_corner);

    // Element access
    point_type min_corner(const gcell_type & gcell) const;
    point_type max_corner(const gcell_type & gcell) const;

    // Iterators
    gcell_container_type::const_iterator begin_gcell() const noexcept;
    gcell_container_type::const_iterator end_gcell() const noexcept;

    // Modifiers

private:
    entity_system::EntitySystem<gcell_type> m_gcells{};
    entity_system::Property<gcell_type, node_type> m_gcell_node{m_gcells};
    entity_system::Property<gcell_type, point_type> m_gcell_min_corners{m_gcells};
    entity_system::Property<gcell_type, point_type> m_gcell_max_corners{m_gcells};

};



}// end namespace

#endif // OPHIDIAN_ROUTING_GCELLGRAPH_H