#include "ILPRouting.h"

#include <fstream>
#include <regex>
#include <boost/lexical_cast.hpp>
#include <ophidian/util/log.h>

#ifndef STATUS
#define STATUS true
#define DEBUG false
#define WRITE_MODEL true
#endif

using namespace ophidian::util;

namespace ophidian::routing {

    template <typename var_type>
    ILPRouting<var_type>::ILPRouting(design::Design & design, std::string circuit_name):
        m_design(design), m_circuit_name(circuit_name)
    {
        m_extra_demand_created = false;
    }

    template <typename var_type>
    std::pair<bool, typename ILPRouting<var_type>::Statistics> ILPRouting<var_type>::route_nets(const std::vector<net_type> & nets, const std::vector<cell_type> & cells, box_type & area, const std::vector<net_type> & fixed_nets, std::vector<net_type> & routed_nets, std::vector<net_type> & unrouted_nets, std::vector<std::pair<cell_type, point_type>> & movements, bool initial_routing)
    {
        if(STATUS) printlog("init function route_nets");
        ILPRouting<var_type>::Statistics statistic;

        //if(!m_extra_demand_created)
        //    add_extra_demand(cells);

        if(STATUS) printlog("Cleaning the data structures ...");
        //m_segments.clear();
        m_wires.clear();
        m_route_candidate.clear();
        m_position_candidates.clear();

        if(STATUS) printlog("Instantiating the envirorment ...");

        m_env = environment_type{};        
        model_type model(m_env);
        solver_type cplex(model);
//        cplex.setOut(m_env.getNullStream());
        // cplex.setParam(IloCplex::Param::Threads, 4);

        if(STATUS) printlog("update capacities from blockages");
        update_gcell_capacities(fixed_nets, cells);
        if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;

        // add_extra_demand();

        if(STATUS) printlog("create all cells initial candidates");
        create_all_cell_initial_candidates(cells, model);
        if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;

        if(STATUS) printlog("create all candidates");
        create_all_candidates(nets, model);
        if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;

        // if(STATUS) printlog("create all candidates with movements");
        //create_all_candidates_with_movements(nets, cells, area, model);
        // if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;

        if(STATUS) printlog("add objective function");
        add_objective_function(model);
        if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;

        if(STATUS) printlog("add candidate constraints");
        add_candidate_constraints(nets, cells, model);
        if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;

        if(STATUS) printlog("add capacity constraints");
        add_capacity_constraints(nets, cells, area, model);
        if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;

        // if(STATUS) printlog("add movements constraints");
        //add_movements_constraints(model);
        // if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;

        if(WRITE_MODEL) printlog("write model");
        if(WRITE_MODEL) cplex.exportModel("ilp_routing_model.lp");
        if(WRITE_MODEL) printlog("exported");

        if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB" << std::endl;
        log() << std::endl;

        auto time_begin = std::chrono::high_resolution_clock::now();
        bool solved = cplex.solve();
        auto time_end = std::chrono::high_resolution_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_end-time_begin).count();
        auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(time_end-time_begin).count();
        if(STATUS) log() << "solved = " << solved << " in " << duration_s << " seconds | or | " << duration_ms << " milliseconds" << std::endl;
        statistic.model_runtime_ms = duration_ms;

        auto status = cplex.getCplexStatus();

        auto result = (status == IloCplex::CplexStatus::Optimal || status == IloCplex::CplexStatus::Feasible || status == IloCplex::CplexStatus::OptimalTol);

        if(STATUS) log() << "status " << status << std::endl;

        if(STATUS) log() << "result " << result << std::endl;

        if(STATUS) log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB"
              << std::endl;

        if(result)
        {
            if(WRITE_MODEL) printlog("writing solution");
            if(WRITE_MODEL) cplex.writeSolution("ilp_routing_model.sol");

    	    write_segments(nets, cplex, routed_nets, unrouted_nets);
            save_movements(cplex, movements);

            //log() << "moved cells " << m_moved_cells << std::endl;

            auto total_nets = m_design.netlist().size_net();
            double ratio = (double) routed_nets.size() / (double) total_nets;
            log() << std::endl;
            log() << "total nets " << total_nets << std::endl;
            log() << "routed nets " << routed_nets.size() << std::endl;
            log() << "routed nets " << ratio*100.0<<"%" << std::endl;
            log() << "unrouted nets " << (total_nets - routed_nets.size()) << std::endl;
            log() << std::endl;

	        // save_result(cplex);
        }
        m_env.end();
        return std::make_pair(result, statistic);
     }

    template <typename var_type>
    void ILPRouting<var_type>::update_gcell_capacities(const std::vector<net_type> & fixed_nets, const cell_container_type & cells)
    {
        auto& routing_library = m_design.routing_library();
        auto& global_routing = m_design.global_routing();
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        auto gcell_graph = global_routing.gcell_graph();

        //copy the blockageDenamd to ILP propoerty!
        // for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
        //     auto gcell = *gcell_it;
        //     auto blockage_demand = gcell_graph->blockage_demand(gcell);
        //     m_gcells_demand[gcell] = blockage_demand;
        // }
        for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
            auto gcell = *gcell_it;

            m_gcells_demand[gcell] = 0;
        }

        /*for (auto cell_it = m_design.netlist().begin_cell_instance(); cell_it != m_design.netlist().end_cell_instance(); cell_it++) {
            auto cell = *cell_it;
            if (std::find(cells.begin(), cells.end(), cell) != cells.end()) {
                continue;
            }

            auto position = placement.location(cell);

            auto std_cell = netlist.std_cell(cell);
            auto blockages = routing_library.blockages(std_cell);
            for(auto blockage : blockages)
            {
                auto layer = routing_library.layer(blockage);
                auto layer_index = routing_library.layerIndex(layer);
                auto gcell = gcell_graph->nearest_gcell(position, layer_index-1);
                auto demand = routing_library.demand(blockage);

                m_gcells_demand[gcell] += demand;
            }
        }*/

        // here we have to update the demand of nets with won't be routed in this execution!
        for (auto net : fixed_nets) {
            //std::unordered_set<gcell_type, entity_system::EntityBaseHash> gcells;
            std::vector<gcell_type> intersecting_gcells;
            for (auto segment : global_routing.segments(net)) {
                auto segment_box = global_routing.box(segment);
                auto layer_start = global_routing.layer_start(segment);
                auto layer_end = global_routing.layer_end(segment);
                auto layer_start_index = routing_library.layerIndex(layer_start);
                auto layer_end_index = routing_library.layerIndex(layer_end);

                auto min_layer_index = std::min(layer_start_index, layer_end_index);
                auto max_layer_index = std::max(layer_start_index, layer_end_index);

                auto base_gcells = gcell_container_type{};
                gcell_graph->intersect(base_gcells, segment_box, min_layer_index-1);

                for(auto layer_index = min_layer_index; layer_index <= max_layer_index; layer_index++)
                {
                    auto layer = routing_library.layer_from_index(layer_index);
                    auto layer_name = routing_library.name(layer);
                    for(auto base_gcell : base_gcells)
                    {
                        auto base_gcell_node = gcell_graph->graph_node(base_gcell);
                        auto base_gcell_position = gcell_graph->position(base_gcell_node);
                        auto gcell = gcell_graph->gcell(base_gcell_position.get<0>(), base_gcell_position.get<1>(), layer_index-1);
                        if (std::find(intersecting_gcells.begin(), intersecting_gcells.end(), gcell) == intersecting_gcells.end()) {
                           intersecting_gcells.push_back(gcell);
                        }
                    }
                }
            }
            for (auto gcell : intersecting_gcells) {
                m_gcells_demand[gcell] += 1;
            }
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::add_extra_demand(const cell_container_type & cells)
    {
        printlog("Genearting extra demand values for all gcells");
        m_extra_demand_created = true;

        auto& netlist = m_design.netlist();
        auto& global_routing = m_design.global_routing();
        auto gcell_graph = global_routing.gcell_graph();
        auto & routing_library = m_design.routing_library();

        std::unordered_map<routing::GCellGraph::gcell_type, std::unordered_map<circuit::StandardCells::cell_type, int, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;

        std::unordered_map<circuit::StandardCells::cell_type, std::unordered_set<routing::GCellGraph::gcell_type, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> gcells_per_std_cell;
        /*for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
            auto gcell = *gcell_it;
            auto gcell_box = gcell_graph->box(gcell);
            auto cells_within_gcell = placement::Placement::cell_container_type{};
            m_design.placement().cells_within(gcell_box, cells_within_gcell);

            std::unordered_map<circuit::StandardCells::cell_type, double, entity_system::EntityBaseHash> std_cell_count;
            for (auto cell : cells_within_gcell) {
                auto std_cell = m_design.netlist().std_cell(cell);
                std_cell_count[std_cell] += 1;
            }
            std_cells_per_gcell[gcell] = std_cell_count;
        }*/

        for (auto & cell : cells) {
            auto location = m_design.placement().location(cell);
            auto std_cell = m_design.netlist().std_cell(cell);

            auto cell_name = m_design.netlist().name(cell);
            auto std_cell_name = m_design.standard_cells().name(std_cell);

            // for (auto layer_it = routing_library.begin_layer(); layer_it != routing_library.end_layer(); layer_it++) {
                // auto layer = *layer_it;
                // auto layer_index = routing_library.layerIndex(layer);
                // auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);
            for (auto layer_index = routing_library.lowest_layer_index(); layer_index <= routing_library.highest_layer_index(); layer_index++) {
                auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);

                //std::cout << "cell " << cell_name << " std cell " << std_cell_name << " location " << location.x().value() << "," << location.y().value() << "," << layer_index;
                //auto gcell_box = gcell_graph->box(gcell);
                //std::cout << " gcell " << gcell_box.min_corner().x().value() << "," << gcell_box.min_corner().y().value() << std::endl;

                std_cells_per_gcell[gcell][std_cell] += 1;
                gcells_per_std_cell[std_cell].insert(gcell);
            }
        }

        auto & routing_constraints = m_design.routing_constraints();
        for (auto same_grid_it = routing_constraints.begin_same_grid(); same_grid_it != routing_constraints.end_same_grid(); same_grid_it++) {
            auto key = same_grid_it->first;
            auto & extra_demand = same_grid_it->second;
            auto demand = extra_demand.demand;

            auto macro1_name = extra_demand.macro1;
            auto macro2_name = extra_demand.macro2;
            auto layer_name = extra_demand.layer;

            auto macro1 = m_design.standard_cells().find_cell(macro1_name);
            auto macro2 = m_design.standard_cells().find_cell(macro2_name);
            auto layer = m_design.routing_library().find_layer_instance(layer_name);

            auto gcells_macro1 = gcells_per_std_cell[macro1];
            auto gcells_macro2 = gcells_per_std_cell[macro2];
            
            auto rule_gcells = std::unordered_set<gcell_type, entity_system::EntityBaseHash>{};
            for (auto gcell : gcells_macro1) {
                rule_gcells.insert(gcell);
            }
            for (auto gcell : gcells_macro2) {
                rule_gcells.insert(gcell);
            }

            for (auto gcell : rule_gcells) {
                auto gcell_layer_index = gcell_graph->layer_index(gcell);
                auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
                auto gcell_layer = m_design.routing_library().find_layer_instance(gcell_layer_name);

                if (layer == gcell_layer) {
                    auto pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[gcell][macro2]);
                    m_gcells_extra_demand[gcell] += pair_count * demand;
                }
            }

        }

        for (auto adj_grid_it = routing_constraints.begin_adj_grid(); adj_grid_it != routing_constraints.end_adj_grid(); adj_grid_it++) {
            auto key = adj_grid_it->first;
            auto & extra_demand = adj_grid_it->second;
            auto demand = extra_demand.demand;

            auto macro1_name = extra_demand.macro1;
            auto macro2_name = extra_demand.macro2;
            auto layer_name = extra_demand.layer;

            auto macro1 = m_design.standard_cells().find_cell(macro1_name);
            auto macro2 = m_design.standard_cells().find_cell(macro2_name);
            auto layer = m_design.routing_library().find_layer_instance(layer_name);
            
            auto gcells_macro1 = gcells_per_std_cell[macro1];
            auto gcells_macro2 = gcells_per_std_cell[macro2];

            auto rule_gcells = std::unordered_set<gcell_type, entity_system::EntityBaseHash>{};
            for (auto gcell : gcells_macro1) {
                rule_gcells.insert(gcell);
            }
            for (auto gcell : gcells_macro2) {
                rule_gcells.insert(gcell);
            }

            for (auto gcell : rule_gcells) {
                auto gcell_node = gcell_graph->graph_node(gcell);
                auto east_node = gcell_graph->east_node(gcell_node);
                auto east_gcell = gcell_graph->gcell(east_node);
                auto west_node = gcell_graph->west_node(gcell_node);
                auto west_gcell = gcell_graph->gcell(west_node);

                auto gcell_layer_index = gcell_graph->layer_index(gcell);
                auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
                auto gcell_layer = m_design.routing_library().find_layer_instance(gcell_layer_name);
            if (layer == gcell_layer) {
                auto east_pair_count = 0;
                auto west_pair_count = 0;
                if (macro1 == macro2) {
                    if (east_node != lemon::INVALID) {
                        east_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[east_gcell][macro1]);
                    }
                    if (west_node != lemon::INVALID) {
                        west_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[west_gcell][macro1]);
                    }
                } else {
                    if (east_node != lemon::INVALID) {
                        east_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[east_gcell][macro2])
                         + std::min(std_cells_per_gcell[gcell][macro2], std_cells_per_gcell[east_gcell][macro1]);
                    }
                    if (west_node != lemon::INVALID) {
                        west_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[west_gcell][macro2])
                         + std::min(std_cells_per_gcell[gcell][macro2], std_cells_per_gcell[west_gcell][macro1]);
                    }
                }
                m_gcells_extra_demand[gcell] += (east_pair_count + west_pair_count) * demand;
            }
            }
        }

        /*for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
            auto gcell = *gcell_it;
            auto gcell_layer_index = gcell_graph->layer_index(gcell);
            auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
            auto gcell_layer = m_design.routing_library().find_layer_instance(gcell_layer_name);

            auto gcell_box = gcell_graph->box(gcell);

            for (auto same_grid_it = routing_constraints.begin_same_grid(); same_grid_it != routing_constraints.end_same_grid(); same_grid_it++) {
                auto key = same_grid_it->first;
                auto & extra_demand = same_grid_it->second;
                auto demand = extra_demand.demand;
                
                auto macro1_name = extra_demand.macro1;
                auto macro2_name = extra_demand.macro2;
                auto layer_name = extra_demand.layer;

                auto macro1 = m_design.standard_cells().find_cell(macro1_name);
                auto macro2 = m_design.standard_cells().find_cell(macro2_name);
                auto layer = m_design.routing_library().find_layer_instance(layer_name);

                if (layer == gcell_layer) {
                    auto pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[gcell][macro2]);
                    m_gcells_extra_demand[gcell] += pair_count * demand;
                }
            }

            auto gcell_node = gcell_graph->graph_node(gcell);
            auto east_node = gcell_graph->east_node(gcell_node);
            auto east_gcell = gcell_graph->gcell(east_node);
            auto west_node = gcell_graph->west_node(gcell_node);
            auto west_gcell = gcell_graph->gcell(west_node);

            for (auto adj_grid_it = routing_constraints.begin_adj_grid(); adj_grid_it != routing_constraints.end_adj_grid(); adj_grid_it++) {
                auto key = adj_grid_it->first;
                auto & extra_demand = adj_grid_it->second;
                auto demand = extra_demand.demand;
                
                auto macro1_name = extra_demand.macro1;
                auto macro2_name = extra_demand.macro2;
                auto layer_name = extra_demand.layer;

                auto macro1 = m_design.standard_cells().find_cell(macro1_name);
                auto macro2 = m_design.standard_cells().find_cell(macro2_name);
                auto layer = m_design.routing_library().find_layer_instance(layer_name);

                if (layer == gcell_layer) {
                    auto east_pair_count = 0;
                    auto west_pair_count = 0;
                    if (macro1 == macro2) {
                        if (east_node != lemon::INVALID) {
                            east_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[east_gcell][macro1]);
                        }
                        if (west_node != lemon::INVALID) {
                            west_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[west_gcell][macro1]);
                        }
                    } else {
                        if (east_node != lemon::INVALID) {
                            east_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[east_gcell][macro2])
                            + std::min(std_cells_per_gcell[gcell][macro2], std_cells_per_gcell[east_gcell][macro1]);
                        }
                        if (west_node != lemon::INVALID) {
                            west_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[west_gcell][macro2])
                            + std::min(std_cells_per_gcell[gcell][macro2], std_cells_per_gcell[west_gcell][macro1]);
                        }
                    }
                    m_gcells_extra_demand[gcell] += (east_pair_count + west_pair_count) * demand;
                }
            }
        }*/
    }


    template <typename var_type>
    void ILPRouting<var_type>::create_all_candidates(const std::vector<net_type> & nets, model_type & model, bool initial_routing)
    {
        for(auto net : nets) {
            create_net_candidates(net, model, initial_routing);
        }
    }

    
    template <typename var_type>
    void ILPRouting<var_type>::create_net_candidates(const net_type & net, model_type & model, bool initial_routing)
    {
        auto& netlist = m_design.netlist();
        auto& global_routing = m_design.global_routing();
        auto& routing_library = m_design.routing_library();
        auto& placement = m_design.placement();
        auto& routing_constraints = m_design.routing_constraints();
        auto graph_ptr = global_routing.gcell_graph();

        auto net_name = netlist.name(net);

        /*int net_number = -1;
        std::regex word_regex("(\\d+)");
        auto words_begin = std::sregex_iterator(net_name.begin(), net_name.end(), word_regex);
        if(words_begin != std::sregex_iterator())
            net_number = boost::lexical_cast<int>(words_begin->str());*/

        auto segments = global_routing.segments(net);

        auto min_layer = routing_library.highest_layer();
        auto max_layer = routing_library.lowest_layer();
        if(segments.size() > 0 && initial_routing)
        {
            auto initial_candidate = m_route_candidate.add();
            m_route_candidate_nets[initial_candidate] = net;
            auto initial_variable_name = net_name + "_initial";
            var_type initial_variable(m_env, 0.0, 1.0, initial_variable_name.c_str());
            m_route_candidate_names[initial_candidate] = initial_variable_name;
            m_name_to_route_candidate[initial_variable_name] = initial_candidate;
            m_route_candidate_variables[initial_candidate] = initial_variable;
            m_net_candidates.addAssociation(net, initial_candidate);

            auto wires = wire_container_type{};
            for(auto segment : segments)
            {
                auto start_gcell = global_routing.gcell_start(segment);
                auto start_box = graph_ptr->box(start_gcell);
                point_type start{(start_box.min_corner().x()+start_box.max_corner().x())/2,
                                 (start_box.min_corner().y()+start_box.max_corner().y())/2};
                auto end_gcell = global_routing.gcell_end(segment);
                auto end_box = graph_ptr->box(end_gcell);
                point_type end{(end_box.min_corner().x()+end_box.max_corner().x())/2,
                               (end_box.min_corner().y()+end_box.max_corner().y())/2};

                auto start_layer = global_routing.layer_start(segment);
                if( routing_library.is_less(start_layer, min_layer) )
                    min_layer = start_layer;
                if( routing_library.is_greater(start_layer, max_layer) )
                    max_layer = start_layer;
                auto end_layer = global_routing.layer_end(segment);
                if( routing_library.is_less(end_layer, min_layer) )
                    min_layer = start_layer;
                if( routing_library.is_greater(end_layer, max_layer) )
                    max_layer = end_layer;
                auto start_layer_name = routing_library.name(start_layer);
                auto end_layer_name = routing_library.name(end_layer);
                auto wire = create_wire(start, end, start_layer, end_layer);
                wires.push_back(wire);
            }
            add_wires_to_candidate(initial_candidate, wires);
        }
        /*
        //define delta in layers
        int delta_layers = 0;
        for(auto i = 0; i < delta_layers; i++)
        {
            min_layer = routing_library.lowerLayer(min_layer);
            max_layer = routing_library.upperLayer(max_layer);
        }
        */
        generate_routes_of_net(net, position_candidate_type(), model, std::make_pair(min_layer, max_layer));
    }

    template <typename var_type>
    bool ILPRouting<var_type>::cell_is_only_connected_to_local_nets(cell_type & cell, const std::vector<net_type> & nets) {
        for (auto pin : m_design.netlist().pins(cell)) {
            auto net = m_design.netlist().net(pin);
            if (std::find(nets.begin(), nets.end(), net) == nets.end()) {
                return false;
            }
        } 
        return true;
    }

    template <typename var_type>
    void ILPRouting<var_type>::create_all_candidates_with_movements(const std::vector<net_type> & nets, const std::vector<cell_type> & cells, box_type & area, model_type & model)
    {
        auto & netlist = m_design.netlist();
        auto & placement = m_design.placement();
        /*for(auto net : nets)
        {
            auto size = netlist.pins(net).size();
            if(size == 2)
                create_2_pin_nets_candidates_with_movements(net, model);
        }*/
        for (auto cell : cells) {
            if( !placement.isFixed(cell) && cell_is_only_connected_to_local_nets(cell, nets))
            {
                //create_center_of_mass_candidate(cell, model);
                create_median_candidate(cell, area, model);
            }
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::create_center_of_mass_candidate(const cell_type cell, model_type & model){
        auto cell_name = m_design.netlist().name(cell);
        //std::cout << "center of mass of cell " << cell_name << std::endl;

        auto & netlist = m_design.netlist();
        auto & placement = m_design.placement();
        std::vector<net_type> cell_nets;
        std::vector<point_type> positions;
        for(auto pin : netlist.pins(cell)){
            auto net = netlist.net(pin);
            cell_nets.push_back(net);
            for(auto net_pin : netlist.pins(net)){
                if(net_pin == pin)
                    continue;
                auto location = placement.location(net_pin);               
                positions.push_back(location);

                //std::cout << "pin location " << location.x().value() << ", " << location.y().value() << std::endl;
            }
        }
        double sum_x = 0.0;
        double sum_y = 0.0;
        for(auto position : positions){
            sum_x += position.x().value();
            sum_y += position.y().value();
        }
        double new_x = sum_x / positions.size();
        double new_y = sum_y / positions.size();

        point_type center_mass {unit_type(new_x), unit_type(new_y)};
        //std::cout << "center of mass " << center_mass.x().value() << ", " << center_mass.y().value() << std::endl;
        
        auto nearst_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(center_mass, 0);

        auto cell_location = placement.location(cell);
        auto actual_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(cell_location, 0);

        if(actual_gcell != nearst_gcell){
            auto center_gcell = m_design.global_routing().gcell_graph()->center_of_box(nearst_gcell);
            //std::cout << "center gcell " << center_gcell.x().value() << ", " << center_gcell.y().value() << std::endl;
            // creating candidate
            auto cell_name = netlist.name(cell);
            create_movement_candidate(cell, candidate_origin_type::CENTER_OF_MASS, center_gcell, cell_nets, cell_name + "_center_mass", model);
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::create_median_candidate(const cell_type cell, box_type & area, model_type & model){
        auto & netlist = m_design.netlist();
        auto & placement = m_design.placement();
        std::vector<net_type> cell_nets;
        
        std::vector<double> x_positions;
        std::vector<double> y_positions;

        for(auto pin : netlist.pins(cell)){
            auto net = netlist.net(pin);
            if(net == ophidian::circuit::Net())
                continue;
            cell_nets.push_back(net);
            auto x_min = std::numeric_limits<double>::max();
            auto y_min = std::numeric_limits<double>::max();
            auto x_max = -std::numeric_limits<double>::max();
            auto y_max = -std::numeric_limits<double>::max();
            for(auto net_pin : netlist.pins(net)){
                if(net_pin == pin)
                    continue;
                auto location = placement.location(net_pin);
                x_min = std::min(x_min, location.x().value());
                y_min = std::min(y_min, location.y().value());
                x_max = std::max(x_max, location.x().value());
                y_max = std::max(y_max, location.y().value());
                //x_positions.push_back(location.x().value());
                //y_positions.push_back(location.y().value());
            }
            x_positions.push_back(x_min);
            x_positions.push_back(x_max);
            y_positions.push_back(y_min);
            y_positions.push_back(y_max);
        }
        if(x_positions.empty() || y_positions.empty())
            return;

        std::nth_element(x_positions.begin(), x_positions.begin() + x_positions.size()/2, x_positions.end());
        auto median_x = x_positions[x_positions.size()/2];
        //median_x = std::min(area.max_corner().x().value(), std::max(area.min_corner().x().value(), median_x));
        std::nth_element(y_positions.begin(), y_positions.begin() + y_positions.size()/2, y_positions.end());
        auto median_y = y_positions[y_positions.size()/2];
        //median_y = std::min(area.max_corner().y().value(), std::max(area.min_corner().y().value(), median_y));

        point_type median_point {unit_type(median_x), unit_type(median_y)};
        
        auto nearest_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(median_point, 0);

        auto cell_location = placement.location(cell);
        auto current_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(cell_location, 0);

        if(current_gcell != nearest_gcell){
            auto center_gcell = m_design.global_routing().gcell_graph()->center_of_box(nearest_gcell);
            // creating candidate
            auto cell_name = netlist.name(cell);
            create_movement_candidate(cell, candidate_origin_type::MEDIAN, center_gcell, cell_nets, cell_name + "_median", model);
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::create_2_pin_nets_candidates_with_movements(const net_type net, model_type & model){
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        auto candidate_type =  candidate_origin_type::TWO_PIN_NET;

        auto net_name = netlist.name(net);
        std::vector<ophidian::circuit::PinInstance> pins (netlist.pins(net).begin(), netlist.pins(net).end());

        auto pin_a = pins[0];
        cell_type cell_a;
        std::string cell_a_name;
        point_type cell_a_pos;
        bool cell_a_fixed;
        std::vector<net_type> nets_of_cell_a;
        if(netlist.is_pad(pin_a)){
            cell_a_name = netlist.name(pin_a);
            cell_a_pos = placement.location(pin_a);
            cell_a_fixed = true;
        }else{
            cell_a = netlist.cell(pin_a);
            cell_a_name = netlist.name(cell_a);
            cell_a_pos = placement.location(cell_a);
            cell_a_fixed = placement.isFixed(cell_a);
            for(auto pin : netlist.pins(cell_a)){
                auto net = netlist.net(pin);
                nets_of_cell_a.push_back(net);
            }
        }
        
        auto pin_b = pins[1];
        cell_type cell_b;
        std::string cell_b_name;
        point_type cell_b_pos;
        bool cell_b_fixed;
        std::vector<net_type> nets_of_cell_b;
        if(netlist.is_pad(pin_b)){
            cell_b_name = netlist.name(pin_b);
            cell_b_pos = placement.location(pin_b);
            cell_b_fixed = true;
        }else{
            cell_b = netlist.cell(pin_b);
            cell_b_name = netlist.name(cell_b);
            cell_b_pos = placement.location(cell_b);
            cell_b_fixed = placement.isFixed(cell_b);
            for(auto pin : netlist.pins(cell_b)){
                auto net = netlist.net(pin);
                nets_of_cell_b.push_back(net);
            }
        }
        // WARNING!!
        // using possition insteag GCEll
        // Will NOT WORK with ICCAD2019 circuits!!
        unit_type candidate_wirelength;
        if(cell_a_pos.x() == cell_b_pos.x() || cell_a_pos.y() == cell_b_pos.y()){
            //same collum or same row
            if(!cell_b_fixed)
                create_movement_candidate(cell_b, candidate_type, cell_a_pos, nets_of_cell_b, cell_b_name + "_to_" + cell_a_name + "_row_" + net_name, model);
            
            if(!cell_a_fixed)        
                create_movement_candidate(cell_a, candidate_type, cell_b_pos, nets_of_cell_a, cell_a_name + "_to_" + cell_b_name + "_row_" + net_name, model);

        }else if (cell_a_pos.x() != cell_b_pos.x() && cell_a_pos.y() != cell_b_pos.y()){
            // different row and column
            point_type new_position;

            if(!cell_b_fixed){
                //change B to A's row
                new_position = point_type(cell_b_pos.x(), cell_a_pos.y());
                create_movement_candidate(cell_b, candidate_type, new_position, nets_of_cell_b, cell_b_name + "_to_" + cell_a_name + "_row_" + net_name, model);

                //change B to A's column
                new_position = point_type(cell_a_pos.x(), cell_b_pos.y());
                create_movement_candidate(cell_b, candidate_type, new_position, nets_of_cell_b, cell_b_name + "_to_" + cell_a_name + "_column_" + net_name, model);
            }

            if(!cell_a_fixed){
                //change A to B's row
                new_position = point_type(cell_a_pos.x(), cell_b_pos.y());
                create_movement_candidate(cell_a, candidate_type, new_position, nets_of_cell_a, cell_a_name + "_to_" + cell_b_name + "_row_" + net_name, model);

                //change A to B's column
                new_position = point_type(cell_b_pos.x(), cell_a_pos.y());
                create_movement_candidate(cell_a, candidate_type, new_position, nets_of_cell_a, cell_a_name + "_to_" + cell_b_name + "_column_" + net_name, model);
            }
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::create_movement_candidate(const cell_type & cell, const candidate_origin_type type, const point_type& new_position, const std::vector<net_type>& nets, std::string variable_name, model_type & model )
    {
        auto & netlist = m_design.netlist();
        auto & placement = m_design.placement();
        auto & routing_library = m_design.routing_library();        
        
        auto initial_position = placement.location(cell);
        auto stdCell = netlist.std_cell(cell);
        auto & stdCellGeometry = m_design.placement_library().geometry(stdCell);
        auto height = stdCellGeometry.height();
        auto width = stdCellGeometry.width();
        auto chipUpperRigthCorner = m_design.floorplan().chip_upper_right_corner();

        auto new_x = new_position.x();
        auto new_y = new_position.y();
        if(new_position.x() + width > chipUpperRigthCorner.x())
        {
            new_x = chipUpperRigthCorner.x() - width;
        }
        if(new_position.y() + height > chipUpperRigthCorner.y())
        {
            new_y = chipUpperRigthCorner.y() - height;
        }
        point_type candidate_position {new_x, new_y};

        auto candidate = m_position_candidates.add();
        auto candidate_variable_name = variable_name;
        var_type candidate_variable(m_env, 0.0, 1.0, candidate_variable_name.c_str());

        m_position_candidate_names[candidate] = candidate_variable_name;
        m_position_candidate_cell[candidate] = cell;
        m_name_to_position_candidate[candidate_variable_name] = candidate;
        m_position_candidate_variables[candidate] = candidate_variable;
        m_position_candidate_position[candidate] = candidate_position;
        m_cell_position_candidates.addAssociation(cell, candidate);
        m_position_candidate_origin[candidate] = type;

        if (DEBUG) std::cout << "create candidate movement " << candidate_variable_name << std::endl; 

        // place cell in the new position
        placement.place(cell, candidate_position);

        //genarate route for every net with new location
        for(auto net : nets) {
            if (DEBUG) std::cout << "net for candidate movement " << m_design.netlist().name(net) << std::endl;
            auto min_layer = routing_library.lowest_layer();
            auto max_layer = routing_library.highest_layer();
            generate_routes_of_net(net, candidate, model, std::make_pair(min_layer, max_layer));
        }
        // restore original position
        placement.place(cell, initial_position);
    }

    template <typename var_type>
    void ILPRouting<var_type>::generate_routes_of_net(const net_type & net, const position_candidate_type pos_candidate , model_type & model, std::pair<layer_type, layer_type> layer_range)
    {

        auto& routing_constraints = m_design.routing_constraints();
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        auto net_pins = netlist.pins(net);
        std::vector<interconnection::Flute::Point> net_points;
        net_points.reserve(net_pins.size());
        std::map<std::pair<int, int>, pin_container_type> net_points_map;

        auto moved_cell = (pos_candidate != position_candidate_type{}) ? m_position_candidate_cell[pos_candidate] : cell_type{};

        if(DEBUG) std::cout << "net name: " << netlist.name(net) << std::endl;
        if(DEBUG) std::cout << "net pins size " << net_pins.size() << std::endl;
        for(auto pin : net_pins)
        {
            auto pin_name = netlist.name(pin);
            auto pin_location = placement.location(pin);
            auto pin_cell = netlist.cell(pin);
            if (pin_cell != cell_type{} && pin_cell == moved_cell) {
                auto original_cell_location = m_design.placement().location(pin_cell);
                auto moved_cell_location = m_position_candidate_position[pos_candidate];                
                auto translation = point_type{moved_cell_location.x() - original_cell_location.x(), moved_cell_location.y() - original_cell_location.y()};
                pin_location = point_type{pin_location.x() + translation.x(), pin_location.y() + translation.y()};
            }
            if (DEBUG) std::cout << "pin " << pin_name << " location " << pin_location.x().value() << ", " << pin_location.y().value() << std::endl;
            net_points.push_back(pin_location);
            auto point = std::make_pair(std::round(pin_location.x().value()), std::round(pin_location.y().value()));
            if (DEBUG) std::cout << "point " << point.first << "," << point.second << std::endl;
            // auto std_pin = netlist.std_cell_pin(pin);
            net_points_map[point].push_back(pin);
        }

        // point_container_type flutePoints;
        // flutePoints.reserve(net_points.size());
        // convert_to_flute(flutePoints, net_points);

        if(DEBUG) std::cout << "before flute instance" << std::endl;
        auto & flute = interconnection::Flute::instance();
        if(DEBUG) std::cout << "before call flute" << std::endl;
        auto tree = flute.create(net_points);
        // auto tree = flute.create(flutePoints);
        if(DEBUG) std::cout << "after call flute" << std::endl;

        auto number_of_segments = tree->size(interconnection::SteinerTree::Segment());
        auto steiner_tree_length = tree->length().value();
        std::set<std::pair<unit_type, unit_type>> net_steiner_points;
        std::vector<segment_type> segments;
        if (DEBUG) std::cout << "tree size " << number_of_segments << std::endl;
        if (DEBUG) std::cout << "tree length " << tree->length().value() << std::endl;
        if(steiner_tree_length == 0)
        {
            if(DEBUG) std::cout << "same gcell" << std::endl;
            // this happens when all pins are in the same gcell
            //auto segment = m_segments.add();
            auto segment = segment_type{};
            auto point = net_points.at(0);
            // auto point = convert_to_design(flutePoints.at(0));
            //m_segment_starts[segment] = point;
            //m_segment_ends[segment] = point;
            segment.start = point;
            segment.end = point;
            auto point_pair = std::make_pair(std::round(point.x().value()), std::round(point.y().value()));

            // std::cout << "printing map in same gcell" << std::endl;
            // for(auto k : net_points_map)
            // {
            //     std::cout << "key: " << k.first.first << " , " << k.first.second << std::endl;
            // }
            // std::cout << "acessing with : " << point_pair.first << " , " << point_pair.second << std::endl;


            //m_segment_start_pin[segment] = net_points_map.at(point_pair);
            //m_segment_end_pin[segment] = net_points_map.at(point_pair);
            segment.start_pins = net_points_map.at(point_pair);
            segment.end_pins = net_points_map.at(point_pair);
            segments.push_back(segment);
        }
        else
        {
            if(DEBUG) std::cout << "more tha one gcell" << std::endl;
      	    for(auto tree_segment_it = tree->segments().first; tree_segment_it != tree->segments().second; ++tree_segment_it)
            {
                auto tree_segment = *tree_segment_it;
                //auto segment = m_segments.add();
                auto segment = segment_type{};

                auto segment_start = tree->position(tree->u(tree_segment));
                auto segment_end = tree->position(tree->v(tree_segment));

                // auto segment_start_double = tree->position(tree->u(tree_segment));
                // auto segment_end_double = tree->position(tree->v(tree_segment));
                // auto segment_start = convert_to_design(segment_start_double);
                // auto segment_end = convert_to_design(segment_end_double);
                
                if (DEBUG) std::cout << "tree segment " << segment_start.x().value() << "," << segment_start.y().value() << "->" << segment_end.x().value() << "," << segment_end.y().value() << std::endl;               


                //m_segment_starts[segment] = segment_start;
                //m_segment_ends[segment] = segment_end;
                segment.start = segment_start;
                segment.end = segment_end;

                // for(auto k : net_points_map)
                // {
                //     std::cout << "key: " << k.first.first << " , " << k.first.second << std::endl;
                // }

                auto point = std::make_pair(std::round(segment_start.x().value()), std::round(segment_start.y().value()));
                if (DEBUG) std::cout << "point " << point.first << "," << point.second << std::endl;
                if(net_points_map.count(point))
                {
                    // std::cout << "acessing with : " << point.first << " , " << point.second << std::endl;
                    //m_segment_start_pin[segment] = net_points_map.at(point);
                    segment.start_pins = net_points_map.at(point);
                    if (DEBUG) std::cout << "start is pin" << std::endl;
                }
                else
                {
                    net_steiner_points.insert(std::make_pair(segment_start.x(), segment_start.y()));
                    if (DEBUG) std::cout << "start is steiner point" << std::endl;
                }
                point = std::make_pair(std::round(segment_end.x().value()), std::round(segment_end.y().value()));
                if(net_points_map.count(point))
                {
                    // std::cout << "acessing with : " << point.first << " , " << point.second << std::endl;
                    //m_segment_end_pin[segment] = net_points_map.at(point);
                    segment.end_pins = net_points_map.at(point);
                    if (DEBUG) std::cout << "end is pin" << std::endl;
                }else
                {
                    net_steiner_points.insert(std::make_pair(segment_end.x(), segment_end.y()));
                    if (DEBUG) std::cout << "end is steiner point" << std::endl;
                }
                segments.push_back(segment);
	        }
        }
        segments.shrink_to_fit();

        if(DEBUG) std::cout << "creating vectors layers" << std::endl;
    	std::vector<layer_type> horizontal_layers;
	    std::vector<layer_type> vertical_layers;

        auto & routing_library = m_design.routing_library();
        auto min_idx = routing_library.index(layer_range.first);
        auto max_idx = routing_library.index(layer_range.second);
    	// for(auto layer_it = m_design.routing_library().begin_layer(); layer_it != m_design.routing_library().end_layer(); layer_it++)
        for(auto layer_idx = min_idx; layer_idx <= max_idx; layer_idx++)
        {
	        // auto layer = *layer_it;
            auto layer = routing_library.layer_from_index(layer_idx);
            auto layer_name = m_design.routing_library().name(layer);
    	    auto layer_direction = m_design.routing_library().direction(layer);
	        if(layer_direction == layer_direction_type::HORIZONTAL)
            {
        		horizontal_layers.push_back(layer);
    	    }
	        if(layer_direction == layer_direction_type::VERTICAL)
            {
        		vertical_layers.push_back(layer);
    	    }
    	}

        if(DEBUG) std::cout << "net candidates in layers" << std::endl;
        bool has_horizontal_segment = true;
        bool has_vertical_segment = true;
        /*for (auto segment : segments) {
            auto segment_start = m_segment_starts[segment];
            auto segment_end = m_segment_ends[segment];

            if (segment_start.x().value() != segment_end.x().value()) {
                has_horizontal_segment = true;                
            }
            if (segment_start.y().value() != segment_end.y().value()) {
                has_vertical_segment = true;
            }
        }*/

    	auto large_net = net_points.size() > 5;        
        auto min_layer = routing_constraints.min_net_layer(net);
        auto min_layer_index = m_design.routing_library().layerIndex(min_layer);
        bool created_net_candidate = false;
    	for(auto horizontal_layer : horizontal_layers)
        {
            auto horizontal_layer_index = m_design.routing_library().layerIndex(horizontal_layer);
            if(has_horizontal_segment && horizontal_layer_index < min_layer_index)
            {
                continue;
            }
	        for(auto vertical_layer : vertical_layers)
            {
                auto vertical_layer_index = m_design.routing_library().layerIndex(vertical_layer);
                if(has_vertical_segment && vertical_layer_index < min_layer_index || (horizontal_layer_index < min_layer_index && vertical_layer_index < min_layer_index))
                {
                    continue;
                }                
                if(std::abs(horizontal_layer_index - vertical_layer_index) > 1)
                {
                    continue;
                }

                created_net_candidate = true;
                
                if(DEBUG) std::cout << "before candidates in layers" << std::endl;
            	create_net_candidates_in_layers(net, segments, horizontal_layer, vertical_layer, large_net, net_steiner_points, pos_candidate, model);
                if(DEBUG) std::cout << "after candidates in layers" << std::endl;
    	    }
	    }

        if (!created_net_candidate && pos_candidate != position_candidate_type()) {
            auto position_candidate_variable = m_position_candidate_variables[pos_candidate];
            auto position_candidate_name = m_position_candidate_names[pos_candidate];
            auto constraint = model.add(position_candidate_variable == 0);
            auto constraint_name = position_candidate_name + "_impossible";
            constraint.setName(constraint_name.c_str());
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::convert_to_flute(point_container_type & converted, const point_container_type & points) const
    {
        if(DEBUG) std::cout << "Converting to flute" << std::endl;
        using dbu = ILPRouting<var_type>::unit_type;
        auto origin = m_design.floorplan().chip_origin();
        auto upper_right_corner = m_design.floorplan().chip_upper_right_corner();

        converted.clear();
        converted.reserve(points.size());

        if(upper_right_corner.x() > dbu(20000) || upper_right_corner.y() > dbu(20000))
        {
            if(DEBUG) std::cout << "converting..." << std::endl;
            auto step_x = (upper_right_corner.x().value() - origin.x().value()) / 2000.0;
            auto step_y = (upper_right_corner.y().value() - origin.y().value()) / 2000.0;

            for(auto p : points){
                auto new_x = (p.x().value() - origin.x().value())/step_x;
                auto new_y = (p.y().value() - origin.y().value())/step_y;
                auto new_point = point_type(dbu(new_x), dbu(new_y));
                converted.push_back(new_point);
            }
        }else{
            std::copy(points.begin(), points.end(), std::back_inserter(converted));
        }
    }

    template <typename var_type>
    typename ILPRouting<var_type>::point_type ILPRouting<var_type>::convert_to_design(point_type & point) const
    {
        if(DEBUG) std::cout << "converting to design" << std::endl;
        using dbu = ILPRouting<var_type>::unit_type;
        auto origin = m_design.floorplan().chip_origin();
        auto upper_right_corner =  m_design.floorplan().chip_upper_right_corner();
     
        if(upper_right_corner.x() > dbu(20000) || upper_right_corner.y() > dbu(20000))
        {
            if(DEBUG) std::cout << "converting..." << std::endl;
            auto step_x = (upper_right_corner.x().value() - origin.x().value()) / 2000.0;
            auto step_y = (upper_right_corner.y().value() - origin.y().value()) / 2000.0;

            auto new_x = (point.x().value() * step_x) + origin.x().value();
            auto new_y = (point.y().value() * step_y) + origin.y().value();
            
            return point_type(dbu(new_x), dbu(new_y));
        }
        return point;
    }



    template <typename var_type>
    void ILPRouting<var_type>::create_net_candidates_in_layers(const net_type & net, const std::vector<segment_type> & segments, const layer_type & horizontal_layer, const layer_type & vertical_layer, bool large_net, const std::set<std::pair<unit_type, unit_type>> & steiner_points, const position_candidate_type pos_candidate, model_type & model)
    {
        if(DEBUG) std::cout << "initi function: create_net_candidates_in_layers" << std::endl;
        auto& placement = m_design.placement();
        auto& routing_library = m_design.routing_library();
        environment_type env = model.getEnv();

        auto net_name = m_design.netlist().name(net);
        auto horizontal_layer_name = m_design.routing_library().name(horizontal_layer);
        auto vertical_layer_name = m_design.routing_library().name(vertical_layer);

        if(DEBUG) std::cout << "creating candidates for net " << net_name << " in layers " << horizontal_layer_name << " and " << vertical_layer_name << std::endl;

        unsigned number_of_candidates = 1;
    	if(large_net)
        {
	        number_of_candidates = 1;
    	}
        else
        {
	    for(auto segment : segments)
            {
                //auto segment_start = m_segment_starts[segment];
                //auto segment_end = m_segment_ends[segment];
                //if(segment_start.x() != segment_end.x() && segment_start.y() != segment_end.y())
                if(segment.start.x() != segment.end.x() && segment.start.y() != segment.end.y())
                {
                    number_of_candidates *= 2;
                }
            }
    	}

        if(DEBUG) std::cout << "number of candidates " << number_of_candidates << std::endl;

        std::vector<route_candidate_type> candidates;
        candidates.reserve(number_of_candidates);
        std::string pos_candidate_str = "";
        if(pos_candidate != position_candidate_type())
            pos_candidate_str = "_" + m_position_candidate_names[pos_candidate];
        for(auto candidate_index = 0; candidate_index < number_of_candidates; candidate_index++)
        {
            auto variable_name = net_name + "_" + horizontal_layer_name + "_" + vertical_layer_name + "_" + std::to_string(candidate_index) + pos_candidate_str;

            while(m_name_to_route_candidate.count(variable_name)>0)
            {
                number_of_candidates ++;
                candidate_index ++;
                variable_name = net_name + "_" + horizontal_layer_name + "_" + vertical_layer_name + "_" + std::to_string(candidate_index) + pos_candidate_str;
            }

            auto candidate = m_route_candidate.add();
            m_route_candidate_nets[candidate] = net;
            if(DEBUG) std::cout << "variable " << variable_name << std::endl;

            var_type variable(env, 0.0, 1.0, variable_name.c_str());
            m_route_candidate_names[candidate] = variable_name;
            m_name_to_route_candidate[variable_name] = candidate;
            m_route_candidate_variables[candidate] = variable;
            candidates.push_back(candidate);

            if(pos_candidate != position_candidate_type()) {
                auto position_candidate_name = m_position_candidate_names[pos_candidate];
                m_position_candidate_to_routes.addAssociation(pos_candidate, candidate);
            }
            // else
            // {
            if(DEBUG)  std::cout << "before else" << std::endl;
            m_net_candidates.addAssociation(net, candidate);
            if(DEBUG)  std::cout << "after else" << std::endl;
            // }

        }

        if(DEBUG)  std::cout << "before for segments" << std::endl;

        unsigned branch_count = 0;
        for(auto & segment : segments)
        {
            //auto segment_start = m_segment_starts[segment];
            //auto segment_end = m_segment_ends[segment];
            auto & segment_start = segment.start;
            auto & segment_end = segment.end;
            if(DEBUG) std::cout << "segment " << segment_start.x().value() << ", " << segment_start.y().value() << "->"
                    << segment_end.x().value() << ", " << segment_end.y().value() << std::endl;
            if(segment_start.x() != segment_end.x() && segment_start.y() != segment_end.y())
            {
                if(DEBUG) std::cout << "split segment" << std::endl;
                add_wires_of_splitted_segment(segment, segment_start, segment_end, horizontal_layer, vertical_layer,  true, branch_count, candidates, large_net);
                if(!large_net)
                {
                    add_wires_of_splitted_segment(segment, segment_start, segment_end, horizontal_layer, vertical_layer, false, branch_count, candidates, large_net);
                }
                branch_count++;
            }
            else
            {
                // TODO: if both x and y are the same, should route in the layer closest to the pins
                auto layer = (segment_start.y() == segment_end.y()) ? horizontal_layer : vertical_layer;
                //std::cout << "create segment wire" << std::endl;
                auto wire = create_wire(segment_start, segment_end, layer, layer);
                auto wires = wire_container_type{wire};
                //auto segment_start_pins = m_segment_start_pin[segment];
                auto & segment_start_pins = segment.start_pins;
                for(auto & segment_start_pin : segment_start_pins)
                {
            auto pin_name = m_design.netlist().name(segment_start_pin);
            if (DEBUG) std::cout << "segment start pin " << pin_name << std::endl;

                    auto pin_geometry = placement.geometry(segment_start_pin);
                    auto layer_name = pin_geometry.front().second;
                    auto pin_layer = routing_library.find_layer_instance(layer_name);
                    if(pin_layer != layer)
                    {
                if (DEBUG) std::cout << "add via" << std::endl;
                        auto start_via = create_wire(segment_start, segment_start, pin_layer, layer);
                        wires.push_back(start_via);
                    }
                }
                //auto segment_end_pins = m_segment_end_pin[segment];
                auto & segment_end_pins = segment.end_pins;
                for(auto & segment_end_pin : segment_end_pins)
                {
            auto pin_name = m_design.netlist().name(segment_end_pin);
            if (DEBUG) std::cout << "segment end pin " << pin_name << std::endl;

                    auto pin_geometry = placement.geometry(segment_end_pin);
                    auto layer_name = pin_geometry.front().second;
                    auto pin_layer = routing_library.find_layer_instance(layer_name);
                    if(pin_layer != layer)
                    {
                if (DEBUG) std::cout << "add via" << std::endl;
                        auto end_via = create_wire(segment_end, segment_end, layer, pin_layer);
                        wires.push_back(end_via);
                    }
                }
                for(auto candidate : candidates)
                {
                    add_wires_to_candidate(candidate, wires);
                }
            }
        }
        if(DEBUG)  std::cout << "before for steiner points" << std::endl; 
        for(auto & steiner_point : steiner_points)
        {
            auto point = point_type{steiner_point.first, steiner_point.second};
            auto via = create_wire(point, point, horizontal_layer, vertical_layer);
            auto wires = wire_container_type{via};
            for(auto candidate : candidates)
            {
                add_wires_to_candidate(candidate, wires);
            }
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::add_wires_of_splitted_segment(const segment_type & segment, const point_type & segment_start, const point_type & segment_end, const layer_type & horizontal_layer, const layer_type & vertical_layer, bool connect_on_y, unsigned branch_count, const std::vector<route_candidate_type> & candidates, bool large_net)
    {
        if(DEBUG)  std::cout << "init function: add_wires_of_splitted_segment" << std::endl;
        auto& placement = m_design.placement();
        auto& routing_library = m_design.routing_library();

        auto layer1 = (connect_on_y) ? vertical_layer : horizontal_layer;
        auto layer2 = (connect_on_y) ? horizontal_layer : vertical_layer;
        wire_container_type wires;
        split_segment(segment_start, segment_end, layer1, layer2, connect_on_y, wires);
        //auto segment_start_pins = m_segment_start_pin[segment];
        auto & segment_start_pins = segment.start_pins;
        for(auto & segment_start_pin : segment_start_pins)
        {
            auto pin_name = m_design.netlist().name(segment_start_pin);
            if (DEBUG) std::cout << "segment start pin " << pin_name << std::endl;

            auto pin_geometry = placement.geometry(segment_start_pin);
            auto layer_name = pin_geometry.front().second;
            auto pin_layer = routing_library.find_layer_instance(layer_name);
            if(pin_layer != layer1)
            {
                if (DEBUG) std::cout << "add via" << std::endl;
                auto start_via = create_wire(segment_start, segment_start, pin_layer, layer1);
                wires.push_back(start_via);
            }
        }
        //auto segment_end_pins = m_segment_end_pin[segment];
        auto & segment_end_pins = segment.end_pins;
        for(auto & segment_end_pin : segment_end_pins)
        {
            auto pin_name = m_design.netlist().name(segment_end_pin);
            if (DEBUG) std::cout << "segment end pin " << pin_name << std::endl;

            auto pin_geometry = placement.geometry(segment_end_pin);
            auto layer_name = pin_geometry.front().second;
            auto pin_layer = routing_library.find_layer_instance(layer_name);
            if(pin_layer != layer2)
            {
                if (DEBUG) std::cout << "add via" << std::endl;
                auto end_via = create_wire(segment_end, segment_end, layer2, pin_layer);
                wires.push_back(end_via);
            }
        }

    	if(large_net)
        {
	        for(auto candidate : candidates)
            {
    	    	add_wires_to_candidate(candidate, wires);
            }
    	}
        else
        {
            auto candidates_per_step = std::pow(2, branch_count);
            auto step = candidates_per_step*2;
            unsigned candidate_index = (connect_on_y) ? 0 : candidates_per_step;
            //std::cout << "branch count " << branch_count << " candidates per step " << candidates_per_step << " step " << step << " first index " << candidate_index << std::endl;
            while(candidate_index < candidates.size())
            {
                for(auto i=candidate_index; i<candidate_index+candidates_per_step; i++)
                {
                    auto candidate = candidates.at(i);
                    add_wires_to_candidate(candidate, wires);
                    //std::cout << "candidate " << i << " number of wires " << m_route_candidate_wires[candidate].size() << std::endl;
                }
                candidate_index += step;
            }
    	}
    }

    template <typename var_type>
    void ILPRouting<var_type>::add_wires_to_candidate(const route_candidate_type & candidate, const wire_container_type & wires)
    {
        auto wirelength = unit_type{0};
        for(auto wire : wires)
        {
            //m_route_candidate_wires[candidate].push_back(wire);
            m_route_candidate_wires.addAssociation(candidate, wire);
            auto wire_start = m_wire_starts[wire];
            auto wire_end = m_wire_ends[wire];
            wirelength += unit_type{std::abs(wire_end.x().value() - wire_start.x().value()) / 10.0 + std::abs(wire_end.y().value() - wire_start.y().value()) / 10.0};
            auto start_layer_index = m_wire_start_layers[wire];
            auto end_layer_index = m_wire_end_layers[wire];
            if(start_layer_index != end_layer_index)
            {
                double abs = std::abs(start_layer_index - end_layer_index);
                wirelength += unit_type{abs};
            }

            //std::cout << "adding wire " << wire_start.x().value() << " " << wire_start.y().value() << " " << start_layer_index << " " << wire_end.x().value() << " " << wire_end.y().value() << " " << end_layer_index << std::endl;
        }
    	//m_route_candidate_wires[candidate].shrink_to_fit();
        m_route_candidate_wirelengths[candidate] += wirelength;
    }

    template <typename var_type>
    void ILPRouting<var_type>::split_segment(const point_type & segment_start, const point_type & segment_end, const layer_type & wire1_layer, const layer_type & wire2_layer, bool connect_on_y, wire_container_type & wires) {
        auto wire1_start = segment_start;
        auto wire1_end = (connect_on_y) ? point_type{segment_start.x(), segment_end.y()} : point_type{segment_end.x(), segment_start.y()};
        auto wire1 = create_wire(wire1_start, wire1_end, wire1_layer, wire1_layer);
        wires.push_back(wire1);

        auto wire2_start = wire1_end;
        auto wire2_end = segment_end;
        auto wire2 = create_wire(wire2_start, wire2_end, wire2_layer, wire2_layer);
        wires.push_back(wire2);

        auto via = create_wire(wire1_end, wire2_start, wire1_layer, wire2_layer);
        wires.push_back(via);
    }

    template <typename var_type>
    typename ILPRouting<var_type>::wire_type ILPRouting<var_type>::create_wire(const point_type & wire_start, const point_type & wire_end, const layer_type & start_layer, const layer_type & end_layer)
    {
        auto wire = m_wires.add();
        m_wire_starts[wire] = wire_start;
        m_wire_ends[wire] = wire_end;

        /*auto min_x = std::min(wire_start.x().value(), wire_end.x().value());
        auto max_x = std::max(wire_start.x().value(), wire_end.x().value());
        auto min_y = std::min(wire_start.y().value(), wire_end.y().value());
        auto max_y = std::max(wire_start.y().value(), wire_end.y().value());

        auto wire_box = geometry::Box<double>{{min_x, min_y}, {max_x, max_y}};

        auto start_layer_index = m_design.routing_library().layerIndex(start_layer);
        auto end_layer_index = m_design.routing_library().layerIndex(end_layer);
        if (DEBUG) std::cout << "creating wire " << wire_box.min_corner().x() << " " << wire_box.min_corner().y() << " " << start_layer_index << " -> " << wire_box.max_corner().x() << " " << wire_box.max_corner().y() << " " << end_layer_index << std::endl;*/

        m_wire_start_layers[wire] = m_design.routing_library().layerIndex(start_layer);
        m_wire_end_layers[wire] = m_design.routing_library().layerIndex(end_layer);
        return wire;
    }

    template <typename var_type>
    void ILPRouting<var_type>::add_objective_function(model_type & model)
    {
        IloObjective obj = IloMinimize(m_env);
        unsigned candidate_count = 0;
        IloExpr expr(m_env);
        for(auto candidate : m_route_candidate)
        {
            gcell_container_type intersecting_gcells;
            find_gcells_intersecting_with_candidate(candidate, intersecting_gcells);

            auto candidate_variable = m_route_candidate_variables[candidate];
            //auto candidate_wirelength = m_route_candidate_wirelengths[candidate].value();
            auto candidate_wirelength = intersecting_gcells.size();

            expr += candidate_variable * (double)candidate_wirelength;
        }
        obj.setExpr(expr);
        model.add(obj);
    }

    template <typename var_type>
    void ILPRouting<var_type>::add_candidate_constraints(const std::vector<net_type> & nets, const cell_container_type & cells, model_type & model)
    {
        if (DEBUG) std::cout << "add net constraints" << std::endl;
        unsigned net_count = 0;
        for(auto net : nets)
        {
            IloExpr candidates_constraints(m_env);
            auto candidates = m_net_candidates.parts(net);
            for(auto candidate : candidates)
            {
                auto candidate_variable = m_route_candidate_variables[candidate];
                candidates_constraints += candidate_variable;
            }
            if (candidates.size()) {
                auto net_name = m_design.netlist().name(net);
                std::string constraint_name = net_name + "_candidates";
                auto constraint = model.add(candidates_constraints == 1);
                constraint.setName(constraint_name.c_str());
            }
        }

        if (DEBUG) std::cout << "add cell constraints" << std::endl;
        auto & netlist = m_design.netlist();
        //for (auto cell_it = m_design.netlist().begin_cell_instance(); cell_it != m_design.netlist().end_cell_instance(); cell_it++) {
        //    auto cell = *cell_it;
        for (auto & cell : cells) {
            auto cell_name = m_design.netlist().name(cell);
            if (DEBUG) std::cout << "cell " << cell_name << " pin " << m_design.netlist().pins(cell).size() << std::endl;
            
            std::vector<net_type> cell_nets;
            const auto & pins = m_design.netlist().pins(cell);
            for(auto pin : pins){
                //if (DEBUG) std::cout << "pin " << m_design.netlist().name(pin) << std::endl;
                cell_nets.push_back(m_design.netlist().net(pin));
            }

            auto cell_initial_position_candidate = m_cell_initial_candidate[cell];
            auto cell_initial_var = m_position_candidate_variables[cell_initial_position_candidate];

            /*for(auto net : cell_nets){
                GRBLinExpr candidates_constraints = 0.0;
                auto candidates = m_net_candidates.parts(net);
                for(auto candidate : candidates)
                {
                    auto candidate_variable = m_route_candidate_variables[candidate];
                    candidates_constraints += candidate_variable;
                }
                model.addConstr(candidates_constraints == cell_initial_var);
            }*/

            if (DEBUG) std::cout << "net to position constraints" << std::endl;
            auto position_candidates = m_cell_position_candidates.parts(cell);
            for(auto pos_candidate : position_candidates) {
                if (DEBUG) std::cout << "pos candidate " << m_position_candidate_names[pos_candidate] << std::endl;                
                std::unordered_map<std::string, IloExpr> cell_candidate_expressions;
                auto routes = m_position_candidate_to_routes.parts(pos_candidate);
                for(auto route : routes)
                {
                    auto candidate_variable = m_route_candidate_variables[route];
                    auto route_net = m_route_candidate_nets[route];
                    auto net_name = m_design.netlist().name(route_net);
                    if (DEBUG) std::cout << "net " << net_name << std::endl;
                    //cell_candidate_expressions[net_name] += candidate_variable;
                    if(cell_candidate_expressions.find(net_name) == cell_candidate_expressions.end())
                    {
                        //first time net
                        IloExpr exp(m_env);
                        cell_candidate_expressions.insert(std::make_pair(net_name, exp));
                    }
                    cell_candidate_expressions.at(net_name) = cell_candidate_expressions.at(net_name) + candidate_variable;
                }
                auto cell_position_variable = m_position_candidate_variables[pos_candidate];
                for (auto expression_pair_it = cell_candidate_expressions.begin(); expression_pair_it != cell_candidate_expressions.end(); expression_pair_it++) {
                    IloExpr expression(m_env);
                    expression = expression_pair_it->second;
                    auto position_candidate_name = m_position_candidate_names[pos_candidate];
                    std::string constraint_name = "routes_ass_cand_" + position_candidate_name + "_net_" + expression_pair_it->first;
                    auto constraint = model.add(expression == cell_position_variable);
                    constraint.setName(constraint_name.c_str());
                }
            }
            /*for(auto pos_candidate : position_candidates)
            {
                GRBLinExpr candidates_constraints = 0.0;
                auto cell_position_variable = m_position_candidate_variables[pos_candidate];
                auto routes = m_position_candidate_to_routes.parts(pos_candidate);
                for(auto route : routes)
                {
                    auto candidate_variable = m_route_candidate_variables[route];
                    candidates_constraints += candidate_variable;
                }
                model.addConstr(candidates_constraints == cell_position_variable);
            }*/
        }

        if (DEBUG) std::cout << "single position constraints" << std::endl;
        // std::unordered_set<net_type, entity_system::EntityBaseHash> nets;
        //for (auto cell_it = m_design.netlist().begin_cell_instance(); cell_it != m_design.netlist().end_cell_instance(); cell_it++) {
        //    auto cell = *cell_it;
        for (auto & cell : cells) {
            IloExpr candidates_constraints(m_env);
            
            auto initial_candidate = m_cell_initial_candidate[cell];
            auto initial_candidate_variable = m_position_candidate_variables[initial_candidate];
            candidates_constraints += initial_candidate_variable;

            auto candidates = m_cell_position_candidates.parts(cell);
            for (auto candidate : candidates) {
                auto candidate_variable = m_position_candidate_variables[candidate];
                candidates_constraints += candidate_variable;
            }

            auto cell_name = m_design.netlist().name(cell);
            std::string constraint_name = cell_name + "_positions_candidates";
            auto constraint = model.add(candidates_constraints == 1);
            constraint.setName(constraint_name.c_str());

            // for(auto pin : netlist.pins(cell)){
            //     auto net = netlist.net(pin);
            //     nets.insert(net);
            // }
        }

        if (DEBUG) std::cout << "neighbor constraints" << std::endl;
        //freeze neighbors
        for(auto net : nets){
            IloExpr neighbors_constraints(m_env);
            auto cont_candidates = 0;
            for(auto pin : netlist.pins(net)){
                if(netlist.is_pad(pin))
                    continue;
                auto cell = netlist.cell(pin);
                auto candidates = m_cell_position_candidates.parts(cell);
                for (auto candidate : candidates) {
                    auto candidate_variable = m_position_candidate_variables[candidate];
                    neighbors_constraints += candidate_variable;
                    cont_candidates ++;
                }
            }
            if(cont_candidates != 0)
            {
                auto net_name = m_design.netlist().name(net);
                std::string constraint_name = net_name + "_freeze_neighbors";
                auto constraint = model.add(neighbors_constraints <= 1);
                constraint.setName(constraint_name.c_str());
            }
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::find_gcells_intersecting_with_candidate(route_candidate_type & candidate, gcell_container_type & intersecting_gcells) {
        auto& global_routing = m_design.global_routing();
        auto gcell_graph = global_routing.gcell_graph();
        auto & routing_library = m_design.routing_library();

        auto & wires = m_route_candidate_wires.parts(candidate);
        for(auto wire : wires)
        {
            auto start_layer_index = m_wire_start_layers[wire];
            auto end_layer_index = m_wire_end_layers[wire];
            auto min_layer_index = std::min(start_layer_index, end_layer_index);
            auto max_layer_index = std::max(start_layer_index, end_layer_index);

            auto wire_start = m_wire_starts[wire];
            auto wire_end = m_wire_ends[wire];
            auto min_x = std::min(wire_start.x(), wire_end.x());
            auto max_x = std::max(wire_start.x(), wire_end.x());
            auto min_y = std::min(wire_start.y(), wire_end.y());
            auto max_y = std::max(wire_start.y(), wire_end.y());

            auto wire_box = box_type{{min_x, min_y}, {max_x, max_y}};

            // auto wire_box = box_type{m_wire_starts[wire], m_wire_ends[wire]};

            gcell_container_type base_gcells;
            gcell_graph->intersect(base_gcells, wire_box, min_layer_index-1);

            for(auto layer_index = min_layer_index; layer_index <= max_layer_index; layer_index++)
            {
                // auto layer_name = "M" + std::to_string(layer_index);
                auto layer = routing_library.layer_from_index(layer_index);
                auto layer_name = routing_library.name(layer);
                //gcell_container_type gcells;
                //gcell_graph->intersect(gcells, wire_box, layer_index-1);
                //std::cout << "gcells " << base_gcells.size() << std::endl;
                for(auto base_gcell : base_gcells)
                {
                    auto base_gcell_node = gcell_graph->graph_node(base_gcell);
                    auto base_gcell_position = gcell_graph->position(base_gcell_node);
                    auto gcell = gcell_graph->gcell(base_gcell_position.get<0>(), base_gcell_position.get<1>(), layer_index-1);
                    //gcell_nets[layer_name][gcell].insert(candidate);
                    //gcell_nets[gcell].insert(candidate);
                    //auto & candidates = gcell_nets[gcell];
                    //if (std::find(candidates.begin(), candidates.end(), candidate) == candidates.end()) {
                    if (std::find(intersecting_gcells.begin(), intersecting_gcells.end(), gcell) == intersecting_gcells.end()) {
                        //gcell_nets[gcell].push_back(candidate);
                        intersecting_gcells.push_back(gcell);
                    }
                }
            }
        }
    }

    template <typename var_type>
    void ILPRouting<var_type>::add_capacity_constraints(const std::vector<net_type> & nets, const cell_container_type & cells, const box_type & area, model_type & model)
    {
        auto& global_routing = m_design.global_routing();
        auto gcell_graph = global_routing.gcell_graph();
        auto & netlist = m_design.netlist();
        auto & routing_library = m_design.routing_library();

        unsigned number_of_gcells = 0;
        unsigned free_gcells = 0;

        gcell_container_type gcells;
        for (auto layer_index = routing_library.lowest_layer_index(); layer_index <= routing_library.highest_layer_index(); layer_index++) {
            gcell_graph->intersect(gcells, area, layer_index-1);
        }

        //std::unordered_map<gcell_type, int, entity_system::EntityBaseHash> gcells_max_demand;
        entity_system::Property<gcell_type, int>  gcells_max_demand{gcell_graph->make_property_gcells<int>(0)};

        //std::unordered_map<gcell_type, IloExpr, entity_system::EntityBaseHash> gcells_constraints;
        //std::unordered_map<gcell_type, bool, entity_system::EntityBaseHash> gcells_constraints_bool;
        entity_system::Property<gcell_type, IloExpr>  gcells_constraints{gcell_graph->make_property_gcells<IloExpr>()};
        entity_system::Property<gcell_type, bool>  gcells_constraints_bool{gcell_graph->make_property_gcells<bool>(false)};
        for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
            auto gcell = *gcell_it;
        //for(auto & gcell : gcells){
            gcells_constraints[gcell] = IloExpr(m_env);
            //gcells_constraints_bool[gcell] = false;

            //gcells_max_demand[gcell] = 0;

            number_of_gcells++;
        }
        
        
        if (DEBUG) std::cout << "add blockages demands" << std::endl;

        for(auto candidate_it = m_position_candidates.begin(); candidate_it != m_position_candidates.end(); candidate_it++)
        {
            auto candidate = *candidate_it;
            auto variable = m_position_candidate_variables[candidate];
            auto position = m_position_candidate_position[candidate];

            auto cell = m_position_candidate_cell[candidate];
            auto std_cell = netlist.std_cell(cell);
            auto blockages = routing_library.blockages(std_cell);
            for(auto blockage : blockages)
            {
                auto layer = routing_library.layer(blockage);
                auto layer_index = routing_library.layerIndex(layer);
                auto gcell = gcell_graph->nearest_gcell(position, layer_index-1);
                auto demand = routing_library.demand(blockage);

                gcells_constraints[gcell] += demand * variable;
                gcells_constraints_bool[gcell] = true;

                gcells_max_demand[gcell] += demand;
            }
        }

        if (DEBUG) std::cout << "find nets in gcells" << std::endl;
        
        //std::unordered_map<std::string, std::unordered_map<gcell_type, std::unordered_set<route_candidate_type, entity_system::EntityBaseHash>, entity_system::EntityBaseHash>> gcell_nets;
        //entity_system::Property<gcell_type, std::vector<route_candidate_type>> gcell_nets(gcell_graph->make_property_gcells<std::vector<route_candidate_type>>());
        for(auto net : nets)
        {
            auto net_name = m_design.netlist().name(net);
            //std::cout << "capacity constraints for net " << net_name << std::endl;
            auto candidates = m_net_candidates.parts(net);

            for(auto candidate : candidates)
            {
                auto & variable = m_route_candidate_variables[candidate];
                std::vector<gcell_type> intersecting_gcells;
                find_gcells_intersecting_with_candidate(candidate, intersecting_gcells);
                for (auto gcell : intersecting_gcells) {
                    gcells_constraints[gcell] += variable;
                    gcells_max_demand[gcell] += 1;
                    gcells_constraints_bool[gcell] = true;
                }
            }
        }
        /*if (DEBUG) std::cout << "add capacity constraints" << std::endl;
        for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++)
        {
            auto gcell = *gcell_it;
            auto & gcell_set = gcell_nets[gcell];
            auto box = gcell_graph->box(gcell);
            auto gcell_min_corner = box.min_corner();

            if(gcell_set.size() > 0)
            {
                for(auto & candidate : gcell_set)
                {
                    auto & variable = m_route_candidate_variables[candidate];
                    gcells_constraints[gcell] += variable;

                    gcells_max_demand[gcell] += 1;
                }
                gcells_constraints_bool[gcell] = true;
            }
        }*/


        if (DEBUG) std::cout << "build std cells per gcell" << std::endl;

        std::unordered_map<circuit::StandardCells::cell_type, std::unordered_set<routing::GCellGraph::gcell_type, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> gcells_per_std_cell;
        /*for (auto & cell : cells) {
            auto location = m_design.placement().location(cell);
            auto std_cell = m_design.netlist().std_cell(cell);

            auto cell_name = m_design.netlist().name(cell);
            auto std_cell_name = m_design.standard_cells().name(std_cell);

            for (auto layer_index = routing_library.lowest_layer_index(); layer_index <= routing_library.highest_layer_index(); layer_index++) {
                auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);

                //std::cout << "cell " << cell_name << " std cell " << std_cell_name << " location " << location.x().value() << "," << location.y().value() << "," << layer_index;
                //auto gcell_box = gcell_graph->box(gcell);
                //std::cout << " gcell " << gcell_box.min_corner().x().value() << "," << gcell_box.min_corner().y().value() << std::endl;

                gcells_per_std_cell[std_cell].insert(gcell);
            }
        }*/

        std::unordered_map<routing::GCellGraph::gcell_type, std::unordered_map<circuit::StandardCells::cell_type, var_container_type, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;
        for(auto pos_candidate : m_position_candidates){
            auto variable = m_position_candidate_variables[pos_candidate];
            auto location = m_position_candidate_position[pos_candidate];
            auto cell = m_position_candidate_cell[pos_candidate];
            auto std_cell = m_design.netlist().std_cell(cell);

            auto variable_name = m_position_candidate_names[pos_candidate];

            auto cell_name = m_design.netlist().name(cell);
            auto std_cell_name = m_design.standard_cells().name(std_cell);

            /*if (cell_name == "C1168" || cell_name == "C1166") {
                std::cout << "cell " << cell_name << " variable " << variable_name << " location " << location.x().value() << "," << location.y().value() << std::endl;
            }*/

            // for (auto layer_it = routing_library.begin_layer(); layer_it != routing_library.end_layer(); layer_it++) {
                // auto layer = *layer_it;
                // auto layer_index = routing_library.layerIndex(layer);
                // auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);
            for (auto layer_index = routing_library.lowest_layer_index(); layer_index <= routing_library.highest_layer_index(); layer_index++) {
                auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);

                /*if (cell_name == "C1168" || cell_name == "C1166") {
                    std::cout << "cell " << cell_name << " std cell " << std_cell_name << " location " << location.x().value() << "," << location.y().value() << "," << layer_index << std::endl;
                    auto gcell_box = gcell_graph->box(gcell);
                    auto gcell_node = gcell_graph->graph_node(gcell);
                    auto gcell_position = gcell_graph->position(gcell_node);
                    //std::cout << " gcell " << gcell_box.min_corner().x().value() << "," << gcell_box.min_corner().y().value() << std::endl;
                    std::cout << "gcell " << gcell_position.get<1>()+1 << "," << gcell_position.get<0>()+1 << "," << gcell_position.get<2>()+1 << std::endl;
                }*/

                std_cells_per_gcell[gcell][std_cell].push_back(variable);
                gcells_per_std_cell[std_cell].insert(gcell);
            }
        }

        
        auto & routing_constraints = m_design.routing_constraints();
        // same grid extra demand rules
        if (DEBUG) std::cout << "extra demand rules" << std::endl;

        for (auto same_grid_it = routing_constraints.begin_same_grid(); same_grid_it != routing_constraints.end_same_grid(); same_grid_it++) {
            auto key = same_grid_it->first;
            //auto demand = same_grid_it->second;
            auto & extra_demand = same_grid_it->second;
            auto demand = extra_demand.demand;
                
            auto macro1_name = extra_demand.macro1;
            auto macro2_name = extra_demand.macro2;
            auto layer_name = extra_demand.layer;

            auto macro1 = m_design.standard_cells().find_cell(macro1_name);
            auto macro2 = m_design.standard_cells().find_cell(macro2_name);
            auto layer = m_design.routing_library().find_layer_instance(layer_name);

            auto gcells_macro1 = gcells_per_std_cell[macro1];
            auto gcells_macro2 = gcells_per_std_cell[macro2];

            auto rule_gcells = std::unordered_set<gcell_type, entity_system::EntityBaseHash>{};
            for (auto gcell : gcells_macro1) {
                rule_gcells.insert(gcell);
            }
            for (auto gcell : gcells_macro2) {
                rule_gcells.insert(gcell);
            }
    
            for (auto gcell : rule_gcells) {
                auto gcell_layer_index = gcell_graph->layer_index(gcell);
                auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
                auto gcell_layer = m_design.routing_library().find_layer_instance(gcell_layer_name);

                if (layer == gcell_layer) {
                    auto & std_cells1 = std_cells_per_gcell[gcell][macro1];
                    auto & std_cells2 = std_cells_per_gcell[gcell][macro2];

                    if (std_cells1.empty() || std_cells2.empty()) {
                        continue;
                    }
                    
                    auto sum_expr1 = IloExpr{m_env};
                    for (auto std_cell : std_cells1) {
                        sum_expr1 += std_cell;
                    }

                    auto sum_expr2 = IloExpr{m_env};                   
                    for (auto std_cell : std_cells2) {
                        sum_expr2 += std_cell;
                    }

                    auto min_var = IloMin(sum_expr1, sum_expr2);
                    gcells_constraints[gcell] += min_var * demand;
                    gcells_constraints_bool[gcell] = true;

                    gcells_max_demand[gcell] += demand;
                }
            }
        }

        for (auto adj_grid_it = routing_constraints.begin_adj_grid(); adj_grid_it != routing_constraints.end_adj_grid(); adj_grid_it++) {
            auto key = adj_grid_it->first;
            //auto demand = same_grid_it->second;
            auto & extra_demand = adj_grid_it->second;
            auto demand = extra_demand.demand;

            auto macro1_name = extra_demand.macro1;
            auto macro2_name = extra_demand.macro2;
            auto layer_name = extra_demand.layer;

            auto macro1 = m_design.standard_cells().find_cell(macro1_name);
            auto macro2 = m_design.standard_cells().find_cell(macro2_name);
            auto layer = m_design.routing_library().find_layer_instance(layer_name);
                    
            //std::cout << "rule " << macro1_name << "," << macro2_name << "," << layer_name << std::endl;

            auto gcells_macro1 = gcells_per_std_cell[macro1];
            auto gcells_macro2 = gcells_per_std_cell[macro2];

    	    auto rule_gcells = std::unordered_set<gcell_type, entity_system::EntityBaseHash>{};
            for (auto gcell : gcells_macro1) {
                rule_gcells.insert(gcell);
            }
            for (auto gcell : gcells_macro2) {
                rule_gcells.insert(gcell);
            }

            for (auto gcell : rule_gcells) {
                auto gcell_node = gcell_graph->graph_node(gcell);

                auto gcell_layer_index = gcell_graph->layer_index(gcell);
                auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
                auto gcell_layer = m_design.routing_library().find_layer_instance(gcell_layer_name);

                //auto gcell_position = gcell_graph->position(gcell_node);
                //std::cout << "gcell " << gcell_position.get<1>()+1 << "," << gcell_position.get<0>()+1 << "," << gcell_position.get<2>()+1 << std::endl;
                
                auto east_node = gcell_graph->east_node(gcell_node);
                auto east_gcell = gcell_graph->gcell(east_node);
                auto west_node = gcell_graph->west_node(gcell_node);
                auto west_gcell = gcell_graph->gcell(west_node);                

                if (layer == gcell_layer) {
                    auto & std_cells1 = std_cells_per_gcell[gcell][macro1];
                    auto & std_cells2 = std_cells_per_gcell[gcell][macro2];
                    
                    //std::cout << "std cells 1 " << std_cells1.size() << " std cells 2 " << std_cells2.size() << std::endl;

                    if (std_cells1.empty() && std_cells2.empty()) {
                        continue;
                    }

                    auto sum_expr1 = IloExpr{m_env}; 
                    for (auto std_cell : std_cells1) {
                        sum_expr1 += std_cell;
                    }

                    auto sum_expr2 = IloExpr{m_env};  
                    for (auto std_cell : std_cells2) {
                        sum_expr2 += std_cell;
                    }

                    if (macro1 == macro2) {
                        if (east_node != lemon::INVALID) {
                            auto & std_cells1_east = std_cells_per_gcell[east_gcell][macro1];

                            if (!std_cells1_east.empty()) {
                                auto sum_expr1_east = IloExpr{m_env};
                                for (auto std_cell : std_cells1_east) {
                                    sum_expr1_east += std_cell;
                                }

                                auto min_var = IloMin(sum_expr1, sum_expr1_east);
                                gcells_constraints[gcell] += min_var * demand;
                            
                                gcells_max_demand[gcell] += demand;
                            }
                        }
                        if (west_node != lemon::INVALID) {
                            auto & std_cells1_west = std_cells_per_gcell[west_gcell][macro1];

                            if (!std_cells1_west.empty()) {
                                auto sum_expr1_west = IloExpr{m_env};
                                for (auto std_cell : std_cells1_west) {
                                    sum_expr1_west += std_cell;
                                }

                                auto min_var = IloMin(sum_expr1, sum_expr1_west);
                                gcells_constraints[gcell] += min_var * demand;
                    
                                gcells_max_demand[gcell] += demand;
                            }
                        }
                    } else {
                        if (east_node != lemon::INVALID) {
                            auto & std_cells1_east = std_cells_per_gcell[east_gcell][macro1];
                            auto & std_cells2_east = std_cells_per_gcell[east_gcell][macro2];
                    
                            //std::cout << "east std cells 1 " << std_cells1_east.size() << " east std cells 2 " << std_cells2_east.size() << std::endl;

                            if (!(std_cells1_east.empty() && std_cells2_east.empty())) {
                                auto sum_expr1_east = IloExpr{m_env};
                                for (auto std_cell : std_cells1_east) {
                                    sum_expr1_east += std_cell;
                                }

                                auto sum_expr2_east = IloExpr{m_env};
                                for (auto std_cell : std_cells2_east) {
                                    sum_expr2_east += std_cell;
                                }

                                auto min_var = IloMin(sum_expr1, sum_expr2_east) + IloMin(sum_expr2, sum_expr1_east);
                                gcells_constraints[gcell] += min_var * demand;
                    
                                gcells_max_demand[gcell] += demand;
                            }
                        }
                        if (west_node != lemon::INVALID) {
                            auto & std_cells1_west = std_cells_per_gcell[west_gcell][macro1];
                            auto & std_cells2_west = std_cells_per_gcell[west_gcell][macro2];
                    
                            //std::cout << "west std cells 1 " << std_cells1_west.size() << " west std cells 2 " << std_cells2_west.size() << std::endl;

                            if (!(std_cells1_west.empty() && std_cells2_west.empty())) {
                                auto sum_expr1_west = IloExpr{m_env};
                                for (auto std_cell : std_cells1_west) {
                                    sum_expr1_west += std_cell;
                                }

                                auto sum_expr2_west = IloExpr{m_env};
                                for (auto std_cell : std_cells2_west) {
                                    sum_expr2_west += std_cell;
                                }
                            
                                auto min_var = IloMin(sum_expr1, sum_expr2_west) + IloMin(sum_expr2, sum_expr1_west);
                                gcells_constraints[gcell] += min_var * demand;
                    
                                gcells_max_demand[gcell] += demand;
                            }
                        }
                    }
                }
            }
        }

        //for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
            //auto gcell = *gcell_it;
        for (auto & gcell : gcells) {
            auto node = gcell_graph->graph_node(gcell);
            auto location = gcell_graph->position(node);

            auto & gcell_constraint = gcells_constraints[gcell];
            
            auto gcell_layer_index = gcell_graph->layer_index(gcell);

            auto gcell_box = gcell_graph->box(gcell);
                
            auto x = gcell_box.min_corner().x().value();
            auto y = gcell_box.min_corner().y().value();
            auto z = gcell_layer_index;

                
            auto capacity = gcell_graph->capacity(gcell);
            auto fixed_demand = m_gcells_demand[gcell];
            auto max_demand = gcells_max_demand[gcell];
            if (capacity - fixed_demand >= max_demand) {
                free_gcells++;
            }
            
            //TIAGO
             if(gcells_constraints_bool[gcell] )
            //if(gcells_constraints_bool[gcell] && capacity - fixed_demand < max_demand)
            {
                auto constraint_name = std::to_string(location.get<1>()) + "_" + std::to_string(location.get<0>()) + "_" + std::to_string(location.get<2>()) ;
                auto demand = m_gcells_demand[gcell];
                auto extra_demand = m_gcells_extra_demand[gcell];
                //std::cout << "constraint " << constraint_name << " " << capacity << " " << demand << " " << extra_demand << std::endl;
                auto constraint = model.add(gcell_constraint <= capacity - demand - extra_demand);
                constraint.setName(constraint_name.c_str());                
            }
        }

        std::cout << "number of gcells " << number_of_gcells << std::endl;
        std::cout << "number of free gcells " << free_gcells << std::endl;        
    }

    template <typename var_type>
    void ILPRouting<var_type>::write_segments(const std::vector<net_type> & nets, const solver_type& solver, std::vector<net_type> & routed_nets, std::vector<net_type> & unrouted_nets)
    {
        auto & global_routing = m_design.global_routing();
        for(auto net : nets)
        {
    	    auto net_name = m_design.netlist().name(net);
            auto candidates = m_net_candidates.parts(net);
            bool routed = 0;
    	    route_candidate_type routed_candidate;
            for(auto candidate : candidates)
            {
                auto candidate_name = m_route_candidate_names[candidate];
                auto variable = m_route_candidate_variables[candidate];
                auto value = solver.getValue(variable);
                //std::cout << candidate_name << " " << value << std::endl;
    	    	//if(std::abs(value - 1.0) <= std::numeric_limits<double>::epsilon())
    	    	if(value > 0.9)
                {
                    routed = 1;
		            routed_candidate = candidate;
                    break;
        		}
            }
            if(routed)
            {
                routed_nets.push_back(net);
                //clear possible old routings
                global_routing.unroute(net);

        		//for(auto wire : m_route_candidate_wires[routed_candidate])
        		for(auto wire : m_route_candidate_wires.parts(routed_candidate))
                {
        		    auto wire_start = m_wire_starts[wire];
		            auto wire_end = m_wire_ends[wire];
        		    auto start_layer_index = m_wire_start_layers[wire];
		            auto end_layer_index = m_wire_end_layers[wire];
        		    auto wire_start_layer = m_design.routing_library().layer_from_index(start_layer_index);
		            auto wire_end_layer = m_design.routing_library().layer_from_index(end_layer_index);

                    auto min_x = std::min(wire_start.x(), wire_end.x());
                    auto max_x = std::max(wire_start.x(), wire_end.x());
                    auto min_y = std::min(wire_start.y(), wire_end.y());
                    auto max_y = std::max(wire_start.y(), wire_end.y());

                    auto wire_box = box_type{{min_x, min_y}, {max_x, max_y}};
                    global_routing.add_segment(wire_box, wire_start_layer, wire_end_layer, net);
		        }
                global_routing.increase_demand(net);
    	    }
            else
            {
                unrouted_nets.push_back(net);
                std::cout << "WARNING: net " << net_name << " unrouted" << std::endl;
            }
         }
    }

    template <typename var_type>
    void ILPRouting<var_type>::save_movements(const solver_type& solver, std::vector<std::pair<cell_type, point_type>> & movements) {
        int initial_candidate = 0;
        int two_pin = 0;
        int center_of_mass = 0;
        int median = 0;
        int na = 0;
        for (auto candidate_it = m_position_candidates.begin(); candidate_it != m_position_candidates.end(); candidate_it++) {
            auto candidate = *candidate_it;
            auto variable = m_position_candidate_variables[candidate];
            auto value = solver.getValue(variable);;
            std::string name (variable.getName());
            if (value > 0.9) {             
                if (name.find("initial") == std::string::npos) {
                    m_moved_cells++;
                    auto candidate = m_name_to_position_candidate[name];
                    auto cell = m_position_candidate_cell[candidate];
                    auto position = m_position_candidate_position[candidate];
                    movements.push_back(std::make_pair(cell, position));
                    m_design.placement().place(cell, position);
                    auto type = m_position_candidate_origin[candidate];
                    switch (type)
                    {
                    case candidate_origin_type::TWO_PIN_NET :
                        two_pin++;
                        break;
                    case candidate_origin_type::CENTER_OF_MASS :
                        center_of_mass++;
                        break;
                    case candidate_origin_type::MEDIAN :
                        median++;
                        break;
                    default:
                        na++;
                        break;
                    }

                }else{
                    initial_candidate ++;
                }
            }
        }

        /*auto sum = initial_candidate + two_pin + center_of_mass + na + median;
        auto move_sum = two_pin + center_of_mass + median;
        std::cout<< "Movements Report:" << sum << "\n" <<
                    "Initial position = " << initial_candidate << "\t" << std::to_string(initial_candidate / sum) << "% \n" <<
                    "2 pin net = " << two_pin << "\t" << std::to_string(two_pin / sum) << "% \t" << std::to_string(two_pin / move_sum) << "% \n" <<
                    "Center of mass = " << center_of_mass << "\t" << std::to_string(center_of_mass / sum) << "% \t" << std::to_string(center_of_mass / move_sum) << "% \n" <<
                    "Median = " << median << "\t" << std::to_string(median / sum) << "% \t" << std::to_string(median / move_sum) << "% \n" <<
                    "NA" << na << " \t" << std::to_string(na / sum) << "%" << std::endl;*/
    }

    template <typename var_type>
    void ILPRouting<var_type>::save_result(const solver_type& solver)
    {
    	std::ofstream results_file("ilp_results.csv", std::ofstream::out | std::ofstream::app);
        auto total_wirelength = unit_type{0};
        for(auto candidate : m_route_candidate)
        {
                auto variable = m_route_candidate_variables[candidate];
                auto value = solver.getValue(variable);
                if (value > 0)
                {
                auto wirelength = m_route_candidate_wirelengths[candidate];
                total_wirelength += wirelength;
                }
        }
        results_file << m_circuit_name << "," << total_wirelength.value() << std::endl;
    }

    template <typename var_type>
    void ILPRouting<var_type>::create_all_cell_initial_candidates(const cell_container_type & cells, model_type & model){
        auto & netlist = m_design.netlist();
        environment_type env = model.getEnv();

        for (auto & cell : cells) {
        //for (auto cell_it = m_design.netlist().begin_cell_instance(); cell_it != m_design.netlist().end_cell_instance(); cell_it++) {
            //auto cell = *cell_it;
            auto cell_name = m_design.netlist().name(cell);
            if (DEBUG) std::cout << "create candidates for cell " << cell_name << std::endl;
            
            // std::vector<net_type> cell_nets;
            // for(auto pin : m_design.netlist().pins(cell)){
            //     cell_nets.push_back(m_design.netlist().net(pin));
            // }
            
            //initial position
            auto initial_candidate = m_position_candidates.add();
            auto initial_variable_name = cell_name + "_initial";

            var_type initial_variable(env, 0.0, 1.0, initial_variable_name.c_str());

            m_position_candidate_names[initial_candidate] = initial_variable_name;
            m_position_candidate_cell[initial_candidate] = cell;
            m_name_to_position_candidate[initial_variable_name] = initial_candidate;
            m_position_candidate_variables[initial_candidate] = initial_variable;
            m_position_candidate_position[initial_candidate] = m_design.placement().location(cell);
            m_position_candidate_origin[initial_candidate] = candidate_origin_type::INITIAL;
            m_cell_initial_candidate[cell] = initial_candidate;
        }

    }

    template <typename var_type>
    void ILPRouting<var_type>::add_movements_constraints(model_type & model){
        auto max_movemnts = m_design.routing_constraints().max_cell_movement() - m_moved_cells;

        IloExpr candicates(m_env);
        for (auto candidate_it = m_position_candidates.begin(); candidate_it != m_position_candidates.end(); candidate_it++) {
            auto candidate = *candidate_it;        
            auto name = m_position_candidate_names[candidate];
            if (name.find("initial") == std::string::npos) {
                candicates += m_position_candidate_variables[candidate];
            }
        }
        std::string constraint_name = "max_movements";
        auto constraint = model.add(candicates <= IloExpr(m_env, max_movemnts));
        constraint.setName(constraint_name.c_str());
    }
}