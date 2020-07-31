#include <sys/time.h>

// #define CATCH_CONFIG_MAINs
#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/routing/AStarRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/log.h>
#include "run_ilp.h"

using namespace ophidian::util;

void save_initial_state(
    ophidian::design::Design & design, 
    ophidian::circuit::Net & net,
    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & initial_cells_location,
    std::vector<ophidian::routing::AStarSegment> & initial_segments,
    std::unordered_set<ophidian::circuit::CellInstance, ophidian::entity_system::EntityBaseHash> & cells_to_colapse,
    std::unordered_set<ophidian::circuit::Net, ophidian::entity_system::EntityBaseHash> & neighbor_nets,
    std::unordered_set<ophidian::circuit::CellInstance, ophidian::entity_system::EntityBaseHash> & neighbor_cells)
{
    using namespace ophidian::routing;
    auto & netlist = design.netlist();
    auto & placement = design.placement();
    auto & global_routing = design.global_routing();
    for(auto pin : netlist.pins(net))
    {
        if(netlist.is_pad(pin))
            continue;
        auto cell = netlist.cell(pin);
        cells_to_colapse.insert(cell);

        //discovery neighbors
        for(auto pin_neighbor : netlist.pins(cell))
        {
            if(pin_neighbor == pin || netlist.is_pad(pin_neighbor))
                continue;
            auto net_neighbor = netlist.net(pin_neighbor);
            if(net_neighbor == net || net_neighbor == ophidian::circuit::Net())
                continue;
            neighbor_nets.insert(net_neighbor);

            //fixed cells (neighbors)
            for(auto pin_fix : netlist.pins(net_neighbor))
            {
                if( pin_fix == pin_neighbor || pin_fix == pin || netlist.is_pad(pin_fix))
                    continue;
                auto cell_fix = netlist.cell(pin_fix);
                neighbor_cells.insert(cell_fix);
            }
        }
    }
    for(auto cell : cells_to_colapse)
    {
        auto position = placement.location(cell);
        initial_cells_location.push_back(std::make_pair(cell, position));
    }
    for(auto segment : global_routing.segments(net))
    {
        initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));
    }
    for(auto net_i : neighbor_nets)
    {
        for(auto segment : global_routing.segments(net_i))
        {
            initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net_i));
        }
    }
}

void restore_initial_status(
    ophidian::design::Design & design, 
    ophidian::routing::AStarRouting & astar_routing,
    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & initial_cells_location,
    std::vector<ophidian::routing::AStarSegment> & initial_segments)
{
    auto & placement = design.placement();
    for(auto pair : initial_cells_location)
    {
        placement.place(pair.first, pair.second);
    }
    //TODO: update extra-demand

    astar_routing.apply_segments_to_global_routing(initial_segments);
}

bool move_net(ophidian::design::Design & design, ophidian::circuit::Net & net, std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements)
{
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;
    using cell_type = ophidian::circuit::CellInstance;
    using gcell_type = ophidian::routing::GCell;
    using AStarSegment = ophidian::routing::AStarSegment;
    using EntityBaseHash = ophidian::entity_system::EntityBaseHash;

    auto & global_routing = design.global_routing();
    auto & netlist = design.netlist();
    auto & placement = design.placement();
    auto astar_routing = ophidian::routing::AStarRouting(design);

    std::vector<std::pair<cell_type, point_type>> initial_cells_location;
    std::vector<AStarSegment> initial_segments;

    std::unordered_set<cell_type, EntityBaseHash> cells_to_colapse;
    std::unordered_set<net_type, EntityBaseHash> neighbor_nets;
    std::unordered_set<cell_type, EntityBaseHash> neighbor_cells;
    
    save_initial_state(design, net, initial_cells_location, initial_segments, cells_to_colapse, neighbor_nets, neighbor_cells);

    if(neighbor_cells.empty())
    {
        //movement is not possible!!
        return false;
    }

    //calculate median of neighbor_cells
    std::vector<double> x_positions;
    std::vector<double> y_positions;
    for(auto cell : neighbor_cells)
    {
        auto location = placement.location(cell);
        x_positions.push_back(location.x().value());
        y_positions.push_back(location.y().value());
    }
    std::nth_element(x_positions.begin(), x_positions.begin() + x_positions.size()/2, x_positions.end());
    auto median_x = x_positions[x_positions.size()/2];
    std::nth_element(y_positions.begin(), y_positions.begin() + y_positions.size()/2, y_positions.end());
    auto median_y = y_positions[y_positions.size()/2];
    point_type median_point {unit_type(median_x), unit_type(median_y)};
    auto nearest_gcell = global_routing.gcell_graph()->nearest_gcell(median_point, 0);
    auto gcell_location = global_routing.gcell_graph()->center_of_box(nearest_gcell);

    // unroute nets
    global_routing.unroute(net);
    for(auto neighbor_net : neighbor_nets)
    {
        global_routing.unroute(neighbor_net);
    }

    // geting gcells arround median gcell
    auto gcells_nearest = global_routing.gcell_graph()->nearest_gcell(gcell_location, 0, 9);
    //evaluating the congestions in these gcells
    std::vector<std::pair<gcell_type, double>> gcells_ocupation;
    gcells_ocupation.reserve(gcells_nearest.size());
    for(auto gcell : gcells_nearest)
    {
        auto capacity = global_routing.gcell_graph()->capacity(gcell);
        auto demand = global_routing.gcell_graph()->demand(gcell);
        double ocupation = demand / capacity;
        gcells_ocupation.push_back(std::make_pair(gcell, ocupation));
    }
    std::sort(gcells_ocupation.begin(), gcells_ocupation.end(), [](std::pair<gcell_type, double> cost_a, std::pair<gcell_type, double> cost_b) {return cost_a.second < cost_b.second;});




    // place all cells from net in the same gcell
    for(auto cell : cells_to_colapse)
    {
        //get less congestioned gcell
        auto pair = *gcells_ocupation.begin();
        auto gcell = pair.first;
        auto ocupation = pair.second;
        gcells_ocupation.erase(gcells_ocupation.begin());

        //TODO: if ocupation is more tha X get more gcells


        auto location = global_routing.gcell_graph()->center_of_box(gcell);
        
        //update demand and extra demand in global routing!
        global_routing.update_blockage_demand(netlist, placement, cell, true);
        placement.place(cell, location);
        global_routing.update_blockage_demand(netlist, placement, cell, false);
        // TODO: update extra demand


        // WARNING : this must be improved!
        auto capacity = global_routing.gcell_graph()->capacity(gcell);
        auto demand = global_routing.gcell_graph()->demand(gcell);
        ocupation = demand / capacity;
        gcells_ocupation.push_back(std::make_pair(gcell, ocupation));
        std::sort(gcells_ocupation.begin(), gcells_ocupation.end(), [](std::pair<gcell_type, double> cost_a, std::pair<gcell_type, double> cost_b) {return cost_a.second < cost_b.second;});
    }

    //TODO: evaluate congestion in upper layers (like layer 2 and 3)




    //try to route nets
    std::vector<AStarSegment> segments;
    bool routed_all_nets = true;
    auto result = astar_routing.route_net(net, segments, false);
    if(!result)
    {
        routed_all_nets = false;
    }
    if(routed_all_nets){
        for(auto neighbor_net : neighbor_nets)
        {
            auto result = astar_routing.route_net(neighbor_net, segments, false);
            if(!result)
            {
                routed_all_nets = false;
                break;
            }
        }
    }

    if(routed_all_nets){
        // apply changes in global routing, generate movements
        astar_routing.apply_segments_to_global_routing(segments);
        for(auto cell : cells_to_colapse)
        {  
            movements.push_back(std::make_pair(cell, gcell_location));
        }
    }else{
        //restore initial status and return false
        restore_initial_status(design, astar_routing, initial_cells_location, initial_segments);
        return false;
    }
    return true;
}


TEST_CASE("iccad20 AStarRouting on all nets and colapsing nets", "[astar_colapse_nets]")
{
    std::vector<std::string> circuit_names = {
        // "case1",
        // "case2",
        // "case3",
        "case3_no_extra_demand",
        // "case4",
        // "case5",
    };

    std::string benchmarks_path = "./input_files/iccad2020/cases/";
    // std::string benchmarks_path = "./input_files/iccad20/"; //Tiago
    // std::string benchmarks_path = "./benchmarks/"; //Tesla
    for (auto circuit_name : circuit_names) 
    {
        using scalar_type = int;
        using AStarSegment = ophidian::routing::AStarSegment;
        using net_type = ophidian::circuit::Net;
        using cell_type = ophidian::circuit::CellInstance;
        using point_type = ophidian::util::LocationDbu;
        std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";
        auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};
        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
        ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);
        auto & global_routing = design.global_routing();
        auto & netlist = design.netlist();
        auto & placement = design.placement();

        std::vector<ophidian::circuit::Net> nets(netlist.begin_net(), netlist.end_net());
        int initial_wirelength = global_routing.wirelength(nets);
        log() << "Circuit initial wirelength = " << initial_wirelength << std::endl;

        int routed_nets = 0;
        int non_routed = 0;
        auto astar_routing = ophidian::routing::AStarRouting(design);

        printlog("Initial A* routing ...");
        for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++)
        {
            auto net = *net_it;
            std::vector<AStarSegment> initial_segments;
            for(auto segment : global_routing.segments(net))
                initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));

            global_routing.unroute(net);
            std::vector<AStarSegment> segments;
            auto result = astar_routing.route_net(net, segments, false);
            if(result)
            {
                routed_nets++;
                astar_routing.apply_segments_to_global_routing(segments);
            }else{
                non_routed++;
                astar_routing.apply_segments_to_global_routing(initial_segments);
            }
        }

        int a_wirelength = global_routing.wirelength(nets);
        log() << "Circuit A* wirelength = " << a_wirelength << std::endl;
        auto score = initial_wirelength - a_wirelength;
        log() << "Estimated score ( "<< initial_wirelength << " - " << a_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) a_wirelength / (double) initial_wirelength) ;
        log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;


        printlog("Compute nets possible gain ...");
        std::vector<std::pair<net_type, scalar_type>> net_wirelength;
        net_wirelength.reserve(netlist.size_net());
        for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++)
        {
            auto net = *net_it;
            auto wirelength = global_routing.wirelength(net);
            net_wirelength.push_back(std::make_pair(net, wirelength));
        }
        std::sort(net_wirelength.begin(), net_wirelength.end(), [](std::pair<net_type, scalar_type> cost_a, std::pair<net_type, scalar_type> cost_b) {return cost_a.second > cost_b.second;});

        std::vector<std::pair<cell_type, point_type>>  movements;

        int number_of_nets = std::floor(netlist.size_net() * 0.05);
        printflog("5 percent of nets is %d nets \n", number_of_nets);
        for(int i = 0; i < number_of_nets; i++)
        {
            auto net = net_wirelength.at(i).first;
            auto wirelength = net_wirelength.at(i).second;
            printflog("Colapsing net : %s with %d wirelength\n", netlist.name(net), wirelength);
            move_net(design, net, movements);
        }


        int colapse_wirelength = global_routing.wirelength(nets);
        log() << "Circuit final wirelength = " << colapse_wirelength << std::endl;
        int partial_score = initial_wirelength - colapse_wirelength;
        score = initial_wirelength - colapse_wirelength;
        log() << "Partial score ( "<< a_wirelength << " - " << colapse_wirelength << " ) = " << partial_score << std::endl;
        log() << "Estimated score ( "<< initial_wirelength << " - " << colapse_wirelength << " ) = " << score << std::endl;
        

        printlog("Writing solution ...");
        iccad_output_writer.write_ICCAD_2020_output(circuit_name + "_out.txt", movements);

        log()<<"routed "<<routed_nets<<" of "<<netlist.size_net()<<" non routed "<<non_routed<<std::endl;
        int final_wirelength = global_routing.wirelength(nets);
        log() << "Circuit final wirelength = " << final_wirelength << std::endl;
        score = initial_wirelength - final_wirelength;
        log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;
    }
}
