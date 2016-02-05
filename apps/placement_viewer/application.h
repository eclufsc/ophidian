#ifndef APPLICATION_H
#define APPLICATION_H

#include "../../src/placement/placement.h"

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <utility>

using point = openeda::geometry::point<double>;
using polygon = openeda::geometry::polygon<point>;
using multi_polygon = openeda::geometry::multi_polygon<polygon>;

using box = typename boost::geometry::model::box<openeda::geometry::point<double> >;
typedef std::pair<box, openeda::entity::entity> rtree_node;
class rtree_node_comparator {
public:
    bool operator()(const rtree_node & node1, const rtree_node & node2) const {
        return node1.second == node2.second;
    }
};

typedef boost::geometry::index::rtree<rtree_node,
boost::geometry::index::rstar<16>,
boost::geometry::index::indexable<rtree_node>, rtree_node_comparator> rtree;

class application
{
    openeda::standard_cell::standard_cells m_std_cells;
    openeda::netlist::netlist m_netlist;
    openeda::placement::library m_library;
    openeda::placement::placement m_placement;

    std::set<openeda::entity::entity> m_selected_cells;

    rtree m_position2cellentity;

    std::vector<rtree_node> create_rtree_nodes(openeda::entity::entity cell);

public:
    application(const std::string v_file, const std::string lef_file, const std::string def_file);


    virtual ~application();

    void click(point position);
    void release_click(point position);

    const openeda::placement::placement& placement() const {
        return m_placement;
    }

    const openeda::netlist::netlist & netlist() const {
        return m_netlist;
    }

    std::set<openeda::entity::entity> selected_cells() const {
        return m_selected_cells;
    }

    void place_cell_and_update_index(openeda::entity::entity cell, point position);

    bool has_cell(point position) const;
};

#endif // APPLICATION_H
