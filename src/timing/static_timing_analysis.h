/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef OPHIDIAN_TIMING_STATIC_TIMING_ANALYSIS_H
#define OPHIDIAN_TIMING_STATIC_TIMING_ANALYSIS_H

#include "generic_sta.h"
#include "endpoints.h"


namespace ophidian {
namespace timing {


using TimeType = boost::units::quantity< boost::units::si::time > ;
using Cell = entity::entity;
using Net = entity::entity;
using Pin = entity::entity;



class static_timing_analysis
{
    const timing::graph * m_timing_graph;
    const entity::vector_property< interconnection::rc_tree > * m_rc_trees;
    const library * m_late_lib;
    const library * m_early_lib;
    const netlist::netlist * m_netlist;
    design_constraints m_dc;


    // lazy pointers
    std::unique_ptr<timing_data> m_late;
    std::unique_ptr<timing_data> m_early;
    std::unique_ptr<graph_and_topology> m_topology;
    std::unique_ptr<generic_sta<effective_capacitance_wire_model, pessimistic> > m_late_sta;
    std::unique_ptr<generic_sta<effective_capacitance_wire_model, optimistic> > m_early_sta;
    std::unique_ptr<test_calculator> m_test;
    endpoints m_endpoints;
    TimeType m_lwns;
    TimeType m_ewns;
    TimeType m_ltns;
    TimeType m_etns;

    void init_timing_data();
    void propagate_ats();
    void propagate_rts();
    void update_wns_and_tns();
    bool has_timing_data() const {
        assert(m_rc_trees);
        assert(m_timing_graph);
        assert(m_late_lib && m_early_lib);
        assert(m_netlist);
        return m_late_sta && m_early_sta;
    }
public:
    static_timing_analysis();
    void graph(const timing::graph& g);
    void rc_trees(const entity::vector_property< interconnection::rc_tree > & trees);
    void late_lib(const library& lib);
    void early_lib(const library& lib);
    void netlist(const netlist::netlist & netlist);
    void set_constraints(const design_constraints & dc);

    void update_timing();


    TimeType late_wns() const {
        return m_lwns;
    }
    TimeType early_wns() const{
        return m_ewns;
    }
    TimeType late_tns() const {
        return m_ltns;
    }
    TimeType early_tns() const{
        return m_etns;
    }

    TimeType early_rise_slack(Pin p) const {
        return m_early_sta->rise_slack(p);
    }
    TimeType early_fall_slack(Pin p) const {
        return m_early_sta->fall_slack(p);
    }
    TimeType late_rise_slack(Pin p) const {
        return m_late_sta->rise_slack(p);
    }
    TimeType late_fall_slack(Pin p) const {
        return m_late_sta->rise_slack(p);
    }

    TimeType early_rise_arrival(Pin p) const {
        return m_early_sta->rise_arrival(p);
    }
    TimeType early_fall_arrival(Pin p) const {
        return m_early_sta->fall_arrival(p);
    }
    TimeType late_rise_arrival(Pin p) const {
        return m_late_sta->rise_arrival(p);
    }
    TimeType late_fall_arrival(Pin p) const {
        return m_late_sta->rise_arrival(p);
    }

    TimeType early_rise_slew(Pin p) const {
        return m_early_sta->rise_slew(p);
    }
    TimeType early_fall_slew(Pin p) const {
        return m_early_sta->fall_slew(p);
    }
    TimeType late_rise_slew(Pin p) const {
        return m_late_sta->rise_slew(p);
    }
    TimeType late_fall_slew(Pin p) const {
        return m_late_sta->rise_slew(p);
    }

    const endpoints & timing_endpoints() const {
        return m_endpoints;
    }

};

}
}


#endif // OPHIDIAN_TIMING_STATIC_TIMING_ANALYSIS_H
