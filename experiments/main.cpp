#include <iostream>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/routing/AStarRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/log.h>
#include <ophidian/routing/MCFMultiThreading.h>
#include <ophidian/routing/Engine.h>
#include <chrono>


using namespace std;
using namespace ophidian::util;
// int THREADS_DEFAULT_VALUE = 1;

// std::chrono::time_point<std::chrono::steady_clock> start_time;
using gcell_type = ophidian::routing::GlobalRouting::gcell_type;

void calculate_median_gcell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, std::vector<gcell_type> & target_gcells)
{
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;

    auto & netlist = design.netlist();
    auto & placement = design.placement();
    std::vector<net_type> cell_nets;
    
    std::vector<double> x_positions;
    std::vector<double> y_positions;

    std::vector<point_type> median_points;
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
            
            //median_points.push_back({location.x(), location.y()});
        }
        x_positions.push_back(x_min);
        x_positions.push_back(x_max);
        y_positions.push_back(y_min);
        y_positions.push_back(y_max);
    }
    auto cell_location = placement.location(cell);
    auto current_gcell = design.global_routing().gcell_graph()->nearest_gcell(cell_location, 0);

    if(x_positions.empty() || y_positions.empty()) {
        //return current_gcell;
        return;
    }

    std::nth_element(x_positions.begin(), x_positions.begin() + x_positions.size()/2, x_positions.end());
    auto median_x = x_positions[x_positions.size()/2];
    //auto median_x_left = x_positions[x_positions.size()/2];
    //auto median_x_right = x_positions[x_positions.size()/2 + 1];
    std::nth_element(y_positions.begin(), y_positions.begin() + y_positions.size()/2, y_positions.end());
    auto median_y = y_positions[y_positions.size()/2];
    //auto median_y_left = y_positions[y_positions.size()/2];
    //auto median_y_right = y_positions[y_positions.size()/2 + 1];

    //point_type median_point {unit_type(median_x), unit_type(median_y)};
    //std::vector<point_type> median_points;
    median_points.push_back({unit_type{median_x}, unit_type{median_y}});
    /*median_points.push_back({unit_type{median_x+10}, unit_type{median_y}});
    median_points.push_back({unit_type{median_x-10}, unit_type{median_y}});
    median_points.push_back({unit_type{median_x}, unit_type{median_y+10}});
    median_points.push_back({unit_type{median_x}, unit_type{median_y-10}});*/
    /*median_points.push_back({unit_type{median_x_left}, unit_type{median_y_left}});
    median_points.push_back({unit_type{median_x_right}, unit_type{median_y_left}});
    median_points.push_back({unit_type{median_x_left}, unit_type{median_y_right}});
    median_points.push_back({unit_type{median_x_right}, unit_type{median_y_right}});*/

    auto current_location = design.placement().location(cell);
    /*median_points.push_back({unit_type{current_location.x().value() + 10}, unit_type{current_location.y().value()}});
    median_points.push_back({unit_type{current_location.x().value() - 10}, unit_type{current_location.y().value()}});
    median_points.push_back({unit_type{current_location.x().value()}, unit_type{current_location.y().value() + 10}});
    median_points.push_back({unit_type{current_location.x().value()}, unit_type{current_location.y().value() - 10}});*/

    for (auto median_point : median_points) {
        auto nearest_gcell = design.global_routing().gcell_graph()->nearest_gcell(median_point, 0);
        
        target_gcells.push_back(nearest_gcell);
    }

    /*if(current_gcell != nearest_gcell){
        return nearest_gcell;
    }
    return current_gcell;*/
}

double test_target_gcell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, gcell_type & initial_gcell, gcell_type & target_gcell, ophidian::routing::AStarRouting & astar_routing) {
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;
    using AStarSegment = ophidian::routing::AStarSegment;

    auto& global_routing = design.global_routing();
    auto& netlist = design.netlist();
    auto& placement = design.placement();
    auto& routing_constr = design.routing_constraints();
    auto& std_cells = design.standard_cells();
    auto& routing_library = design.routing_library();
    auto gcell_graph_ptr = global_routing.gcell_graph();

    ophidian::routing::AStarRouting::box_type chip_area{design.floorplan().chip_origin(), design.floorplan().chip_upper_right_corner()};

    // Get connected nets
    std::vector<net_type> cell_nets;
    for(auto pin : netlist.pins(cell))
    {
        auto net = netlist.net(pin);
        if(net == ophidian::circuit::Net())
            continue;
        cell_nets.push_back(net);
    }

    //move cell to median
    auto overflow_movement = global_routing.move_cell(initial_gcell, target_gcell, cell, netlist, placement, routing_constr, std_cells);
    if(overflow_movement)
        return std::numeric_limits<double>::max();

    //BEGIN:Pin access check before routing
    std::unordered_map<ophidian::routing::GCell, int, ophidian::entity_system::EntityBaseHash> gcell_pin_count;
    for(auto pin : netlist.pins(cell))
    {
        auto pin_location = placement.location(pin);
        auto pin_geometry = placement.geometry(pin);
        auto pin_layer_name = pin_geometry.front().second;
        auto pin_layer = routing_library.find_layer_instance(pin_layer_name);
        auto pin_layer_index = routing_library.layerIndex(pin_layer);
        auto gcell = gcell_graph_ptr->nearest_gcell(pin_location, pin_layer_index-1);
        gcell_pin_count[gcell]++;
    }
    for(auto map_pair : gcell_pin_count)
    {
        auto cap = gcell_graph_ptr->capacity(map_pair.first);
        auto demand = gcell_graph_ptr->demand(map_pair.first);
        if(cap < (demand + gcell_pin_count[map_pair.first]))
            return std::numeric_limits<double>::max();
    }
    //END: Pin access check before routing

    
    for(auto net : cell_nets)
    {
        std::vector<AStarSegment> segments;
        bool result = astar_routing.route_net(net, segments, chip_area);
        if(result == false)
            return std::numeric_limits<double>::max();
    }
    return global_routing.wirelength(cell_nets);
}

bool move_cell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, ophidian::routing::AStarRouting & astar_routing)
{
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;
    using AStarSegment = ophidian::routing::AStarSegment;

    auto& global_routing = design.global_routing();
    auto& netlist = design.netlist();
    auto& placement = design.placement();
    auto& routing_constr = design.routing_constraints();
    auto& std_cells = design.standard_cells();
    auto gcell_graph_ptr = global_routing.gcell_graph();

    auto initial_location = placement.location(cell);
    auto initial_gcell = gcell_graph_ptr->nearest_gcell(initial_location, 0);
    std::vector<gcell_type> target_gcells;
    //auto median_gcell = calculate_median_gcell(design, cell);
    calculate_median_gcell(design, cell, target_gcells);

    // Get connected nets
    std::vector<net_type> cell_nets;
    for(auto pin : netlist.pins(cell))
    {
        auto net = netlist.net(pin);
        if(net == ophidian::circuit::Net())
            continue;
        cell_nets.push_back(net);

    }
    // Backup routing information
    std::vector<AStarSegment> initial_segments;
    auto wirelength_before = global_routing.wirelength(cell_nets);
    for(auto net : cell_nets)
        for(auto segment : global_routing.segments(net))
            initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));

    auto ex_demand_initial_gcell = global_routing.extra_demand_neighborhood(initial_gcell);

    auto min_wirelength = wirelength_before;
    auto best_gcell = gcell_type{};
    std::vector<ophidian::routing::ExtraDemandGCell> ex_demand_best_gcell = ex_demand_initial_gcell;
    for (auto target_gcell : target_gcells)
    {
        if (initial_gcell == target_gcell)
            continue;
        auto ex_demand_target_gcell = global_routing.extra_demand_neighborhood(target_gcell);
        for (auto net : cell_nets)
            global_routing.unroute(net);

        auto wirelength = test_target_gcell(design, cell, initial_gcell, target_gcell, astar_routing);
        if (wirelength < min_wirelength)
        {
            min_wirelength = wirelength;
            best_gcell = target_gcell;
	    ex_demand_best_gcell = ex_demand_target_gcell;
        } else {
        
            for(auto net : cell_nets)
                global_routing.unroute(net);

            global_routing.restore_movement(target_gcell, initial_gcell, cell, netlist, placement, routing_constr, std_cells, ex_demand_initial_gcell, ex_demand_target_gcell);
        bool undo = astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
        if(undo == false)
            std::cout<<"WARNING: UNDO ROUTING OVERFLOW!"<<std::endl;
        }

        
    }
    if (best_gcell == gcell_type{})
        return false;
    /*for (auto net : cell_nets)
        global_routing.unroute(net);
    auto result = test_target_gcell(design, cell, initial_gcell, best_gcell, astar_routing);
    if (result == false)
    {
        std::cout << "WARNING: FAILED TO MOVE TO BEST GCELL" << std::endl;
        return false;
    }*/
    return true;
}

bool cell_has_more_than_1_pin_in_same_net(ophidian::design::Design & design, ophidian::circuit::CellInstance cell)
{
    auto & netlist = design.netlist();
    std::unordered_map<std::string, int> pin_map;
    for(auto pin : netlist.pins(cell))
    {
        auto net = netlist.net(pin);
        auto net_name = netlist.name(net);
        pin_map[net_name]++;
    }
    for(auto map_it : pin_map)
        if(map_it.second > 1)
            return true;
    return false;
}

std::vector<std::pair<ophidian::circuit::CellInstance, double>> compute_cell_move_costs_descending_order(ophidian::design::Design & design)
{
    auto & netlist = design.netlist();
    auto & placement = design.placement();
    std::vector<std::pair<ophidian::circuit::CellInstance, double>> cells_costs;

    for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); ++cell_it)
    {
        auto cell = *cell_it;
        if(placement.isFixed(cell))
            continue;
        if(cell_has_more_than_1_pin_in_same_net(design, cell))
            continue;

        std::unordered_set<ophidian::circuit::Net, ophidian::entity_system::EntityBaseHash> cell_nets;
        for(auto pin : netlist.pins(cell))
        {
            auto net = netlist.net(pin);
            cell_nets.insert(net);
        }

        double routed_length = 0;
        double lower_bound = 0;
        for(auto net : cell_nets)
        {
            routed_length += design.global_routing().wirelength(net);
            //lower_bound += design.global_routing().lower_bound_wirelength(net);
        }
        auto cost = routed_length;
        //auto cost = routed_length / lower_bound;
        //auto cost = 1;
        cells_costs.push_back(std::make_pair(cell, cost));
    }
    //SORT IN DESCENDING ORDER
    std::sort(cells_costs.begin(), cells_costs.end(), [](std::pair<ophidian::circuit::CellInstance, double> cost_a, std::pair<ophidian::circuit::CellInstance, double> cost_b) {return cost_a.second > cost_b.second;});
    return cells_costs;
}

void move_cells_for_until_x_minutes(ophidian::design::Design & design,
                                    int time_limit,
                                    std::vector<std::pair<ophidian::circuit::CellInstance, double>> & cells,
                                    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements,
                                    ophidian::routing::AStarRouting & astar_routing)
{
    /*auto debug_gcell = design.global_routing().gcell_graph()->gcell(20, 19, 0);
    auto capacity = design.global_routing().gcell_graph()->capacity(debug_gcell);
    auto demand = design.global_routing().gcell_graph()->demand(debug_gcell);
    auto layer_index = design.global_routing().gcell_graph()->layer_index(debug_gcell);
    auto gcell_box = design.global_routing().gcell_graph()->box(debug_gcell);

    std::cout << "debug gcell " << gcell_box.min_corner().y().value() << " " << gcell_box.min_corner().x().value() << " " << layer_index << std::endl;
    std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/

    auto start_time = std::chrono::steady_clock::now();
    auto nets = std::vector<ophidian::circuit::Net>{design.netlist().begin_net(), design.netlist().end_net()};
    //auto wirelength = design.global_routing().wirelength(nets);
    //std::cout << "initial wirelength " << wirelength << std::endl;

    int moved_cells = movements.size();
    if(moved_cells == design.routing_constraints().max_cell_movement()) {
        return;
    }
    for(auto pair : cells)
    {
        auto cell = pair.first;
        if (design.placement().isFixed(cell)) {
            continue;
        }
        auto cell_name = design.netlist().name(cell);
        //std::cout << "cell " << cell_name << std::endl;
        auto moved = move_cell(design, cell, astar_routing);
        if(moved)
        {
            auto location = design.placement().location(cell);
            moved_cells++;            
            movements.push_back(std::make_pair(cell, design.placement().location(cell)));
            //std::cout << "moved to " << location.x().value() << "," << location.y().value() << std::endl;
            //std::cout<<"# of moved cells: "<<moved_cells<<std::endl;

            //auto wirelength = design.global_routing().wirelength(nets);
            //std::cout << "wirelength " << wirelength << std::endl;
        }
    
        /*auto capacity = design.global_routing().gcell_graph()->capacity(debug_gcell);
        auto demand = design.global_routing().gcell_graph()->demand(debug_gcell);
        std::cout << "debug gcell " << gcell_box.min_corner().y().value() << " " << gcell_box.min_corner().x().value() << " " << layer_index << std::endl;
        std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end_time-start_time;
        //std::cout << "time " << diff.count() << std::endl;
        bool time_out = diff.count() > time_limit * 60.0 ? true : false;
        //if (cell_name == "C1245") break;
        //if(moved_cells == design.routing_constraints().max_cell_movement() || time_out)
        if(moved_cells == design.routing_constraints().max_cell_movement() || time_out)
        //if(moved_cells == 1)
            break;
    }
}

void comput_lower_bound(ophidian::design::Design & design, ophidian::routing::AStarRouting & astar_routing)
{
    ophidian::routing::AStarRouting::box_type chip_area{design.floorplan().chip_origin(), design.floorplan().chip_upper_right_corner()};
    //compute lower bound using A* without capacities!
    // ophidian::routing::AStarRouting astar_routing{design, false};
    // #pragma omp parallel for num_threads(4) private(astar_routing)
    for(auto net_it = design.netlist().begin_net(); net_it != design.netlist().end_net(); ++net_it)
    {
        auto net = *net_it;
        std::vector<ophidian::routing::AStarSegment> segments;
        astar_routing.route_net(net, segments, chip_area, false, false);
        //calculing wirelength
        std::vector<ophidian::routing::GCell> gcells;
        for(auto segment : segments)
        {
            auto layer_start_index = design.routing_library().layerIndex(segment.start_layer);
            auto layer_end_index = design.routing_library().layerIndex(segment.end_layer);
            auto min_index = std::min(layer_start_index, layer_end_index);
            auto max_index = std::max(layer_start_index, layer_end_index);
            for(auto i = min_index; i <= max_index; i++)
            {
                auto box = segment.wire_box;
                design.global_routing().gcell_graph()->intersect(gcells, box, i-1);
            }
        }
        //remove duplicated gcells
        std::sort(gcells.begin(), gcells.end(), [&](auto &lhs, auto &rhs){return design.global_routing().gcell_graph()->id(lhs) < design.global_routing().gcell_graph()->id(rhs);});
        gcells.erase(std::unique(gcells.begin(), gcells.end()), gcells.end());

        design.global_routing().lower_bound_wirelength(net, gcells.size());
    }

    //comparison of steiner tree
    // ophidian::entity_system::Property<ophidian::circuit::Net, int> net_stwl{design.netlist().make_property_net<int>(0)};
    // for (auto net_it = design.netlist().begin_net(); net_it != design.netlist().end_net(); ++net_it)
    // {
    //     auto net = *net_it;
    //     auto pins = design.netlist().pins(net);

    //     std::vector<ophidian::interconnection::Flute::Point> net_points;
    //     net_points.reserve(pins.size());
    //     for (auto pin : pins) {
    //         auto pin_location = design.placement().location(pin);
    //         net_points.push_back(pin_location);
    //     }

    //     auto & flute = ophidian::interconnection::Flute::instance();
    //     auto tree = flute.create(net_points);
    //     auto stwl = tree->length().value();
    //     stwl /= 10;
    //     if (stwl == 0) {
    //         stwl = 1;
    //     }
    //     net_stwl[net] = stwl;
    // }

    // std::cout << "net,stwl,lowerBound" << std::endl;
    // for (auto net_it = design.netlist().begin_net(); net_it != design.netlist().end_net(); ++net_it)
    // {
    //     auto net = *net_it;
    //     auto net_name = design.netlist().name(net);
    //     std::cout << net_name << "," << net_stwl[net] << "," << design.global_routing().lower_bound_wirelength(net) << std::endl;
    // }
}

void run_astar_for_nets(ophidian::design::Design & design, std::vector<ophidian::circuit::Net> & nets, ophidian::routing::AStarRouting & astar_routing) {
    ophidian::routing::AStarRouting::box_type chip_area{design.floorplan().chip_origin(), design.floorplan().chip_upper_right_corner()};
    for (auto & net : nets) {
        std::vector<ophidian::routing::AStarSegment> initial_segments;
        for(auto segment : design.global_routing().segments(net)) {
            initial_segments.push_back(ophidian::routing::AStarSegment(design.global_routing().box(segment), design.global_routing().layer_start(segment), design.global_routing().layer_end(segment), net));
        }
        design.global_routing().unroute(net);
        std::vector<ophidian::routing::AStarSegment> segments;
        auto result = astar_routing.route_net(net, segments, chip_area, false);
        if (result) {
            //log() << "applying segments" << std::endl;
            bool apply = astar_routing.apply_segments_to_global_routing(segments);
            if (!apply) {
                std::cout << "WARNING: FAILED TO APPLY" << std::endl;
            }
        } else {
            //log() << "undo segments" << std::endl;
            bool undo = astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
            if(!undo) {
                std::cout<<"WARNING: UNDO ROUTING FAILED, THIS SHOULD NEVER HAPPEN!"<<std::endl;
                //break;
            }
        }
    }
}

void greetings(){
    using std::endl;

    printlog("===================================================");
    printlog("              ICCAD 2020 CAD Contest               ");
    printlog("      Problem B : Routing with Cell Movement       ");
    printlog("            Team Number:   cada0194                ");
    printlog("              Team Name:      CABRA                ");
    printlog("                    Members:                       ");
    printlog("Affiliation:  University of Calgary                ");
    printlog("    Erfan Aghaeekiasaraee                          ");
    printlog("    Upma Gandhi                                    ");
    printlog("    Laleh Behjat                                   ");
    printlog("Affiliation:  Federal University of Santa Catarina ");
    printlog("    Arthur Philippi Bianco                         ");
    printlog("    Renan Oliveira Netto                           ");
    printlog("    Sheiny Fabre Almeida                           ");
    printlog("    Tiago Augusto Fontana                          ");
    printlog("    Vinicius Livramento                            ");
    printlog("    Cristina Meinhardt                             ");
    printlog("    Jose Luis Guntzel                              ");
    printlog("===================================================");
};

void run_mcf_for_circuit(ophidian::design::Design & design, std::string circuit_name, std::string output){
    /*ophidian::routing::AStarRouting astar_routing{design};
    auto& netlist = design.netlist();
    for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++) {
        auto net = *net_it;
        auto net_name = design.netlist().name(net);
        std::cout << "net " << net_name << std::endl;
        design.global_routing().unroute(net);
        std::vector<ophidian::routing::AStarSegment> segments;    
        astar_routing.route_net(net, segments);
    }*/

    // UCal::MCFRouting mcf_routing(design,circuit_name);
    
    auto start_time = std::chrono::steady_clock::now();
    
    ophidian::routing::AStarRouting astar_routing{design};

    std::vector<ophidian::circuit::Net> nets{design.netlist().begin_net(), design.netlist().end_net()};

    std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements;


    design.global_routing().set_gcell_cell_instances(design.netlist(), design.placement());

    // UCal::MCFMultiThreading mcf_multi_threading(design); 
    // mcf_multi_threading.run(movements);

    UCal::Engine engine(design);
    engine.run(start_time);
    
    //run_astar_for_nets(design, nets, astar_routing);
    
    //log() << "movements after ILP " << movements.size() << std::endl;

    if(true){
        auto end_time = std::chrono::steady_clock::now();

        std::chrono::duration<double> diff = end_time-start_time;
        auto current_time = diff.count() / 60.0;
        auto remaining_time = 55 - current_time;

        log() << "current time " << current_time << " remaining time " << remaining_time << std::endl;
        
        for (auto movement : movements) {
            auto cell = movement.first;
            design.placement().fixLocation(cell);
        }


        //compute lower bound using A* without capacities!
        /*ophidian::routing::AStarRouting astar_routing{design};
        printlog();
        comput_lower_bound(design, astar_routing);
        printlog();*/

        auto cell_costs = compute_cell_move_costs_descending_order(design);

        /*auto debug_gcell = design.global_routing().gcell_graph()->gcell(23, 10, 0);
        auto capacity = design.global_routing().gcell_graph()->capacity(debug_gcell);
        auto demand = design.global_routing().gcell_graph()->demand(debug_gcell);
        auto layer_index = design.global_routing().gcell_graph()->layer_index(debug_gcell);
        auto gcell_box = design.global_routing().gcell_graph()->box(debug_gcell);

        std::cout << "debug gcell before moves " << gcell_box.min_corner().y().value() << " " << gcell_box.min_corner().x().value() << " " << layer_index << std::endl;
        std::cout << "debug gcell before moves capacity " << capacity << " demand " << demand << std::endl;*/

        move_cells_for_until_x_minutes(design, remaining_time, cell_costs, movements, astar_routing);

    }//end if 
    
        
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    iccad_output_writer.write_ICCAD_2020_output(output, movements);

    log() << "number of movements: " << movements.size() << std::endl;

    log() << "end" << std::endl;
}//end run_mcf_for_circuit

void run_for_circuit(ophidian::design::Design & design, std::string circuit_name, std::string output) {
    auto & netlist = design.netlist();
    auto& global_routing = design.global_routing();
    ophidian::routing::AStarRouting astar_routing{design};
    //ophidian::routing::ILPRouting ilpRouting(design, circuit_name);
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<ophidian::circuit::Net> fixed_nets;
    std::vector<ophidian::circuit::Net> routed_nets;

    //std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements;
    // std::log() << "routing nets" << std::endl;
    //auto result = ilpRouting.route_nets(nets, fixed_nets, routed_nets, movements);
    // std::log() << "result " << result << std::endl;


    for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++)
    {
        global_routing.unroute(*net_it);
        std::vector<ophidian::routing::AStarSegment> segments;
        //astar_routing.route_net(*net_it, segments);
    }


    // auto cells = compute_cell_move_costs_descending_order(design);
    // move_cells_for_until_x_minutes(design, 40, cells, movements, astar_routing);

    
    
    // for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++)
    // {
    //     global_routing.unroute(*net_it);
    //     std::vector<ophidian::routing::AStarSegment> segments;
    //     astar_routing.route_net(*net_it, segments);
    // }





    iccad_output_writer.write_ICCAD_2020_output(output, {});
    // if(result.first){
    //     iccad_output_writer.write_ICCAD_2020_output(output, movements);
    // }

    // std::log() << "connected nets" << std::endl;
    // for (auto net : nets) {
    //     ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
    //     for (auto pin : design.netlist().pins(net)) {
    //         auto pin_name = design.netlist().name(pin);                
    //         auto location = design.placement().location(pin);
    //         auto box = ophidian::routing::GCellGraph::box_type{location, location};
    //         auto pin_geometry = design.placement().geometry(pin);
    //         auto layer_name = pin_geometry.front().second;
    //         auto pin_layer = design.routing_library().find_layer_instance(layer_name);
    //         auto layer_index = design.routing_library().layerIndex(pin_layer);

    //         // std::log() << "pin " << pin_name << " layer " << layer_name << " index " << layer_index << std::endl;

    //         design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index-1);
    //     }
    //     auto connected = design.global_routing().is_connected(net, pin_gcells);

    //     auto net_name = design.netlist().name(net);
    //     if(!connected)
    //         std::log() << "net " << net_name << " is open" << std::endl;
    // }
}

bool test_input(const std::string input_file)
{
    if(input_file.find(".txt") != std::string::npos)
        return true;
    return false;
}

std::string extract_circuit_name(const std::string input_file)
{
    // Find the position of first delimiter ( last / )
    int firstDelPos = input_file.rfind("/");
    // Find the position of second delimiter
    int secondDelPos = input_file.rfind(".txt");
    auto circuit = input_file.substr(firstDelPos + 1, secondDelPos - firstDelPos - 1);
    return circuit;
}

#include "date21.h"

int main(int argc, char** argv) {
    //start_time = std::chrono::steady_clock::now();
    auto time_begin = std::chrono::high_resolution_clock::now();   

    greetings();

    bool input_found{false};
    string input_file{};
    string circuit_name{};
    int experiment = (argc == 4)? std::atoi(argv[3]) : -1;

    bool output_found{false};
    string output{};

    // HELP
    if (argc == 1 || string(argv[1]) == "-h" || string(argv[1]) == "-help" || string(argv[1]) == "--help")
    {
        log() << "usage:" << endl;
        log() << "./cell_move_router <input.txt> <output.txt>" << endl;
        return 0;
    }

    if (argc < 2)
    {
        log() << "Cannot proceed, missing inputn and/or output file name" << endl;
        log() << "For help, set --help or -help or -h" << endl;
        log() << "usage: ./cell_move_router <input.txt> <output.txt>" << endl;
        return 0;
    } 

    if (argv[1])
    {
        input_file = argv[1];
        if(test_input(input_file))
        {
            input_found = true;
            circuit_name = extract_circuit_name(input_file);
        }
    }

    if (argv[2])
    {
        output = argv[2];
        if(test_input(output))
        {
            output_found = true;
        }
    }

    // must have flags:
    if (input_file == "")
    {
        log() << "Cannot proceed, missing Input file" << endl;
        log() << "For help, set --help or -help or -h" << endl;
        return 0;
    }

    if (output == "")
    {
        log() << "Cannot proceed, missing output file name" << endl;
        log() << "For help, set --help or -help or -h" << endl;
        return 0;
    }

    auto iccad_2020 = ophidian::parser::ICCAD2020{input_file};

    auto design = ophidian::design::Design();
    ophidian::design::factory::make_design_iccad2020(design, iccad_2020);

    auto time_end = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> movements;
    //ophidian::routing::AStarRouting astar_routing{design};
    auto engine = UCal::Engine(design);
    std::vector<std::pair<ophidian::circuit::CellInstance, double>> cells;
    
    // "exp1" --> "Astar_without_paneling_and_without_movements"
    // "exp2" --> "Astar_with_paneling_and_without_movements"
    // "exp3" --> "Astar_with_paneling_and_with_movements"
    // "exp4" --> "Astar_with_paneling_and_with_movements_parallel"

    // "exp5" --> "ILP_without_movements_Astar_without_movements"
    // "exp6" --> "ILP_with_movements_Astar_with_movements"
    // "exp7" --> "ILP_with_movements_Astar_with_movements_parallel"

    switch (experiment)
    {
    case 1:
        time_begin = std::chrono::high_resolution_clock::now();
        Astar_without_paneling_and_without_movements(design,circuit_name, output);
        time_end = std::chrono::high_resolution_clock::now();
        break;
    case 2:
        time_begin = std::chrono::high_resolution_clock::now();
        Astar_with_paneling_and_without_movements(design,circuit_name, output);
        time_end = std::chrono::high_resolution_clock::now();
        break;
    case 3:
        time_begin = std::chrono::high_resolution_clock::now();
        Astar_with_paneling_and_with_movements(design,circuit_name, output);
        time_end = std::chrono::high_resolution_clock::now();
        break;
    case 4:
        time_begin = std::chrono::high_resolution_clock::now();
        Astar_with_paneling_and_with_movements_parallel(design,circuit_name, output);
        time_end = std::chrono::high_resolution_clock::now();
        break;
    case 5:
        time_begin = std::chrono::high_resolution_clock::now();
        ILP_without_movements_Astar_without_movements(design,circuit_name, output, movements);
        time_end = std::chrono::high_resolution_clock::now();
        break;
    case 6:
        time_begin = std::chrono::high_resolution_clock::now();
        ILP_with_movements_Astar_with_movements(design,circuit_name, output, movements);
        //move_cells_for_until_x_minutes(design, 1000000, cells, movements, astar_routing);
        time_end = std::chrono::high_resolution_clock::now();
        break;
    case 7:
        time_begin = std::chrono::high_resolution_clock::now();
        ILP_with_movements_Astar_with_movements_parallel(design,circuit_name, output, movements, engine);
        std::cout << "movements " << movements.size() << std::endl;
        for (auto movement : movements) {
            auto cell = movement.first;
            design.placement().fixLocation(cell);
        }
        cells = compute_cell_move_costs_descending_order(design);
        move_cells_for_until_x_minutes(design, 1000000, cells, movements, engine.astar_routing());
        time_end = std::chrono::high_resolution_clock::now();
        break;
    default:
        //run the code sent to the contest;
        time_begin = std::chrono::high_resolution_clock::now();
        run_mcf_for_circuit(design,circuit_name, output);
        time_end = std::chrono::high_resolution_clock::now();
        break;
    }

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_end-time_begin).count();
    auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(time_end-time_begin).count();
    std::cout << "Total run_time in: " << duration_s << " seconds | or | " << duration_ms << " milliseconds" << std::endl;
    
    //Write the csv file
    auto csv_file = "exp" + std::to_string(experiment) + "_initial_results.csv";
    write_csv_header(csv_file);
    write_csv(design, circuit_name, csv_file, duration_s);

    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);
    iccad_output_writer.write_ICCAD_2020_output(output, movements);

    return 0;
}
