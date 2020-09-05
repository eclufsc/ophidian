
#include "run_ilp.h"

#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/log.h>

#include <ophidian/routing/MCFMultiThreading.h>

using namespace ophidian::util;
using point_type = ophidian::util::LocationDbu;
using unit_type = ophidian::util::database_unit_t;
using cell_map = std::unordered_map<ophidian::circuit::CellInstance, bool, ophidian::entity_system::EntityBaseHash>;
bool DEBUG_TEST = true;

void run_mcf_multithreading(ophidian::design::Design & design) {
    UCal::MCFMultiThreading mcf(design);

    std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements;
    mcf.run(movements);
}

point_type cell_median_location(ophidian::circuit::CellInstance & cell, ophidian::design::Design & design) {
    std::vector<double> x_positions;
    std::vector<double> y_positions;

    for(auto pin : design.netlist().pins(cell)){
        auto net = design.netlist().net(pin);
        if(net == ophidian::circuit::Net())
            continue;
        auto x_min = std::numeric_limits<double>::max();
        auto y_min = std::numeric_limits<double>::max();
        auto x_max = -std::numeric_limits<double>::max();
        auto y_max = -std::numeric_limits<double>::max();
        for(auto net_pin : design.netlist().pins(net)){
            if(net_pin == pin)
                continue;
            auto location = design.placement().location(net_pin);
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
        return design.placement().location(cell);

    std::nth_element(x_positions.begin(), x_positions.begin() + x_positions.size()/2, x_positions.end());
    auto median_x = x_positions[x_positions.size()/2];
    //median_x = std::min(area.max_corner().x().value(), std::max(area.min_corner().x().value(), median_x));
    std::nth_element(y_positions.begin(), y_positions.begin() + y_positions.size()/2, y_positions.end());
    auto median_y = y_positions[y_positions.size()/2];
    //median_y = std::min(area.max_corner().y().value(), std::max(area.min_corner().y().value(), median_y));

    point_type median_point {unit_type(median_x), unit_type(median_y)};

    return median_point;
}

double measure_cell_wirelength(ophidian::circuit::CellInstance & cell, ophidian::design::Design & design) {   
    double cell_wirelength = 0;
    for (auto cell_pin : design.netlist().pins(cell)) {
        auto net = design.netlist().net(cell_pin);

        auto pins = design.netlist().pins(net);
        std::vector<ophidian::interconnection::Flute::Point> net_points;
        net_points.reserve(pins.size());
        for (auto pin : pins) {
            auto pin_location = design.placement().location(pin);
            net_points.push_back(pin_location);
        }

        auto & flute = ophidian::interconnection::Flute::instance();
        auto tree = flute.create(net_points);
        auto stwl = tree->length().value();
        stwl /= 10;
        if (stwl == 0) {
            stwl = 1;
        }        

        cell_wirelength += stwl;
    }
    return cell_wirelength;
}

std::pair<bool, typename ophidian::routing::ILPRouting<IloBoolVar>::Statistics> run_ilp_for_part_of_nets(const std::vector<std::pair<ophidian::circuit::Net, double>> & nets_costs, unsigned start_index, unsigned end_index, ophidian::routing::ILPRouting<IloBoolVar> & ilpRouting, std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements, bool initial_routing, const std::vector<ophidian::circuit::CellInstance> & cells, ophidian::placement::Placement::box_type & area) {    
    std::vector<ophidian::circuit::Net> nets_to_route;
    std::vector<ophidian::circuit::Net> fixed_nets;
    for (unsigned index = 0; index < nets_costs.size(); index++) {
        auto net_cost = nets_costs.at(index);
        if (index >= start_index && index < end_index) {
            nets_to_route.push_back(net_cost.first);
        } else {
            fixed_nets.push_back(net_cost.first);
        }
    }

    std::cout << "nets to route " << nets_to_route.size() << std::endl;
    
    std::vector<ophidian::circuit::Net> routed_nets;
    std::vector<ophidian::circuit::Net> unrouted_nets;
    
    auto result = ilpRouting.route_nets(nets_to_route, cells, area, fixed_nets, routed_nets, unrouted_nets, movements, initial_routing);
    return result;
}

std::pair<bool, typename ophidian::routing::ILPRouting<IloBoolVar>::Statistics> run_ilp_for_part_of_cells(const std::vector<std::pair<ophidian::circuit::CellInstance, double>> & cells_costs, unsigned start_index, unsigned end_index, ophidian::routing::ILPRouting<IloBoolVar> & ilpRouting, std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements, bool initial_routing, const std::vector<ophidian::circuit::Net> & fixed_nets, ophidian::placement::Placement::box_type & area, cell_map & initial_fixed, ophidian::design::Design & design) {    
    std::vector<ophidian::circuit::CellInstance> cells;
    for (unsigned index = 0; index < cells_costs.size(); index++) {
        auto cell_cost = cells_costs.at(index);
        cells.push_back(cell_cost.first);
        //if (!(index >= start_index && index < end_index)) {
        if (index >= end_index) {
            design.placement().fixLocation(cell_cost.first);
        }
    }

    std::cout << "cells " << cells.size() << std::endl;
    
    std::vector<ophidian::circuit::Net> routed_nets;
    std::vector<ophidian::circuit::Net> unrouted_nets;
    
    auto result = ilpRouting.route_nets(fixed_nets, cells, area, {}, routed_nets, unrouted_nets, movements, initial_routing, true);

    for (auto cell_cost : cells_costs) {
        auto cell = cell_cost.first;
        if (initial_fixed[cell]) {
            design.placement().fixLocation(cell);
        } else {
            design.placement().unfixLocation(cell);
        }
    }
        
    return result;
}

void run_ilp_for_circuit(ophidian::design::Design & design, std::string circuit_name, std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements, bool initial_routing) {
    if(DEBUG_TEST) log() << "starting function run_ilp_for_circuit" << std::endl;
    ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(design, circuit_name);
    if(DEBUG_TEST) log() << "create writer" << std::endl;
    // ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<ophidian::circuit::CellInstance> cells(design.netlist().begin_cell_instance(), design.netlist().end_cell_instance());

    cell_map initial_fixed;
    for (auto cell : cells) {
        initial_fixed[cell] = design.placement().isFixed(cell);
    }
    
    auto chip_origin = design.floorplan().chip_origin();
    auto chip_upper_right_corner = design.floorplan().chip_upper_right_corner();
    auto chip_area = ophidian::placement::Placement::box_type{chip_origin, chip_upper_right_corner};
    
    std::vector<std::pair<ophidian::circuit::CellInstance, double>> cells_costs;
    for (auto cell : cells) {
        auto initial_location = design.placement().location(cell); 

        auto initial_wirelength = measure_cell_wirelength(cell, design);

        auto new_location = cell_median_location(cell, design);
        design.placement().place(cell, new_location);

        auto new_wirelength = measure_cell_wirelength(cell, design);

        auto cost = initial_wirelength / new_wirelength;
        cells_costs.push_back(std::make_pair(cell, cost));

        design.placement().place(cell, initial_location);
    }    
    
    std::sort(cells_costs.begin(), cells_costs.end(), [](std::pair<ophidian::circuit::CellInstance, double> cost_a, std::pair<ophidian::circuit::CellInstance, double> cost_b) {return cost_a.second > cost_b.second;});

    std::vector<std::pair<ophidian::circuit::Net, double>> nets_costs;
    for (auto net : nets) {
        auto pins = design.netlist().pins(net);
        std::vector<ophidian::interconnection::Flute::Point> net_points;
        net_points.reserve(pins.size());
        for (auto pin : pins) {
            auto pin_location = design.placement().location(pin);
            net_points.push_back(pin_location);
        }

        auto & flute = ophidian::interconnection::Flute::instance();
        auto tree = flute.create(net_points);
        auto stwl = tree->length().value();
        stwl /= 10;
        if (stwl == 0) {
            stwl = 1;
        }

        auto routed_length = design.global_routing().wirelength(net);

        auto cost = routed_length / stwl;

        nets_costs.push_back(std::make_pair(net, cost));
    }

    std::sort(nets_costs.begin(), nets_costs.end(), [](std::pair<ophidian::circuit::Net, double> cost_a, std::pair<ophidian::circuit::Net, double> cost_b) {return cost_a.second < cost_b.second;});

    int initial_wirelength = design.global_routing().wirelength(nets);
    if(DEBUG_TEST) log() << "Circuit initial wirelength = " << initial_wirelength << std::endl;

    auto ovfl = design.global_routing().is_overflow() ? "there is overflow" : "No overflow";
    log() << ovfl << " in input file" << std::endl;

    auto demand_before = design.global_routing().gcell_graph()->total_net_demand();
    if(DEBUG_TEST) log() << "routing nets" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<ophidian::circuit::Net> routed_nets;
    std::vector<ophidian::circuit::Net> unrouted_nets;
    
     
    auto result = ilpRouting.route_nets(nets, cells, chip_area, {}, routed_nets, unrouted_nets, movements, initial_routing, false, false);
    
    ilpRouting.route_nets(nets, cells, chip_area, {}, routed_nets, unrouted_nets, movements, initial_routing, true, true);

    std::cout << "movements " << movements.size() << std::endl;

    /*unsigned number_of_cells = cells_costs.size();
    unsigned number_of_cells_per_step = 200;
    unsigned number_of_steps = std::ceil( (double) number_of_cells / (double) number_of_cells_per_step);
    //std::pair<bool, typename ophidian::routing::ILPRouting<IloBoolVar>::Statistics> result;
    for (auto step_index = 0; step_index < number_of_steps; step_index++) {
        unsigned start_index = step_index * number_of_cells_per_step;
        unsigned end_index = start_index + number_of_cells_per_step;
        result = run_ilp_for_part_of_cells(cells_costs, start_index, end_index, ilpRouting, movements, initial_routing, nets, chip_area, initial_fixed, design);

        std::cout << "movements " << movements.size() << std::endl;

        for (auto movement : movements) {
            auto cell = movement.first;
            design.placement().fixLocation(cell);
            initial_fixed[cell] = true;
        }

        if (movements.size() == 820) {
            break;
        }
    }*/

    //auto result = ilpRouting.route_nets(nets_to_route, fixed_nets, routed_nets, unrouted_nets, movements, initial_routing);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if(DEBUG_TEST) log() << "result " << result.first << std::endl;

    if(result.first){
        // need to generate a new guide file
        if(DEBUG_TEST) log() << "Runtime : " << duration.count() << " microsseconds"<< std::endl;
        if(DEBUG_TEST) log() << "Runtime : " << std::to_string( (double)duration.count() / 1000000.0 ) << " seconds"<< std::endl;
        int final_wirelength = design.global_routing().wirelength(nets);
        if(DEBUG_TEST) log() << "Total movements = " << movements.size() << std::endl;
        if(DEBUG_TEST) log() << "Circuit final wirelength = " << final_wirelength << std::endl;
        auto score = initial_wirelength - final_wirelength;
        if(DEBUG_TEST) log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        if(DEBUG_TEST) log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;

        log() <<
            circuit_name << ";" <<
            initial_wirelength << ";" <<
            final_wirelength << ";" <<
            score << ";" <<
            reduction << ";" <<
            movements.size() << ";" << 
            duration.count() << ";" << // total time
            result.second.model_runtime_ms << ";" << //ILP time (ms)
            result.second.model_memory << std::endl; //ILP memory usage

        // iccad_output_writer.write_ICCAD_2020_output("RUN_TESTS_OUTPUT.txt", movements);
    }

    // log() << "movements" << std::endl;
    // for (auto movement : movements) {
    //     auto cell = movement.first;
    //     auto cell_name = design.netlist().name(cell);
    //     auto location = movement.second;
    //     log() << "cell " << cell_name << " " << location.x().value() << "," << location.y().value() << std::endl;
    // }

    ovfl = design.global_routing().is_overflow() ? "there is overflow" : "No overflow";
    log() << ovfl << std::endl;
    ophidian::routing::GlobalRouting::net_container_type ovfl_nets{};
    ovfl = design.global_routing().is_overflow(nets, ovfl_nets) ? "there is overflow" : "No overflow";
    log() << ovfl << std::endl;
    auto demand_after = design.global_routing().gcell_graph()->total_net_demand();
    ophidian::util::mem_use::get_current();
    log() << "Total Demand before: " << demand_before << " Total Demand after: "
    << demand_after << " change: " << demand_before-demand_after << std::endl;
    

    log() << "Checking for disconnected nets..." << std::endl;
    bool is_nets_open = false;
    for (auto net : nets) {
        auto net_name = design.netlist().name(net);
        ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
        for (auto pin : design.netlist().pins(net)) {
            auto pin_name = design.netlist().name(pin);                
            auto location = design.placement().location(pin);
            auto box = ophidian::routing::GCellGraph::box_type{location, location};
            auto pin_geometry = design.placement().geometry(pin);
            auto layer_name = pin_geometry.front().second;
            auto pin_layer = design.routing_library().find_layer_instance(layer_name);
            auto layer_index = design.routing_library().layerIndex(pin_layer);

            // log() << "pin " << pin_name << " layer " << layer_name << " index " << layer_index << std::endl;

            design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index-1);
        }
        auto connected = design.global_routing().is_connected(net, pin_gcells, net_name);

        if(!connected)
        {
            log() << "net " << net_name << " disconnected with " << design.netlist().pins(net).size() << " pins" << std::endl;
            is_nets_open = true;
        }
    }
    if (is_nets_open) printlog("Open nets detected!"); else printlog("All nets connected!");
    log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB"
              << std::endl;
        log() << std::endl;
    printlog("Terminating normally...");
}


void run_circuit(ophidian::design::Design & design, std::string circuit_name) {
    using cell_type = ophidian::circuit::CellInstance;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;

    if(DEBUG_TEST) log() << "starting function run_circuit" << std::endl;
    ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(design, circuit_name);
    ophidian::routing::ILPRouting<IloNumVar> lpRouting(design, circuit_name);

    if(DEBUG_TEST) log() << "create writer" << std::endl;
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    auto chip_origin = design.floorplan().chip_origin();
    auto chip_upper_right_corner = design.floorplan().chip_upper_right_corner();
    auto chip_area = ophidian::placement::Placement::box_type{chip_origin, chip_upper_right_corner};

    std::vector<net_type> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<ophidian::circuit::CellInstance> cells(design.netlist().begin_cell_instance(), design.netlist().end_cell_instance());
    std::vector<net_type> fixed_nets;
    std::vector<net_type> routed_nets;
    std::vector<net_type> unrouted_nets;

    int initial_wirelength = design.global_routing().wirelength(nets);
    if(DEBUG_TEST) log() << "Circuit initial wirelength = " << initial_wirelength << std::endl;

    auto ovfl = design.global_routing().is_overflow() ? "there is overflow" : "No overflow";
    log() << ovfl << " in input file" << std::endl;

    auto demand_before = design.global_routing().gcell_graph()->total_net_demand();
    std::vector<std::pair<cell_type, point_type>> movements; 
    if(DEBUG_TEST) log() << "routing nets" << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); 
    auto result = lpRouting.route_nets(nets, cells, chip_area, fixed_nets, routed_nets, unrouted_nets, movements);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if(DEBUG_TEST) log() << "result " << result.first << std::endl;


    if(result.first){
        // need to generate a new guide file
        if(DEBUG_TEST) log() << "Runtime : " << duration.count() << " microsseconds"<< std::endl;
        if(DEBUG_TEST) log() << "Runtime : " << std::to_string( (double)duration.count() / 1000000.0 ) << " seconds"<< std::endl;
        int final_wirelength = design.global_routing().wirelength(nets);
        if(DEBUG_TEST) log() << "Total movements = " << movements.size() << std::endl;
        if(DEBUG_TEST) log() << "Circuit final wirelength = " << final_wirelength << std::endl;
        auto score = initial_wirelength - final_wirelength;
        if(DEBUG_TEST) log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        if(DEBUG_TEST) log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;

        iccad_output_writer.write_ICCAD_2020_output("RUN_TESTS_OUTPUT.txt", movements);
    }

    auto nets_size = nets.size();
    auto routed_size = routed_nets.size();
    auto unrouted_size = unrouted_nets.size();

    if(nets_size != (routed_size + unrouted_size))
        log() << "NETS SIZE WRONG!!" << std::endl;
        

    if(unrouted_size > 0)
    {
        log();
        log() << "Initing second iteration ..." << std::endl;
        log();

        start = std::chrono::high_resolution_clock::now(); 
        auto result2 = ilpRouting.route_nets(nets, cells, chip_area, fixed_nets, routed_nets, unrouted_nets, movements);
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        log() << "result " << result2.first << std::endl;

        if(result.first){
            // need to generate a new guide file
            if(DEBUG_TEST) log() << "Runtime : " << duration.count() << " microsseconds"<< std::endl;
            if(DEBUG_TEST) log() << "Runtime : " << std::to_string( (double)duration.count() / 1000000.0 ) << " seconds"<< std::endl;
            int final_wirelength = design.global_routing().wirelength(nets);
            if(DEBUG_TEST) log() << "Total movements = " << movements.size() << std::endl;
            if(DEBUG_TEST) log() << "Circuit final wirelength = " << final_wirelength << std::endl;
            auto score = initial_wirelength - final_wirelength;
            if(DEBUG_TEST) log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
            double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
            if(DEBUG_TEST) log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;
        }
    }

}
