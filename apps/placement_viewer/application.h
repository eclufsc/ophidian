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
#include "../../src/floorplan/floorplan.h"

using point = ophidian::geometry::point<double>;
using polygon = ophidian::geometry::polygon<point>;
using multi_polygon = ophidian::geometry::multi_polygon<polygon>;

using box = typename boost::geometry::model::box<ophidian::geometry::point<double> >;
typedef std::pair<box, ophidian::entity::entity> rtree_node;
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
    ophidian::standard_cell::standard_cells m_std_cells;
    ophidian::netlist::netlist m_netlist;
    ophidian::placement::library m_library;
    ophidian::placement::placement m_placement;
    ophidian::floorplan::floorplan m_floorplan;
    ophidian::timing::library_timing_arcs m_timing_arcs;
    ophidian::timing::library m_timing_library;
    ophidian::timing::graph m_graph;
    std::unique_ptr<ophidian::timingdriven_placement::sta_flute_net_calculator> m_flute;
    std::unique_ptr<ophidian::timing::static_timing_analysis> m_sta;

    rtree m_position2cellentity;

    std::vector<rtree_node> create_rtree_nodes(ophidian::entity::entity cell);

public:
    application(const std::string v_file, const std::string lef_file, const std::string def_file, const std::string lib_file);


    virtual ~application();

    const ophidian::placement::placement& placement() const {
        return m_placement;
    }

    const ophidian::netlist::netlist & netlist() const {
        return m_netlist;
    }

    const ophidian::standard_cell::standard_cells & std_cells() const {
        return m_std_cells;
    }

    void place_cell_and_update_index(ophidian::entity::entity cell, point position);

    ophidian::entity::entity get_cell(point position) const;

    bool cell_std_cell(ophidian::entity::entity cell, std::string std_cell_name);


    boost::units::quantity< boost::units::si::time > cell_worst_slack(ophidian::entity::entity cell) const;

    void run_sta();

    boost::units::quantity< boost::units::si::time > rise_arrival(ophidian::entity::entity pin) const;
    boost::units::quantity< boost::units::si::time > fall_arrival(ophidian::entity::entity pin) const;


    boost::units::quantity< boost::units::si::time > rise_slack(ophidian::entity::entity pin) const;
    boost::units::quantity< boost::units::si::time > fall_slack(ophidian::entity::entity pin) const;

    std::vector< ophidian::entity::entity > critical_path() const;
};

#endif // APPLICATION_H
