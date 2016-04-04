#ifndef APPLICATION_H
#define APPLICATION_H

#include "../../src/placement/placement.h"

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <utility>

#include "../timing/library.h"
#include "../timing/graph.h"
#include "../timing/static_timing_analysis.h"
#include "../timing-driven_placement/sta_flute_net_calculator.h"

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
    openeda::timing::library_timing_arcs m_timing_arcs;
    openeda::timing::library m_timing_library;
    openeda::timing::graph m_graph;
    std::unique_ptr<openeda::timingdriven_placement::sta_flute_net_calculator> m_flute;
    std::unique_ptr<openeda::timing::static_timing_analysis> m_sta;

    rtree m_position2cellentity;

    std::vector<rtree_node> create_rtree_nodes(openeda::entity::entity cell);

public:
    application(const std::string v_file, const std::string lef_file, const std::string def_file, const std::string lib_file);


    virtual ~application();

    const openeda::placement::placement& placement() const {
        return m_placement;
    }

    const openeda::netlist::netlist & netlist() const {
        return m_netlist;
    }

    const openeda::standard_cell::standard_cells & std_cells() const {
        return m_std_cells;
    }

    void place_cell_and_update_index(openeda::entity::entity cell, point position);

    openeda::entity::entity get_cell(point position) const;

    bool cell_std_cell(openeda::entity::entity cell, std::string std_cell_name);


    boost::units::quantity< boost::units::si::time > cell_worst_slack(openeda::entity::entity cell) const;

    void run_sta();

    boost::units::quantity< boost::units::si::time > rise_arrival(openeda::entity::entity pin) const;
    boost::units::quantity< boost::units::si::time > fall_arrival(openeda::entity::entity pin) const;


    boost::units::quantity< boost::units::si::time > rise_slack(openeda::entity::entity pin) const;
    boost::units::quantity< boost::units::si::time > fall_slack(openeda::entity::entity pin) const;

    std::vector< openeda::entity::entity > critical_path() const;
};

#endif // APPLICATION_H
