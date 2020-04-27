#include "ILPRouting.h"

#include <fstream>
#include <regex>
#include <boost/lexical_cast.hpp>

namespace ophidian::routing
{
    ILPRouting::ILPRouting(design::Design & design, std::string circuit_name):
        m_design(design), m_circuit_name(circuit_name)
    {
    }

    bool ILPRouting::route_nets(const std::vector<net_type> & nets)
    {
        m_segments.clear();
        m_candidates.clear();

        IloModel model(m_env);
        IloCplex cplex(model);

        std::cout << "update capacities from blockages" << std::endl;
        update_gcell_capacities();

        std::cout << "add extra demand rules" << std::endl;
        add_extra_demand();

        std::cout << "create all candidates" << std::endl;
        create_all_candidates(nets, model);

        std::cout << "add objective function" << std::endl;
        add_objective_function(model);

        std::cout << "add candidate constraints" << std::endl;
        add_candidate_constraints(nets, model);

        std::cout << "add capacity constraints" << std::endl;
        add_capacity_constraints(nets, model);

        std::cout << "write model" << std::endl;
        cplex.exportModel("ilp_routing_model.lp");

        bool solved = cplex.solve();

        auto status = cplex.getCplexStatus();

        auto result = (status == IloCplex::CplexStatus::Optimal || status == IloCplex::CplexStatus::Feasible);

        if(result)
        {
            std::cout << "write solution" << std::endl;
            cplex.writeSolution("ilp_routing_model.sol");
	        unsigned routed_segments = 0;
    	    unsigned unrouted_segments = 0;
            for(auto net_it = m_design.netlist().begin_net(); net_it != m_design.netlist().end_net(); net_it++)
            {
                auto net = *net_it;
                auto candidates = m_net_candidates.parts(net);
                bool routed = 0;
            for(auto candidate : candidates)
            {
                auto variable = m_candidate_variables[candidate];
                auto value = cplex.getValue(variable);
                routed |= (value == 1);
            }
            if(routed)
            {
                routed_segments++;
        	}else{
                unrouted_segments++;
            }
        }
        double ratio = (double)routed_segments / (double)(routed_segments + unrouted_segments);
        std::cout << "routed segments " << routed_segments << std::endl;
        std::cout << "unrouted segments " << unrouted_segments << std::endl;
        std::cout << "routed segments " << ratio*100.0<<"%" << std::endl;

        write_segments(cplex, nets);

        //write_segments_dbg(cplex, nets);

        save_result(cplex);
        }
        return true;
    }

    void ILPRouting::update_gcell_capacities()
    {
        auto& global_routing = m_design.global_routing();
        auto gcell_graph = global_routing.gcell_graph();

        //copy the blockageDenamd to ILP propoerty!
        for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++)
        {
            auto gcell = *gcell_it;
            auto blockage_demand = gcell_graph->blockage_demand(gcell);
            m_gcells_demand[gcell] = blockage_demand;
        }

        // here we have to update the demand of nets which won't be routed in this execution!
    }

    void ILPRouting::add_extra_demand()
    {
        std::unordered_map<routing::GCellGraph::gcell_type, std::unordered_map<circuit::StandardCells::cell_type, double, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;
        auto& global_routing = m_design.global_routing();
        auto gcell_graph = global_routing.gcell_graph();
        for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++)
        {
            auto gcell = *gcell_it;
            auto gcell_box = gcell_graph->box(gcell);
            auto cells_within_gcell = placement::Placement::cell_container_type{};
            m_design.placement().cells_within(gcell_box, cells_within_gcell);

            std::unordered_map<circuit::StandardCells::cell_type, double, entity_system::EntityBaseHash> std_cell_count;
            for(auto cell : cells_within_gcell)
            {
                auto std_cell = m_design.netlist().std_cell(cell);
                std_cell_count[std_cell] += 1;
            }
            std_cells_per_gcell[gcell] = std_cell_count;
        }

        auto & routing_constraints = m_design.routing_constraints();
        for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
            auto gcell = *gcell_it;
            auto gcell_layer_index = gcell_graph->layer_index(gcell);
            auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
            auto gcell_layer = m_design.routing_library().find_layer_instance(gcell_layer_name);

            auto gcell_box = gcell_graph->box(gcell);

            for(auto same_grid_it = routing_constraints.begin_same_grid(); same_grid_it != routing_constraints.end_same_grid(); same_grid_it++)
            {
                auto key = same_grid_it->first;
                auto demand = same_grid_it->second;

                std::vector<std::string> strs;
                boost::split(strs, key, boost::is_any_of(":"));

                auto macro1_name = strs.at(0);
                auto macro2_name = strs.at(1);
                auto layer_name = strs.at(2);

                auto macro1 = m_design.standard_cells().find_cell(macro1_name);
                auto macro2 = m_design.standard_cells().find_cell(macro2_name);
                auto layer = m_design.routing_library().find_layer_instance(layer_name);

                if (layer == gcell_layer)
                {
                    auto pair_count = std::min(std_cells_per_gcell.at(gcell)[macro1], std_cells_per_gcell.at(gcell)[macro2]);
                    m_gcells_demand[gcell] += pair_count * demand;
                }
            }

            auto gcell_node = gcell_graph->graph_node(gcell);
            auto east_node = gcell_graph->east_node(gcell_node);
            auto east_gcell = gcell_graph->gcell(east_node);
            auto west_node = gcell_graph->west_node(gcell_node);
            auto west_gcell = gcell_graph->gcell(west_node);

            for(auto adj_grid_it = routing_constraints.begin_adj_grid(); adj_grid_it != routing_constraints.end_adj_grid(); adj_grid_it++)
            {
                auto key = adj_grid_it->first;
                auto demand = adj_grid_it->second;

                std::vector<std::string> strs;
                boost::split(strs, key, boost::is_any_of(":"));

                auto macro1_name = strs.at(0);
                auto macro2_name = strs.at(1);
                auto layer_name = strs.at(2);

                auto macro1 = m_design.standard_cells().find_cell(macro1_name);
                auto macro2 = m_design.standard_cells().find_cell(macro2_name);
                auto layer = m_design.routing_library().find_layer_instance(layer_name);

                if(layer == gcell_layer)
                {
                    auto east_pair_count = 0;
                    auto west_pair_count = 0;
                    if(macro1 == macro2)
                    {
                        if(east_node != lemon::INVALID)
                            east_pair_count = std::min(std_cells_per_gcell.at(gcell)[macro1], std_cells_per_gcell.at(east_gcell)[macro1]);
                        if(west_node != lemon::INVALID)
                            west_pair_count = std::min(std_cells_per_gcell.at(gcell)[macro1], std_cells_per_gcell.at(west_gcell)[macro1]);
                    }else{
                        if(east_node != lemon::INVALID)
                        {
                            east_pair_count = std::min(std_cells_per_gcell.at(gcell)[macro1], std_cells_per_gcell.at(east_gcell)[macro2])
                            + std::min(std_cells_per_gcell.at(gcell)[macro2], std_cells_per_gcell.at(east_gcell)[macro1]);
                        }
                        if(west_node != lemon::INVALID)
                        {
                            west_pair_count = std::min(std_cells_per_gcell.at(gcell)[macro1], std_cells_per_gcell.at(west_gcell)[macro2])
                            + std::min(std_cells_per_gcell.at(gcell)[macro2], std_cells_per_gcell.at(west_gcell)[macro1]);
                        }
                    }
                    m_gcells_demand[gcell] += (east_pair_count + west_pair_count) * demand;
                }
            }
        }
    }


    void ILPRouting::create_all_candidates(const std::vector<net_type> & nets, IloModel & model)
    {
        for(auto net : nets)
            create_net_candidates(net, model);
    }

    void ILPRouting::create_net_candidates(const net_type & net, IloModel & model)
    {
        auto& netlist = m_design.netlist();
        auto& global_routing = m_design.global_routing();
        auto& routing_library = m_design.routing_library();
        auto& placement = m_design.placement();
        auto& routing_constraints = m_design.routing_constraints();
        auto graph_ptr = global_routing.gcell_graph();
        IloEnv env = model.getEnv();

        auto net_name = netlist.name(net);

        int net_number = -1;
        std::regex word_regex("(\\d+)");
        auto words_begin = std::sregex_iterator(net_name.begin(), net_name.end(), word_regex);
        if(words_begin != std::sregex_iterator())
            net_number = boost::lexical_cast<int>(words_begin->str());

        auto segments = global_routing.segments(net);

        if(segments.size() > 0)
        {
            auto initial_candidate = m_candidates.add();
            auto initial_variable_name = net_name + "_initial";
            ilp_var_type initial_variable(env, initial_variable_name.c_str());
            m_candidate_names[initial_candidate] = initial_variable_name;
            m_name_to_candidate[initial_variable_name] = initial_candidate;
            m_candidate_variables[initial_candidate] = initial_variable;
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
                auto end_layer = global_routing.layer_end(segment);
                auto start_layer_name = routing_library.name(start_layer);
                auto end_layer_name = routing_library.name(end_layer);
                auto wire = create_wire(start, end, start_layer, end_layer);
                wires.push_back(wire);
            }
            add_wires_to_candidate(initial_candidate, wires);
        }

        auto net_pins = netlist.pins(net);
        std::vector<interconnection::Flute::Point> net_points;
        net_points.reserve(net_pins.size());
        std::map<std::pair<double, double>, pin_container_type> net_points_map;
        for(auto pin : net_pins)
        {
            auto pin_name = netlist.name(pin);
            auto pin_location = placement.location(pin);
            //std::cout << "pin " << pin_name << " location " << pin_location.x().value() << ", " << pin_location.y().value() << std::endl;
            net_points.push_back(pin_location);
            auto point = std::make_pair(pin_location.x().value(), pin_location.y().value());
            auto std_pin = netlist.std_cell_pin(pin);
            net_points_map[point].push_back(std_pin);
        }

        auto & flute = interconnection::Flute::instance();
        auto tree = flute.create(net_points);

        auto number_of_segments = tree->size(interconnection::SteinerTree::Segment());
        std::set<std::pair<unit_type, unit_type>> net_steiner_points;
        if(number_of_segments == 0)
        {
            // this happens when all pins are in the same gcell
            auto segment = m_segments.add();
            auto point = net_points.at(0);
            m_segment_starts[segment] = point;
            m_segment_ends[segment] = point;
            auto point_pair = std::make_pair(point.x().value(), point.y().value());
            m_segment_start_pin[segment] = net_points_map.at(point_pair);
            m_segment_end_pin[segment] = net_points_map.at(point_pair);
            m_net_segments.addAssociation(net, segment);
        }
        else
        {
      	    for(auto tree_segment_it = tree->segments().first; tree_segment_it != tree->segments().second; ++tree_segment_it)
            {
                auto tree_segment = *tree_segment_it;
                auto segment = m_segments.add();
                auto segment_start = tree->position(tree->u(tree_segment));
                auto segment_end = tree->position(tree->v(tree_segment));

                m_segment_starts[segment] = segment_start;
                m_segment_ends[segment] = segment_end;

                auto point = std::make_pair(segment_start.x().value(), segment_start.y().value());
                if(net_points_map.count(point))
                {
                    m_segment_start_pin[segment] = net_points_map.at(point);
                }else{
                    net_steiner_points.insert(std::make_pair(segment_start.x(), segment_start.y()));
                }
                point = std::make_pair(segment_end.x().value(), segment_end.y().value());
                if(net_points_map.count(point))
                {
                    m_segment_end_pin[segment] = net_points_map.at(point);
                }else{
                    net_steiner_points.insert(std::make_pair(segment_end.x(), segment_end.y()));
                }
                m_net_segments.addAssociation(net, segment);
	        }
        }

    	std::vector<layer_type> horizontal_layers;
	    std::vector<layer_type> vertical_layers;
    	for(auto layer_it = m_design.routing_library().begin_layer(); layer_it != m_design.routing_library().end_layer(); layer_it++)
        {
	        auto layer = *layer_it;
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

    	auto large_net = net_points.size() > 5;
        auto min_layer = routing_constraints.min_net_layer(net);
        auto min_layer_index = m_design.routing_library().layerIndex(min_layer);
    	for(auto horizontal_layer : horizontal_layers)
        {
            auto horizontal_layer_index = m_design.routing_library().layerIndex(horizontal_layer);
            if(horizontal_layer_index < min_layer_index)
            {
                continue;
            }
            for(auto vertical_layer : vertical_layers)
            {
                auto vertical_layer_index = m_design.routing_library().layerIndex(vertical_layer);
                if(vertical_layer_index < min_layer_index)
                {
                    continue;
                }
                if(std::abs(horizontal_layer_index - vertical_layer_index) > 1)
                {
                    continue;
                }
            	create_net_candidates_in_layers(net, horizontal_layer, vertical_layer, large_net, net_steiner_points, model);
    	    }
	    }
    }

    void ILPRouting::create_net_candidates_in_layers(const net_type & net, const layer_type & horizontal_layer, const layer_type & vertical_layer, bool large_net, const std::set<std::pair<unit_type, unit_type>> & steiner_points, IloModel & model)
    {
        auto& placement_library = m_design.placement_library();
        auto& routing_library = m_design.routing_library();
        IloEnv env = model.getEnv();

        auto net_name = m_design.netlist().name(net);
        auto horizontal_layer_name = m_design.routing_library().name(horizontal_layer);
        auto vertical_layer_name = m_design.routing_library().name(vertical_layer);

        //std::cout << "creating candidates for net " << net_name << " in layers " << horizontal_layer_name << " and " << vertical_layer_name << std::endl;

        unsigned number_of_candidates = 1;
    	if(large_net)
        {
	        number_of_candidates = 1;
    	}
        else
        {
            for(auto segment : m_net_segments.parts(net))
            {
                auto segment_start = m_segment_starts[segment];
                auto segment_end = m_segment_ends[segment];
                if(segment_start.x() != segment_end.x() && segment_start.y() != segment_end.y())
                {
                    number_of_candidates *= 2;
                }
            }
    	}

        //std::cout << "number of candidates " << number_of_candidates << std::endl;

        std::vector<candidate_type> candidates;
        candidates.reserve(number_of_candidates);
        for(auto candidate_index = 0; candidate_index < number_of_candidates; candidate_index++)
        {
            auto candidate = m_candidates.add();
            auto variable_name = net_name + "_" + horizontal_layer_name + "_" + vertical_layer_name + "_" + std::to_string(candidate_index);
            //std::cout << "variable " << variable_name << std::endl;
            ilp_var_type variable(env, variable_name.c_str());
            m_candidate_names[candidate] = variable_name;
            m_name_to_candidate[variable_name] = candidate;
            m_candidate_variables[candidate] = variable;
            m_net_candidates.addAssociation(net, candidate);
            candidates.push_back(candidate);
        }

        unsigned branch_count = 0;
        for(auto segment : m_net_segments.parts(net))
        {
            auto segment_start = m_segment_starts[segment];
            auto segment_end = m_segment_ends[segment];
            //std::cout << "segment " << segment_start.x().value() << ", " << segment_start.y().value() << "->"
                //<< segment_end.x().value() << ", " << segment_end.y().value() << std::endl;
            if(segment_start.x() != segment_end.x() && segment_start.y() != segment_end.y())
            {
                //std::cout << "split segment" << std::endl;
                add_wires_of_splitted_segment(segment, segment_start, segment_end, horizontal_layer, vertical_layer,  true, branch_count, candidates, large_net);
                if(!large_net)
                {
                    add_wires_of_splitted_segment(segment, segment_start, segment_end, horizontal_layer, vertical_layer, false, branch_count, candidates, large_net);
                }
                branch_count++;
            }
            else
            {
                auto layer = (segment_start.x() == segment_end.x()) ? vertical_layer : horizontal_layer;
                //std::cout << "create segment wire" << std::endl;
                auto wire = create_wire(segment_start, segment_end, layer, layer);
                auto wires = wire_container_type{wire};
                auto segment_start_pins = m_segment_start_pin[segment];
                for(auto segment_start_pin : segment_start_pins)
                {
                    auto pin_geometry = placement_library.geometry(segment_start_pin);
                    auto layer_name = pin_geometry.front().second;
                    auto pin_layer = routing_library.find_layer_instance(layer_name);
                    if(pin_layer != layer)
                    {
                        auto start_via = create_wire(segment_start, segment_start, pin_layer, layer);
                        wires.push_back(start_via);
                    }
                }
                auto segment_end_pins = m_segment_end_pin[segment];
                for(auto segment_end_pin : segment_end_pins)
                {
                    auto pin_geometry = placement_library.geometry(segment_end_pin);
                    auto layer_name = pin_geometry.front().second;
                    auto pin_layer = routing_library.find_layer_instance(layer_name);
                    if(pin_layer != layer)
                    {
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

        for(auto steiner_point : steiner_points)
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

    void ILPRouting::add_wires_of_splitted_segment(const segment_type & segment, const point_type & segment_start, const point_type & segment_end, const layer_type & horizontal_layer, const layer_type & vertical_layer, bool connect_on_y, unsigned branch_count, const std::vector<candidate_type> & candidates, bool large_net)
    {
        auto& placement_library = m_design.placement_library();
        auto& routing_library = m_design.routing_library();

        auto layer1 = (connect_on_y) ? vertical_layer : horizontal_layer;
        auto layer2 = (connect_on_y) ? horizontal_layer : vertical_layer;
        wire_container_type wires;
        split_segment(segment_start, segment_end, layer1, layer2, connect_on_y, wires);
        auto segment_start_pins = m_segment_start_pin[segment];
        for(auto segment_start_pin : segment_start_pins)
        {
            auto pin_geometry = placement_library.geometry(segment_start_pin);
            auto layer_name = pin_geometry.front().second;
            auto pin_layer = routing_library.find_layer_instance(layer_name);
            if(pin_layer != layer1)
            {
                auto start_via = create_wire(segment_start, segment_start, pin_layer, layer1);
                wires.push_back(start_via);
            }
        }
        auto segment_end_pins = m_segment_end_pin[segment];
        for(auto segment_end_pin : segment_end_pins)
        {
            auto pin_geometry = placement_library.geometry(segment_end_pin);
            auto layer_name = pin_geometry.front().second;
            auto pin_layer = routing_library.find_layer_instance(layer_name);
            if(pin_layer != layer2)
            {
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
                    //std::cout << "candidate " << i << " number of wires " << m_candidate_wires[candidate].size() << std::endl;
                }
                candidate_index += step;
            }
    	}
    }

    void ILPRouting::add_wires_to_candidate(const candidate_type & candidate, const wire_container_type & wires)
    {
        auto wirelength = unit_type{0};
        for(auto wire : wires)
        {
            m_candidate_wires[candidate].push_back(wire);
            auto wire_start = m_wire_starts[wire];
            auto wire_end = m_wire_ends[wire];
            wirelength += unit_type{std::abs(wire_end.x().value() - wire_start.x().value()) / 10.0 + std::abs(wire_end.y().value() - wire_start.y().value()) / 10.0};
            auto wire_start_layer = m_wire_start_layers[wire];
            auto wire_end_layer = m_wire_end_layers[wire];
            auto start_layer_index = m_design.routing_library().layerIndex(wire_start_layer);
            auto end_layer_index = m_design.routing_library().layerIndex(wire_end_layer);
            if(wire_start_layer != wire_end_layer)
            {
                double abs = std::abs(start_layer_index - end_layer_index);
                wirelength += unit_type{abs};
            }

            //std::cout << "adding wire " << wire_start.x().value() << " " << wire_start.y().value() << " " << start_layer_index << " " << wire_end.x().value() << " " << wire_end.y().value() << " " << end_layer_index << std::endl;
        }
    	m_candidate_wires[candidate].shrink_to_fit();
        m_candidate_wirelengths[candidate] += wirelength;
    }

    void ILPRouting::split_segment(const point_type & segment_start, const point_type & segment_end, const layer_type & wire1_layer, const layer_type & wire2_layer, bool connect_on_y, wire_container_type & wires)
    {
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

    ILPRouting::wire_type ILPRouting::create_wire(const point_type & wire_start, const point_type & wire_end, const layer_type & start_layer, const layer_type & end_layer)
    {
        auto wire = m_wires.add();
        m_wire_starts[wire] = wire_start;
        m_wire_ends[wire] = wire_end;

        auto min_x = std::min(wire_start.x().value(), wire_end.x().value());
        auto max_x = std::max(wire_start.x().value(), wire_end.x().value());
        auto min_y = std::min(wire_start.y().value(), wire_end.y().value());
        auto max_y = std::max(wire_start.y().value(), wire_end.y().value());

        auto wire_box = geometry::Box<double>{{min_x, min_y}, {max_x, max_y}};

        auto start_layer_index = m_design.routing_library().layerIndex(start_layer);
        auto end_layer_index = m_design.routing_library().layerIndex(end_layer);
        //std::cout << "creating wire " << wire_box.min_corner().x() << " " << wire_box.min_corner().y() << " " << start_layer_index << " -> " << wire_box.max_corner().x() << " " << wire_box.max_corner().y() << " " << end_layer_index << std::endl;

        m_wire_start_layers[wire] = start_layer;
        m_wire_end_layers[wire] = end_layer;
        return wire;
    }

    void ILPRouting::add_objective_function(IloModel & model)
    {
        IloObjective obj = IloMinimize(m_env);
        unsigned candidate_count = 0;
        IloExpr expr(m_env);
        for(auto candidate : m_candidates)
        {
            auto candidate_variable = m_candidate_variables[candidate];
            auto candidate_wirelength = m_candidate_wirelengths[candidate].value();

            //std::cout << "candidate " << m_candidate_names[candidate] << " wirelength " << candidate_wirelength << std::endl;

            expr += candidate_variable * candidate_wirelength;
        }
        obj.setExpr(expr);
        model.add(obj);
    }

    void ILPRouting::add_candidate_constraints(const std::vector<net_type> & nets, IloModel & model)
    {
        unsigned net_count = 0;
        for(auto net : nets)
        {
            IloExpr expr(m_env);
            auto candidates = m_net_candidates.parts(net);
            for(auto candidate : candidates)
            {
                auto candidate_variable = m_candidate_variables[candidate];
                expr += candidate_variable;
            }
            model.add(expr == 1);
        }
    }

    void ILPRouting::add_capacity_constraints(const std::vector<net_type> & nets, IloModel & model)
    {
        auto& global_routing = m_design.global_routing();
        auto gcell_graph = global_routing.gcell_graph();

        std::unordered_map<std::string, std::unordered_map<gcell_type, std::unordered_set<candidate_type, entity_system::EntityBaseHash>, entity_system::EntityBaseHash>> gcell_nets;
        for(auto net : nets)
        {
            auto net_name = m_design.netlist().name(net);
            //std::cout << "capacity constraints for net " << net_name << std::endl;
            auto candidates = m_net_candidates.parts(net);
            for(auto candidate : candidates)
            {
                auto wires = m_candidate_wires[candidate];
                for(auto wire : wires)
                {
                    auto start_layer = m_wire_start_layers[wire];
                    auto end_layer = m_wire_end_layers[wire];
                    auto start_layer_index = m_design.routing_library().layerIndex(start_layer);
                    auto end_layer_index = m_design.routing_library().layerIndex(end_layer);
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

                    for(auto layer_index = min_layer_index; layer_index <= max_layer_index; layer_index++)
                    {
                        auto layer_name = "M" + std::to_string(layer_index);
                        gcell_container_type gcells;
                        gcell_graph->intersect(gcells, wire_box, layer_index-1);
                        //std::cout << "gcells " << gcells.size() << std::endl;
                        for(auto gcell : gcells)
                        {
                            gcell_nets[layer_name][gcell].insert(candidate);
                        }
                    }
                }
            }
        }
        for(auto layer_pair : gcell_nets)
        {
            auto layer_name = layer_pair.first;
            auto layer_map = layer_pair.second;
            auto layer = m_design.routing_library().find_layer_instance(layer_name);
            auto layer_index = m_design.routing_library().layerIndex(layer);
            for (auto gcell_pair : layer_map) {
                auto gcell = gcell_pair.first;
                auto gcell_set = gcell_pair.second;
                auto box = gcell_graph->box(gcell);
                auto gcell_min_corner = box.min_corner();
                if(gcell_set.size() > 0)
                {
                    IloExpr expr(m_env);
                    for(auto candidate : gcell_set)
                    {
                        auto variable = m_candidate_variables[candidate];
                        expr += variable;
                    }
                    auto capacity = gcell_graph->capacity(gcell);
                    auto demand = m_gcells_demand[gcell];
                    auto constraint_name = std::to_string((int)gcell_min_corner.y().value()) + "_" + std::to_string((int)gcell_min_corner.x().value()) + "_" + std::to_string(layer_index);
                    auto constraint = model.add(expr <= (capacity - demand));
                    constraint.setName(constraint_name.c_str());
                }
            }
        }
    }

    void ILPRouting::write_segments(const IloCplex& cplex, const std::vector<net_type> & nets)
    {
        auto & global_routing = m_design.global_routing();
        for(auto net : nets)
        {
    	    auto net_name = m_design.netlist().name(net);
            auto candidates = m_net_candidates.parts(net);
            bool routed = 0;
    	    candidate_type routed_candidate;
            for(auto candidate : candidates)
            {
                auto candidate_name = m_candidate_names[candidate];
                auto variable = m_candidate_variables[candidate];
                auto value = cplex.getValue(variable);
    	    	if(value)
                {
                    routed = 1;
		            routed_candidate = candidate;
                    break;
        		}
            }
            if(routed)
            {
                //clear possible old routings
                global_routing.unroute(net);

        		for(auto wire : m_candidate_wires[routed_candidate])
                {
        		    auto wire_start = m_wire_starts[wire];
		            auto wire_end = m_wire_ends[wire];
        		    auto wire_start_layer = m_wire_start_layers[wire];
		            auto wire_end_layer = m_wire_end_layers[wire];

                    auto min_x = std::min(wire_start.x(), wire_end.x());
                    auto max_x = std::max(wire_start.x(), wire_end.x());
                    auto min_y = std::min(wire_start.y(), wire_end.y());
                    auto max_y = std::max(wire_start.y(), wire_end.y());

                    auto wire_box = box_type{{min_x, min_y}, {max_x, max_y}};
                    global_routing.add_segment(wire_box, wire_start_layer, wire_end_layer, net);
		        }
    	    }
            else
            {
                std::cout << "WARNING: net " << net_name << " unrouted" << std::endl;
            }
         }
    }

    void ILPRouting::write_segments_dbg(const IloCplex& cplex, const std::vector<net_type> & nets)
    {
	    std::ofstream routed_segments("routed_segments_" + m_circuit_name + ".txt");
	    routed_segments << "net_name,xs,ys,zs,xt,yt,zt" << std::endl;
	    std::ofstream unrouted_segments("unrouted_segments_" + m_circuit_name + ".txt");
	    unrouted_segments << "net_name,xs,ys,pin_start,xt,yt,pin_end" << std::endl;

        std::vector<std::string> route_strings;

	    unsigned net_count = 0;
        for(auto net : nets)
        {
    	    auto net_name = m_design.netlist().name(net);
            //std::cout << net_name << " count " << net_count++ << std::endl;
            auto candidates = m_net_candidates.parts(net);
            bool routed = 0;
    	    candidate_type routed_candidate;
            for(auto candidate : candidates)
            {
                auto candidate_name = m_candidate_names[candidate];
                auto variable = m_candidate_variables[candidate];
                auto value = cplex.getValue(variable);
    	    	if(value)
                {
                    routed = 1;
		            routed_candidate = candidate;
                    auto name = variable.getName();
        		}
            }
            if(routed)
            {
	    	    unsigned count_candidate = 0;
                auto candidate_name = m_candidate_names[routed_candidate];
                //std::cout << "routed candidate " << candidate_name << std::endl;
        		for(auto wire : m_candidate_wires[routed_candidate])
                {
        		    auto wire_start = m_wire_starts[wire];
		            auto wire_end = m_wire_ends[wire];
        		    auto wire_start_layer = m_wire_start_layers[wire];
		            auto wire_end_layer = m_wire_end_layers[wire];
        		    auto start_layer_index = m_design.routing_library().layerIndex(wire_start_layer);
		            auto end_layer_index = m_design.routing_library().layerIndex(wire_end_layer);
        		    routed_segments << net_name << "," << wire_start.x().value() << "," << wire_start.y().value() << "," << start_layer_index << "," << wire_end.x().value() << "," << wire_end.y().value() << "," << end_layer_index << std::endl;
                    std::cout << net_name << "," << wire_start.x().value() << "," << wire_start.y().value() << "," << start_layer_index << "," << wire_end.x().value() << "," << wire_end.y().value() << "," << end_layer_index << std::endl;

                    auto route_string = std::to_string(((int)wire_start.y().value() - 5) / 10 + 1) + " " + std::to_string(((int)wire_start.x().value() - 5) / 10 +1) + " " + std::to_string(start_layer_index) + " " + std::to_string(((int)wire_end.y().value() - 5) / 10+1) + " " + std::to_string(((int)wire_end.x().value() - 5) / 10 +1) + " " + std::to_string(end_layer_index) + " " + net_name;
                    route_strings.push_back(route_string);
		        }
    	    }
            else
            {
                std::cout << "net " << net_name << " unrouted" << std::endl;
        		for(auto segment : m_net_segments.parts(net))
                {
		    	    auto segment_start = m_segment_starts[segment];
        			auto segment_end = m_segment_ends[segment];
	        		auto pin_start = m_segment_start_pin[segment];
		        	auto pin_end = m_segment_end_pin[segment];
	        	}
            }
         }
    }

    void ILPRouting::save_result(const IloCplex& cplex)
    {
    	std::ofstream results_file("ilp_results.csv", std::ofstream::out | std::ofstream::app);
        auto total_wirelength = unit_type{0};
        for(auto candidate : m_candidates)
        {
            auto variable = m_candidate_variables[candidate];
            auto value = cplex.getValue(variable);
            if (value == 1)
            {
                auto wirelength = m_candidate_wirelengths[candidate];
                total_wirelength += wirelength;
            }
        }
        results_file << m_circuit_name << "," << total_wirelength.value() << std::endl;
    }
}