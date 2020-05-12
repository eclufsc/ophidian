#include "ILPRouting.h"

#include <fstream>
#include <regex>
#include <boost/lexical_cast.hpp>

bool DEBUG = true;

namespace ophidian::routing {
    ILPRouting::ILPRouting(design::Design & design, std::string circuit_name):
        m_design(design), m_circuit_name(circuit_name)
    {
    }

    bool ILPRouting::route_nets(const std::vector<net_type> & nets, const std::vector<net_type> & fixed_nets, std::vector<net_type> & routed_nets, std::vector<std::pair<cell_type, point_type>> & movements, bool integer)
    {
        m_integer = integer;

        m_segments.clear();
        m_wires.clear();
        m_route_candidate.clear();
        m_position_candidates.clear();

        GRBModel model = GRBModel(m_GRBENv);

        if(DEBUG) std::cout << "update capacities from blockages" << std::endl;
        update_gcell_capacities(fixed_nets);

        if(DEBUG) std::cout << "create all cells initial candidates" << std::endl;
        create_all_cell_initial_candidates(model);

        if(DEBUG) std::cout << "create all candidates" << std::endl;
        create_all_candidates(nets, model);

        if(DEBUG) std::cout << "create all candidates with movements" << std::endl;
        create_all_candidates_with_movements(nets, model);

        if(DEBUG) std::cout << "add objective function" << std::endl;
        add_objective_function(model);

        if(DEBUG) std::cout << "add candidate constraints" << std::endl;
        add_candidate_constraints(nets, model);

        if(DEBUG) std::cout << "add capacity constraints" << std::endl;
        add_capacity_constraints(nets, model);

        if(DEBUG) std::cout << "add movements constraints" << std::endl;
        add_movements_constraints(model);

        if(DEBUG) std::cout << "write model" << std::endl;
        model.write("ilp_routing_model.lp");

        model.optimize();

        auto status = model.get(GRB_IntAttr_Status);

        auto result = (status == GRB_OPTIMAL || status == GRB_SUBOPTIMAL);

        if(result)
        {
            if(DEBUG) std::cout << "write solution" << std::endl;
            model.write("ilp_routing_model.sol");

	        unsigned routed_segments = 0;
    	    unsigned unrouted_segments = 0;
            for(auto net_it = m_design.netlist().begin_net(); net_it != m_design.netlist().end_net(); net_it++) {
                auto net = *net_it;
        		auto candidates = m_net_candidates.parts(net);
		        bool routed = 0;
        		for(auto candidate : candidates)
                {
		        	auto variable = m_route_candidate_variables[candidate];
        			auto value = variable.get(GRB_DoubleAttr_X);
		        	routed |= (value == 1);
        		}
        		if(routed)
                {
		        	routed_segments++;
        		}else
                {
		        	unrouted_segments++;
        		}
	        }
    	    double percentage = (double)routed_segments / (double)(routed_segments + unrouted_segments);
	        if(DEBUG) std::cout << "routed segments " << routed_segments << std::endl;
	        if(DEBUG) std::cout << "unrouted segments " << unrouted_segments << std::endl;
    	    if(DEBUG) std::cout << "percentage of routed segments " << percentage << std::endl;

    	    write_segments(nets, routed_nets);

            save_movements(movements);

	        save_result();
        }
        return result;
     }

    void ILPRouting::update_gcell_capacities(const std::vector<net_type> & fixed_nets)
    {
        // auto& routing_library = m_design.routing_library();
        auto& global_routing = m_design.global_routing();
        // auto& netlist = m_design.netlist();
        // auto& placement = m_design.placement();
        auto gcell_graph = global_routing.gcell_graph();

        //copy the blockageDenamd to ILP propoerty!
        // for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
        //     auto gcell = *gcell_it;
        //     auto blockage_demand = gcell_graph->blockage_demand(gcell);
        //     m_gcells_demand[gcell] = blockage_demand;
        // }

        // here we have to update the demand od nets with won't be routed in this execution!
        for (auto net : fixed_nets) {
            std::unordered_set<gcell_type, entity_system::EntityBaseHash> gcells;
            for (auto segment : global_routing.segments(net)) {
                auto gcell_start = global_routing.gcell_start(segment);
                auto gcell_end = global_routing.gcell_end(segment);
                gcells.insert(gcell_start);
                gcells.insert(gcell_end);
            }
            for (auto gcell : gcells) {
                m_gcells_demand[gcell] += 1;
            }
        }
    }

    void ILPRouting::create_all_candidates(const std::vector<net_type> & nets, GRBModel & model)
    {
        for(auto net : nets)
            create_net_candidates(net, model);
    }

    

    void ILPRouting::create_net_candidates(const net_type & net, GRBModel & model)
    {
        auto& netlist = m_design.netlist();
        auto& global_routing = m_design.global_routing();
        auto& routing_library = m_design.routing_library();
        auto& placement = m_design.placement();
        auto& routing_constraints = m_design.routing_constraints();
        auto graph_ptr = global_routing.gcell_graph();

        auto net_name = netlist.name(net);

        int net_number = -1;
        std::regex word_regex("(\\d+)");
        auto words_begin = std::sregex_iterator(net_name.begin(), net_name.end(), word_regex);
        if(words_begin != std::sregex_iterator())
            net_number = boost::lexical_cast<int>(words_begin->str());

        auto segments = global_routing.segments(net);


        if(segments.size() > 0)
        {
            auto initial_candidate = m_route_candidate.add();
            auto initial_variable_name = net_name + "_initial";
            GRBVar initial_variable;
            if (m_integer) {
                initial_variable = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, initial_variable_name);
            } else {
                initial_variable = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, initial_variable_name);
            }
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
                auto end_layer = global_routing.layer_end(segment);
                auto start_layer_name = routing_library.name(start_layer);
                auto end_layer_name = routing_library.name(end_layer);
                auto wire = create_wire(start, end, start_layer, end_layer);
                wires.push_back(wire);
            }
            add_wires_to_candidate(initial_candidate, wires);
        }

        generate_routes_of_net(net, position_candidate_type(), model);   
    }

    void ILPRouting::create_all_candidates_with_movements(const std::vector<net_type> & nets, GRBModel & model)
    {
        auto & netlist = m_design.netlist();
        auto & placement = m_design.placement();
        for(auto net : nets)
        {
            auto size = netlist.pins(net).size();
            if(size == 2)
                create_2_pin_nets_candidates_with_movements(net, model);
        }
        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++){
            auto cell = *cell_it;
            if( !placement.isFixed(cell))
            {
                create_center_of_mass_candidate(cell, model);
                create_median_candidate(cell, model);
            }
        }
    }

    void ILPRouting::create_center_of_mass_candidate(const cell_type cell, GRBModel & model){
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

    void ILPRouting::create_median_candidate(const cell_type cell, GRBModel & model){
        auto & netlist = m_design.netlist();
        auto & placement = m_design.placement();
        std::vector<net_type> cell_nets;
        
        std::vector<double> x_positions;
        std::vector<double> y_positions;

        for(auto pin : netlist.pins(cell)){
            auto net = netlist.net(pin);
            cell_nets.push_back(net);
            for(auto net_pin : netlist.pins(net)){
                if(net_pin == pin)
                    continue;
                auto location = placement.location(net_pin);
                x_positions.push_back(location.x().value());
                y_positions.push_back(location.y().value());
            }
        }

        std::nth_element(x_positions.begin(), x_positions.begin() + x_positions.size()/2, x_positions.end());
        auto median_x = x_positions[x_positions.size()/2];
        std::nth_element(y_positions.begin(), y_positions.begin() + y_positions.size()/2, y_positions.end());
        auto median_y = y_positions[y_positions.size()/2];

        point_type median_point {unit_type(median_x), unit_type(median_y)};
        
        auto nearst_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(median_point, 0);

        auto cell_location = placement.location(cell);
        auto actual_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(cell_location, 0);

        if(actual_gcell != nearst_gcell){
            auto center_gcell = m_design.global_routing().gcell_graph()->center_of_box(nearst_gcell);
            // creating candidate
            auto cell_name = netlist.name(cell);
            create_movement_candidate(cell, candidate_origin_type::MEDIAN, center_gcell, cell_nets, cell_name + "_median", model);
        }
    }

    void ILPRouting::create_2_pin_nets_candidates_with_movements(const net_type net, GRBModel & model){
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        auto candidate_type =  candidate_origin_type::TWO_PIN_NET;

        auto net_name = netlist.name(net);
        std::vector<ophidian::circuit::PinInstance> pins (netlist.pins(net).begin(), netlist.pins(net).end());

        auto pin_a = pins[0];
        auto cell_a = netlist.cell(pin_a);
        auto cell_a_name = netlist.name(cell_a);
        auto cell_a_pos = placement.location(cell_a);
        auto cell_a_fixed = placement.isFixed(cell_a);
        std::vector<net_type> nets_of_cell_a;
        for(auto pin : netlist.pins(cell_a)){
            auto net = netlist.net(pin);
            nets_of_cell_a.push_back(net);
        }
        
        auto pin_b = pins[1];
        auto cell_b = netlist.cell(pin_b);
        auto cell_b_name = netlist.name(cell_b);
        auto cell_b_pos = placement.location(cell_b);
        auto cell_b_fixed = placement.isFixed(cell_b);
        std::vector<net_type> nets_of_cell_b;
        for(auto pin : netlist.pins(cell_b)){
            auto net = netlist.net(pin);
            nets_of_cell_b.push_back(net);
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

    void ILPRouting::create_movement_candidate(const cell_type & cell, const candidate_origin_type type, const point_type& new_position, const std::vector<net_type>& nets, std::string variable_name, GRBModel & model )
    {
        auto & netlist = m_design.netlist();
        auto & placement = m_design.placement();
        auto & placement_library = m_design.placement_library();
        
        
        auto initial_position = placement.location(cell);
        auto stdCell = netlist.std_cell(cell);
        auto stdCellGeometry = placement_library.geometry(stdCell);
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
        GRBVar candidate_variable;
        if (m_integer) {
            candidate_variable = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, candidate_variable_name);
        } else {
            candidate_variable = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, candidate_variable_name);
        }
        m_position_candidate_names[candidate] = candidate_variable_name;
        m_position_candidate_cell[candidate] = cell;
        m_name_to_position_candidate[candidate_variable_name] = candidate;
        m_position_candidate_variables[candidate] = candidate_variable;
        m_position_candidate_position[candidate] = candidate_position;
        m_cell_position_candidates.addAssociation(cell, candidate);
        m_position_candidate_origin[candidate] = type;

        

        // place cell in the new position
        placement.place(cell, candidate_position);

        //genarate route for every net with new location
        for(auto net : nets)
            generate_routes_of_net(net, candidate, model);
        // restore original position
        placement.place(cell, initial_position);
    }

    void ILPRouting::generate_routes_of_net(const net_type & net, const position_candidate_type pos_candidate , GRBModel & model)
    {
        auto& routing_constraints = m_design.routing_constraints();
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
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
        std::vector<segment_type> segments;
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
            // m_net_segments.addAssociation(net, segment);
            segments.push_back(segment);
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
                }
                else
                {
                    net_steiner_points.insert(std::make_pair(segment_start.x(), segment_start.y()));
                }
                point = std::make_pair(segment_end.x().value(), segment_end.y().value());
                if(net_points_map.count(point))
                {
                    m_segment_end_pin[segment] = net_points_map.at(point);
                }else
                {
                    net_steiner_points.insert(std::make_pair(segment_end.x(), segment_end.y()));
                }
                // m_net_segments.addAssociation(net, segment);
                segments.push_back(segment);
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
            	create_net_candidates_in_layers(net, segments, horizontal_layer, vertical_layer, large_net, net_steiner_points, pos_candidate, model);
    	    }
	    }
    }

    void ILPRouting::create_net_candidates_in_layers(const net_type & net, const std::vector<segment_type> & segments, const layer_type & horizontal_layer, const layer_type & vertical_layer, bool large_net, const std::set<std::pair<unit_type, unit_type>> & steiner_points, const position_candidate_type pos_candidate, GRBModel & model)
    {
        auto& placement_library = m_design.placement_library();
        auto& routing_library = m_design.routing_library();

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
	    for(auto segment : segments)
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
            //std::cout << "variable " << variable_name << std::endl;
            GRBVar variable;
            if (m_integer) {
                variable = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, variable_name);
            } else {                
                variable = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, variable_name);
            }
            m_route_candidate_names[candidate] = variable_name;
            m_name_to_route_candidate[variable_name] = candidate;
            m_route_candidate_variables[candidate] = variable;
            candidates.push_back(candidate);

            if(pos_candidate != position_candidate_type()) {
                auto position_candidate_name = m_position_candidate_names[pos_candidate];
                m_position_candidate_to_routes.addAssociation(pos_candidate, candidate);
            }
            //else
            m_net_candidates.addAssociation(net, candidate);
        }

        unsigned branch_count = 0;
        for(auto segment : segments)
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

    void ILPRouting::add_wires_of_splitted_segment(const segment_type & segment, const point_type & segment_start, const point_type & segment_end, const layer_type & horizontal_layer, const layer_type & vertical_layer, bool connect_on_y, unsigned branch_count, const std::vector<route_candidate_type> & candidates, bool large_net)
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
            auto pin_name = m_design.standard_cells().name(segment_end_pin);
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
                    //std::cout << "candidate " << i << " number of wires " << m_route_candidate_wires[candidate].size() << std::endl;
                }
                candidate_index += step;
            }
    	}
    }

    void ILPRouting::add_wires_to_candidate(const route_candidate_type & candidate, const wire_container_type & wires)
    {
        auto wirelength = unit_type{0};
        for(auto wire : wires)
        {
            m_route_candidate_wires[candidate].push_back(wire);
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
    	m_route_candidate_wires[candidate].shrink_to_fit();
        m_route_candidate_wirelengths[candidate] += wirelength;
    }

    void ILPRouting::split_segment(const point_type & segment_start, const point_type & segment_end, const layer_type & wire1_layer, const layer_type & wire2_layer, bool connect_on_y, wire_container_type & wires) {
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

    void ILPRouting::add_objective_function(GRBModel & model)
    {
        GRBLinExpr objective = 0.0;
        unsigned candidate_count = 0;
        for(auto candidate : m_route_candidate)
        {
            auto candidate_variable = m_route_candidate_variables[candidate];
            auto candidate_wirelength = m_route_candidate_wirelengths[candidate].value();

            //std::cout << "candidate " << m_route_candidate_names[candidate] << " wirelength " << candidate_wirelength << std::endl;

            objective += candidate_variable * candidate_wirelength;
        }
        model.setObjective(objective, GRB_MINIMIZE);
    }

    void ILPRouting::add_candidate_constraints(const std::vector<net_type> & nets, GRBModel & model)
    {
        unsigned net_count = 0;
        for(auto net : nets)
        {
            GRBLinExpr candidates_constraints = 0.0;
            auto candidates = m_net_candidates.parts(net);
            for(auto candidate : candidates)
            {
                auto candidate_variable = m_route_candidate_variables[candidate];
                candidates_constraints += candidate_variable;
            }
            model.addConstr(candidates_constraints == 1);
        }

        auto & netlist = m_design.netlist();
        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++){
            auto cell = *cell_it;
            std::vector<net_type> cell_nets;
            for(auto pin : m_design.netlist().pins(cell)){
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

            auto position_candidates = m_cell_position_candidates.parts(cell);
            for(auto pos_candidate : position_candidates) {
                std::unordered_map<std::string, GRBLinExpr> cell_candidate_expressions;
                auto routes = m_position_candidate_to_routes.parts(pos_candidate);
                for(auto route : routes)
                {
                    auto candidate_variable = m_route_candidate_variables[route];
                    auto variable_name = m_route_candidate_names[route];
                    std::vector<std::string> strs;
                    boost::split(strs, variable_name, boost::is_any_of("_"));
                    auto net_name = strs.at(0);
                    cell_candidate_expressions[net_name] += candidate_variable;
                }
                auto cell_position_variable = m_position_candidate_variables[pos_candidate];
                for (auto expression_pair_it = cell_candidate_expressions.begin(); expression_pair_it != cell_candidate_expressions.end(); expression_pair_it++) {
                    auto expression = expression_pair_it->second;
                    model.addConstr(expression == cell_position_variable);
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

        // std::unordered_set<net_type, entity_system::EntityBaseHash> nets;
        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++){
            auto cell = *cell_it;
            GRBLinExpr candidates_constraints = 0.0;
            
            auto initial_candidate = m_cell_initial_candidate[cell];
            auto initial_candidate_variable = m_position_candidate_variables[initial_candidate];
            candidates_constraints += initial_candidate_variable;

            auto candidates = m_cell_position_candidates.parts(cell);
            for (auto candidate : candidates) {
                auto candidate_variable = m_position_candidate_variables[candidate];
                candidates_constraints += candidate_variable;
            }
            model.addConstr(candidates_constraints == 1);

            // for(auto pin : netlist.pins(cell)){
            //     auto net = netlist.net(pin);
            //     nets.insert(net);
            // }
        }

        //freeze neighbors
        for(auto net : nets){
            GRBLinExpr neighbors_constraints = 0.0;
            auto cont_candidates = 0;
            for(auto pin : netlist.pins(net)){
                auto cell = netlist.cell(pin);
                auto candidates = m_cell_position_candidates.parts(cell);
                for (auto candidate : candidates) {
                    auto candidate_variable = m_position_candidate_variables[candidate];
                    neighbors_constraints += candidate_variable;
                    cont_candidates ++;
                }
            }
            if(cont_candidates != 0)
                model.addConstr(neighbors_constraints <= 1);
        }
    }

    void ILPRouting::add_capacity_constraints(const std::vector<net_type> & nets, GRBModel & model)
    {
        auto& global_routing = m_design.global_routing();
        auto gcell_graph = global_routing.gcell_graph();
        auto & netlist = m_design.netlist();
        auto & routing_library = m_design.routing_library();
        
        entity_system::Property<gcell_type, GRBLinExpr>  gcells_constraints{m_design.global_routing().gcell_graph()->make_property_gcells<GRBLinExpr>()};
        
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
            }
        }


        std::unordered_map<std::string, std::unordered_map<gcell_type, std::unordered_set<route_candidate_type, entity_system::EntityBaseHash>, entity_system::EntityBaseHash>> gcell_nets;
        for(auto net : nets)
        {
            auto net_name = m_design.netlist().name(net);
            //std::cout << "capacity constraints for net " << net_name << std::endl;
            auto candidates = m_net_candidates.parts(net);
            for(auto candidate : candidates)
            {
                auto wires = m_route_candidate_wires[candidate];
                for(auto wire : wires)
                {
                    auto start_layer = m_wire_start_layers[wire];
                    auto end_layer = m_wire_end_layers[wire];
                    auto start_layer_index = routing_library.layerIndex(start_layer);
                    auto end_layer_index = routing_library.layerIndex(end_layer);
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
                        // auto layer_name = "M" + std::to_string(layer_index);
                        auto layer = routing_library.layer_from_index(layer_index);
                        auto layer_name = routing_library.name(layer);
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
                    GRBLinExpr gcell_constraint = 0.0;
                    for(auto candidate : gcell_set)
                    {
                        auto variable = m_route_candidate_variables[candidate];
                        gcell_constraint += variable;
                    }
                    gcells_constraints[gcell] += gcell_constraint;
                }
            }
        }

        for(auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++){
            auto gcell = *gcell_it;

            auto node = gcell_graph->graph_node(gcell);
            auto location = gcell_graph->position(node);

            auto gcell_constraint = gcells_constraints[gcell];
            if(gcell_constraint.size() > 0 )
            {
                auto capacity = gcell_graph->capacity(gcell);
                auto demand = m_gcells_demand[gcell];
                auto constraint_name = std::to_string(location.get<1>()) + "_" + std::to_string(location.get<0>()) + "_" + std::to_string(location.get<2>()) ;
                model.addConstr(gcell_constraint <= capacity - demand, constraint_name);
            }
        }
    }

    void ILPRouting::write_segments(const std::vector<net_type> & nets, std::vector<net_type> & routed_nets)
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
                auto value = variable.get(GRB_DoubleAttr_X);
    	    	if(value == 1)
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

        		for(auto wire : m_route_candidate_wires[routed_candidate])
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
                //std::cout << "WARNING: net " << net_name << " unrouted" << std::endl;
            }
         }
    }

    void ILPRouting::save_movements(std::vector<std::pair<cell_type, point_type>> & movements) {
        int initial_candidate = 0;
        int two_pin = 0;
        int center_of_mass = 0;
        int median = 0;
        int na = 0;
        for (auto candidate_it = m_position_candidates.begin(); candidate_it != m_position_candidates.end(); candidate_it++) {
            auto candidate = *candidate_it;
            auto variable = m_position_candidate_variables[candidate];
            auto value = variable.get(GRB_DoubleAttr_X);
            auto name = variable.get(GRB_StringAttr_VarName);
            if (value == 1) {
                if (name.find("initial") == std::string::npos) {
                    auto candidate = m_name_to_position_candidate[name];
                    auto cell = m_position_candidate_cell[candidate];
                    auto position = m_position_candidate_position[candidate];
                    movements.push_back(std::make_pair(cell, position));
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

        auto sum = initial_candidate + two_pin + center_of_mass + na + median;
        auto move_sum = two_pin + center_of_mass + median;
        /*std::cout<< "Movements Report:" << sum << "\n" <<
                    "Initial position = " << initial_candidate << "\t" << std::to_string(initial_candidate / sum) << "% \n" <<
                    "2 pin net = " << two_pin << "\t" << std::to_string(two_pin / sum) << "% \t" << std::to_string(two_pin / move_sum) << "% \n" <<
                    "Center of mass = " << center_of_mass << "\t" << std::to_string(center_of_mass / sum) << "% \t" << std::to_string(center_of_mass / move_sum) << "% \n" <<
                    "Median = " << median << "\t" << std::to_string(median / sum) << "% \t" << std::to_string(median / move_sum) << "% \n" <<
                    "NA" << na << " \t" << std::to_string(na / sum) << "%" << std::endl;*/
    }

    void ILPRouting::save_result()
    {
    	std::ofstream results_file("ilp_results.csv", std::ofstream::out | std::ofstream::app);
        auto total_wirelength = unit_type{0};
        for(auto candidate : m_route_candidate)
        {
                auto variable = m_route_candidate_variables[candidate];
                auto value = variable.get(GRB_DoubleAttr_X);
                if (value == 1)
                {
                auto wirelength = m_route_candidate_wirelengths[candidate];
                total_wirelength += wirelength;
                }
        }
        results_file << m_circuit_name << "," << total_wirelength.value() << std::endl;
    }

    void ILPRouting::create_all_cell_initial_candidates(GRBModel & model){
        auto & netlist = m_design.netlist();

        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++){
            auto cell = *cell_it;
            auto cell_name = m_design.netlist().name(cell);
            if (DEBUG) std::cout << "create candidates for cell " << cell_name << std::endl;
            
            // std::vector<net_type> cell_nets;
            // for(auto pin : m_design.netlist().pins(cell)){
            //     cell_nets.push_back(m_design.netlist().net(pin));
            // }
            
            //initial position
            auto initial_candidate = m_position_candidates.add();
            auto initial_variable_name = cell_name + "_initial";
            GRBVar initial_variable;
            if (m_integer) {
                initial_variable = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, initial_variable_name);
            } else {
                initial_variable = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, initial_variable_name);
            }
            m_position_candidate_names[initial_candidate] = initial_variable_name;
            m_position_candidate_cell[initial_candidate] = cell;
            m_name_to_position_candidate[initial_variable_name] = initial_candidate;
            m_position_candidate_variables[initial_candidate] = initial_variable;
            m_position_candidate_position[initial_candidate] = m_design.placement().location(cell);
            m_position_candidate_origin[initial_candidate] = candidate_origin_type::INITIAL;
            m_cell_initial_candidate[cell] = initial_candidate;
        }

    }

    void ILPRouting::add_movements_constraints(GRBModel & model){
        auto max_movemnts = m_design.routing_constraints().max_cell_movement();
        GRBLinExpr candicates = 0.0;
        for (auto candidate_it = m_position_candidates.begin(); candidate_it != m_position_candidates.end(); candidate_it++) {
            auto candidate = *candidate_it;        
            auto name = m_position_candidate_names[candidate];
            if (name.find("initial") == std::string::npos) {
                candicates += m_position_candidate_variables[candidate];
            }
        }
        model.addConstr(candicates <= max_movemnts, "max_movements");
    }
}
