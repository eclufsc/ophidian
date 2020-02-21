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

// A hash function used to hash a pair of any kind 
struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const std::pair<T1, T2>& p) const
    { 
        auto hash1 = std::hash<T1>{}(p.first); 
        auto hash2 = std::hash<T2>{}(p.second); 
        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return hash1 ^ hash2; 
    } 
};

class GCellGraph : public ophidian::util::GridGraph_3D
{
public:
    template <class T> using container_type = std::vector<T>;
    using index_type            = ophidian::util::GridGraph_3D::index_type;
    using node_type             = ophidian::util::GridGraph_3D::node_type;
    using unit_type             = util::database_unit_t;
    using unit_container_type   = container_type<unit_type>;
    using point_type            = ophidian::util::LocationDbu;
    using box_type              = geometry::Box<unit_type>;
    using gcell_type            = GCell;
    using gcell_container_type  = std::vector<gcell_type>;


    using map_type              = std::unordered_map<std::pair<index_type, index_type>, box_type, hash_pair >;

    // Constructors
    //! Construct GCellGraph
    GCellGraph() = delete;

    //! coppy constructor
    GCellGraph(const GCellGraph &) = delete;
    GCellGraph & operator =(const GCellGraph &) = delete;

    //! Move Constructor
    GCellGraph(GCellGraph &&) = default;
    GCellGraph& operator=(GCellGraph &&) = default;

    GCellGraph(unit_container_type x, unit_container_type y, index_type z);

    // Element access

    // Iterators
    gcell_container_type::const_iterator begin_gcell() const noexcept;
    gcell_container_type::const_iterator end_gcell() const noexcept;

    // Modifiers

private:
    entity_system::EntitySystem<gcell_type> m_gcells{};
    entity_system::Property<gcell_type, node_type> m_gcell_node{m_gcells};

    map_type m_gcell_box;
};



}// end namespace

#endif // OPHIDIAN_ROUTING_GCELLGRAPH_H