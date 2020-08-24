#include <sys/time.h>

// #define CATCH_CONFIG_MAINs
#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/routing/AStarRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/log.h>
#include <chrono>
#include "run_ilp.h"

std::chrono::time_point<std::chrono::steady_clock> start_time;
using namespace ophidian::util;

//Time in seconds
bool time_out(int time_limit)
{
    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end_time-start_time;
    return diff.count() > time_limit;
}

void improve_routing(ophidian::design::Design & design, ophidian::routing::AStarRouting & astar_routing)
{
    auto & global_routing = design.global_routing();
    auto & netlist = design.netlist();
    auto routed_nets = 0;
    auto non_routed = 0;

    ophidian::routing::AStarRouting::box_type chip_area{design.floorplan().chip_origin(), design.floorplan().chip_upper_right_corner()};

    std::vector<std::pair<int, ophidian::circuit::Net>> sorted_nets;
    for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++)
    {
        int length = global_routing.wirelength(*net_it);
        sorted_nets.push_back(std::make_pair(length, *net_it));
    }
    std::sort(sorted_nets.begin(), sorted_nets.end(), [](auto & pair1, auto & pair2){ return pair1.first < pair2.first; });

    //Reroute smallest nets first to avoid congestion (TODO: not sure about this)
    for(auto pair : sorted_nets)
    {
        auto net = pair.second;
        auto before_wl = global_routing.wirelength(net);
        std::vector<ophidian::routing::AStarSegment> initial_segments;
        for(auto segment : global_routing.segments(net))
            initial_segments.push_back(ophidian::routing::AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));

        global_routing.unroute(net);
        std::vector<ophidian::routing::AStarSegment> segments;
        auto result = astar_routing.route_net(net, segments, chip_area, false);
        if(result)
        {
            astar_routing.apply_segments_to_global_routing(segments);
            auto after_wl = global_routing.wirelength(net);
            routed_nets++;
            if(before_wl < after_wl)
            {
                global_routing.unroute(net);
                astar_routing.apply_segments_to_global_routing(initial_segments);
                non_routed++;
                routed_nets--;
            }
        }else{
            non_routed++;
            astar_routing.apply_segments_to_global_routing(initial_segments);
        }
        if(time_out(300))//5 minutes
        {
            log() << "Time Out reached for optmize routing." << std::endl;
            break;
        }
    }
    std::cout<<"routed "<<routed_nets<<" of "<<netlist.size_net()<<" non routed "<<non_routed<<std::endl;
}

//if not specified the net name, it draws the whole circuit ((not recommended)
void draw_gcell_svg(ophidian::design::Design & design, std::string net_name){
    auto& netlist = design.netlist();
    auto& routing_library = design.routing_library();
    auto& global_routing = design.global_routing();
    auto gcell_graph_ptr = global_routing.gcell_graph();
    std::ofstream out_svg;
    if (net_name == "")
        out_svg.open("output.svg");
    else
        out_svg.open(net_name + ".svg");

    //layer index
    std::unordered_map<int, std::string> layer2color = {
        {1,"#0000ff"},//blue
        {2,"#ff0000"},//red
        {3,"#00d000"},//green
        {4,"#d0d000"},//yellow
        {5,"#a52a2a"},//dark red
        {6,"#ffa500"},//orange
        {7,"#d000d0"},//pink
        {8,"#00d0d0"},//light blue
        {9,"#a52a2a"},//brown
        {10,"#ffff00"},//light yellow
        {11,"#008000"},//dark green
        {12,"#ff00ff"},//purple
        {13,"#ffc0cb"},//light pink
        {14,"#00ffff"},//light blue
        {15,"#800080"},//dark purple
        {16,"#808000"},//dark yellow
    };

    out_svg<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    out_svg<<"<svg>\n";

    for(auto layer_color : layer2color)
    {
        out_svg<<"<g\n";
        out_svg<<"inkscape:label=\""<<layer_color.first<<"\"\n";
        out_svg<<"inkscape:groupmode=\"layer\"\n";
        out_svg<<"id=\""<<layer_color.first<<"\">\n";
        for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); ++net_it)
        {
            if(net_name != "" && netlist.name(*net_it) != net_name)
                continue;

            for(auto gcell : global_routing.gcells(*net_it))
            {

                if(gcell_graph_ptr->layer_index(gcell) != layer_color.first)
                    continue;

                auto box = gcell_graph_ptr->box(gcell);
                auto width = box.max_corner().x() - box.min_corner().x();
                auto height = box.max_corner().y() - box.min_corner().y();

                out_svg<<"<rect\n";
                out_svg<<"style=\"fill:"<<layer_color.second<<";fill-opacity:0.5;\"\n";
                out_svg<<"width=\""<<units::unit_cast<double>(width)<<"\"\n";
                out_svg<<"height=\""<<units::unit_cast<double>(height)<<"\"\n";
                out_svg<<"x=\""<<units::unit_cast<double>(box.min_corner().x())<<"\"\n";
                out_svg<<"y=\""<<units::unit_cast<double>(-box.max_corner().y())<<"\" />\n";//svg files use y axis flipped
            }
        }
        out_svg<<"</g>\n";
    }
    out_svg<<"</svg>";
    out_svg.close();
}

//the wirelength and wirelength demand (demand - blockage_demand) should be always equal
bool sanity_check(ophidian::design::Design & design)
{
    auto & netlist = design.netlist();
    auto & global_routing = design.global_routing();
    auto gcell_graph_ptr = global_routing.gcell_graph();

    std::vector<ophidian::circuit::Net> nets(netlist.begin_net(), netlist.end_net());
    auto wirelength = design.global_routing().wirelength(nets);

    std::cout<<"Wirelength "<<wirelength<<std::endl;

    int wirelength_demand = 0;
    for(auto gcell_it = gcell_graph_ptr->begin_gcell(); gcell_it != gcell_graph_ptr->end_gcell(); gcell_it++)
    {
        auto gcell = *gcell_it;
        wirelength_demand += (gcell_graph_ptr->demand(gcell) - gcell_graph_ptr->blockage_demand(gcell) - gcell_graph_ptr->extra_demand(gcell));
    }

    std::cout<<"Wirelength demmand "<<wirelength_demand<<std::endl;
    return wirelength == wirelength_demand;
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

void write_statistics_for_circuit(ophidian::design::Design & design, std::string circuit_name, std::unordered_map<ophidian::circuit::Net, double, ophidian::entity_system::EntityBaseHash> & nets_initial_wirelength) {
    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());

    std::ofstream stats_file("stats/" + circuit_name + "_nets.csv");
    stats_file << "net,pins,stwl,routed_length,routed_length_no_vias,initial_wirelength,box_width,box_height" << std::endl;
    for (auto net : nets) {
        auto net_name = design.netlist().name(net);
        auto pins = design.netlist().pins(net);

        std::vector<ophidian::interconnection::Flute::Point> net_points;
        net_points.reserve(pins.size());
        auto min_x = std::numeric_limits<double>::max();
        auto max_x = -std::numeric_limits<double>::max();
        auto min_y = std::numeric_limits<double>::max();
        auto max_y = -std::numeric_limits<double>::max();
        for (auto pin : pins) {
            auto pin_location = design.placement().location(pin);
            net_points.push_back(pin_location);

            min_x = std::min(min_x, pin_location.x().value());
            max_x = std::max(max_x, pin_location.x().value());
            min_y = std::min(min_y, pin_location.y().value());
            max_y = std::max(max_y, pin_location.y().value());
        }

        auto & flute = ophidian::interconnection::Flute::instance();
        auto tree = flute.create(net_points);
        auto stwl = tree->length().value();
        stwl /= 10;
        if (stwl == 0) {
            stwl = 1;
        } 

        auto routes = design.global_routing().segments(net);
        auto routed_length_no_vias = 0;
        auto via_length = 0;
        for (auto route : routes) {
            auto box = design.global_routing().box(route);
            auto start = box.min_corner();
            auto end = box.max_corner();
            routed_length_no_vias += (std::abs(start.x().value() - end.x().value()) + std::abs(start.y().value() - end.y().value()));

            auto start_layer = design.global_routing().layer_start(route);
            auto end_layer = design.global_routing().layer_end(route);
            auto start_layer_index = design.routing_library().layerIndex(start_layer);
            auto end_layer_index = design.routing_library().layerIndex(end_layer);
            via_length += std::abs(start_layer_index - end_layer_index);
        }
        routed_length_no_vias /= 10;
        if (routed_length_no_vias == 0) {
            routed_length_no_vias = 1;
        }
        auto routed_length = routed_length_no_vias + via_length;

        auto initial_wirelength = nets_initial_wirelength[net];

        auto box_width = max_x - min_x;
        auto box_height = max_y - min_y;

        stats_file << net_name << "," << pins.size() << "," << stwl << "," << routed_length << "," << routed_length_no_vias << "," << initial_wirelength << "," << box_width << "," << box_height << std::endl;
    }
    stats_file.close();
}

TEST_CASE("run ILP for iccad20 benchmarks", "[iccad20]") {
    std::vector<std::string> circuit_names = {
        // "case1",
        //"case1N4",
        // "case2",
        // "case3",
        "case5",
        //"case3_no_blockages",
        // "case3_no_extra_demand"
        //"case3_only_same_grid"
        //"case3_only_adj_rule"
        //"case3_small_rules"
        //"same_grid_test"
    };

    // std::string benchmarks_path = "./input_files/iccad2020/cases/";
    std::string benchmarks_path = "./input_files/iccad20/"; //Tiago
    // std::string benchmarks_path = "./benchmarks/"; //Tesla
    for (auto circuit_name : circuit_names) {
        log() << "running circuit " << circuit_name << std::endl;

        std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";

        log() << "file " << iccad_2020_file << std::endl;

        auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
        run_ilp_for_circuit(design, circuit_name);
        // auto is_connected = design.global_routing().is_connected() ? "grafo conexo" : "grafo desconexo!";
        
        // log() << "done, " << is_connected << circuit_name << std::endl;
    }


}

TEST_CASE("run ILP for iccad20 benchmarks", "[iccad20]") {
    std::vector<std::string> circuit_names = {
        // "case1",
        //"case1N4",
        // "case2",
        "case3",
        //"case4",
        // "case5",
         //"case5_no_extra_demand",
        //"case3_no_blockages",
        // "case3_no_extra_demand"
        //"case3_only_same_grid"
        //"case3_only_adj_rule"
        //"case3_small_rules"
        //"same_grid_test"
    };

     std::string benchmarks_path = "./input_files/iccad2020/cases/";
    // std::string benchmarks_path = "./input_files/iccad20/cases/"; //Tiago
    // std::string benchmarks_path = "./benchmarks/"; //Tesla
    for (auto circuit_name : circuit_names) {
        log() << "running circuit " << circuit_name << std::endl;

        std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";

        log() << "file " << iccad_2020_file << std::endl;

        auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
        run_ilp_for_circuit(design, circuit_name);
        // auto is_connected = design.global_routing().is_connected() ? "grafo conexo" : "grafo desconexo!";
        
        // log() << "done, " << is_connected << circuit_name << std::endl;
    }


}

TEST_CASE("run LP for iccad20 benchmarks", "[iccad20_LP_ILP]") {
    std::vector<std::string> circuit_names = {
        // "case1",
        // "case2",
         "case3",
        // "case4",
        //"case5",
    };

     std::string benchmarks_path = "./input_files/iccad2020/cases/";
    //std::string benchmarks_path = "./input_files/iccad20/"; //Tiago
    // std::string benchmarks_path = "./benchmarks/"; //Tesla
    for (auto circuit_name : circuit_names) {
        log() << "running circuit " << circuit_name << std::endl;

        std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";

        log() << "file " << iccad_2020_file << std::endl;

        auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
        
        // run_ilp_for_circuit(design, circuit_name);
        run_circuit(design, circuit_name);
    }


}

TEST_CASE("run ILP with panelling for iccad20 benchmarks", "[iccad20_LP_ILP]") {
    std::vector<std::string> circuit_names = {
        // "case1",
        // "case2",
         "case3",
         //"case4",
        //"case5",
    };

     std::string benchmarks_path = "./input_files/iccad2020/cases/";
    //std::string benchmarks_path = "./input_files/iccad20/"; //Tiago
    // std::string benchmarks_path = "./benchmarks/"; //Tesla
    for (auto circuit_name : circuit_names) {
        log() << "running circuit " << circuit_name << std::endl;

        std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";

        log() << "file " << iccad_2020_file << std::endl;

        auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design_iccad2020(design, iccad_2020);

        std::unordered_map<ophidian::circuit::Net, double, ophidian::entity_system::EntityBaseHash> nets_initial_wirelength;
        std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
        for (auto net : nets) {
            auto net_wirelength = design.global_routing().wirelength(net);
            nets_initial_wirelength[net] = net_wirelength; 
        }
        
         run_ilp_for_circuit(design, circuit_name);
        //run_circuit(design, circuit_name);
        //run_mcf_multithreading(design);
        //write_statistics_for_circuit(design, circuit_name, nets_initial_wirelength);
    }


}

TEST_CASE("iccad20 AStarRouting", "[astar]")
{
    std::string circuit_name = "case3";
    std::string benchmarks_path = "./input_files/iccad2020/cases/";
    std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";
    std::cout<<iccad_2020_file<<std::endl;
    auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};
    auto design = ophidian::design::Design();
    ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);
    ophidian::routing::AStarRouting::box_type chip_area{design.floorplan().chip_origin(), design.floorplan().chip_upper_right_corner()};
    auto astar_routing = ophidian::routing::AStarRouting(design);
    auto net = design.netlist().find_net("N2594");
    std::vector<ophidian::routing::AStarSegment> segments;
    auto & global_routing = design.global_routing();
    global_routing.unroute(net);
    astar_routing.route_net(net, segments, chip_area);
    iccad_output_writer.write_ICCAD_2020_output("case3.txt", {});
}

TEST_CASE("iccad20 AStarRouting on all nets", "[astar_all_nets]")
{
    start_time = std::chrono::steady_clock::now();
    std::string circuit_name = "case3";
    std::string benchmarks_path = "./input_files/iccad2020/cases/";
    std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";
    std::cout<<iccad_2020_file<<std::endl;
    auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};
    auto design = ophidian::design::Design();
    auto& global_routing = design.global_routing();
    ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    auto& netlist = design.netlist();
    int routed_nets = 0;
    int non_routed = 0;
    auto astar_routing = ophidian::routing::AStarRouting(design);

    sanity_check(design);
    improve_routing(design, astar_routing);
    sanity_check(design);

    iccad_output_writer.write_ICCAD_2020_output(circuit_name + "_out.txt", {});
}

ophidian::routing::GlobalRouting::gcell_type calculate_median_gcell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell)
{
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;

    auto & netlist = design.netlist();
    auto & placement = design.placement();
    std::vector<net_type> cell_nets;
    
    std::vector<double> x_positions;
    std::vector<double> y_positions;

    for(auto pin : netlist.pins(cell)){
        auto net = netlist.net(pin);
        if(net == ophidian::circuit::Net())
            continue;
        cell_nets.push_back(net);
        for(auto net_pin : netlist.pins(net)){
            if(net_pin == pin)
                continue;
            auto location = placement.location(net_pin);
            x_positions.push_back(location.x().value());
            y_positions.push_back(location.y().value());
        }
    }
    auto cell_location = placement.location(cell);
    auto current_gcell = design.global_routing().gcell_graph()->nearest_gcell(cell_location, 0);

    if(x_positions.empty() || y_positions.empty())
        return current_gcell;

    std::nth_element(x_positions.begin(), x_positions.begin() + x_positions.size()/2, x_positions.end());
    auto median_x = x_positions[x_positions.size()/2];
    std::nth_element(y_positions.begin(), y_positions.begin() + y_positions.size()/2, y_positions.end());
    auto median_y = y_positions[y_positions.size()/2];

    point_type median_point {unit_type(median_x), unit_type(median_y)};
    
    auto nearest_gcell = design.global_routing().gcell_graph()->nearest_gcell(median_point, 0);

    if(current_gcell != nearest_gcell){
        return nearest_gcell;
    }
    return current_gcell;
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
    auto median_gcell = calculate_median_gcell(design, cell);

    ophidian::routing::AStarRouting::box_type chip_area{design.floorplan().chip_origin(), design.floorplan().chip_upper_right_corner()};

    if(initial_gcell != median_gcell)
    {
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
        {
            for(auto segment : global_routing.segments(net))
                initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));
            global_routing.unroute(net);
        }

        //move cell to median
        auto overflow_movement = global_routing.move_cell(initial_gcell, median_gcell, cell, netlist, placement, routing_constr, std_cells);
        if(overflow_movement == false)
        {
            std::vector<AStarSegment> segments;
            bool routed_all_nets = true;
            for(auto net : cell_nets)
            {
                routed_all_nets = astar_routing.route_net(net, segments, chip_area, false);
                if(routed_all_nets == false)
                    break;
            }
            if(routed_all_nets)
            {
                bool working_correct = astar_routing.apply_segments_to_global_routing(segments);
                auto wirelength_after = global_routing.wirelength(cell_nets);
                if(wirelength_before > wirelength_after && working_correct)
                    return true;
            }
        }
        for(auto net : cell_nets)
            global_routing.unroute(net);

        bool undo_overflow = global_routing.move_cell(median_gcell, initial_gcell, cell, netlist, placement, routing_constr, std_cells);
        if(undo_overflow == true)
            std::cout<<"WARNING: UNDO MOVEMENT OVERFLOW!"<<std::endl;
        bool undo = astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
        if(undo == false)
            std::cout<<"WARNING: UNDO ROUTING OVERFLOW!"<<std::endl;
    }
    return false;
}

TEST_CASE("iccad20 AStarRouting on all nets and moving cells", "[astar_moving_cells]")
{
    std::vector<std::string> circuit_names = {
        // "case1",
        // "case2",
        "case4",
        // "case3_no_extra_demand",
        // "case4",
        // "case5",
        // "case6",
    };

    std::string benchmarks_path = "./input_files/iccad2020/cases/";
    // std::string benchmarks_path = "./input_files/iccad20/"; //Tiago
    // std::string benchmarks_path = "./benchmarks/"; //Tesla
    for (auto circuit_name : circuit_names) 
    {
        log() << "Circuit name : " << circuit_name << std::endl;
        start_time = std::chrono::steady_clock::now();
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

        auto astar_routing = ophidian::routing::AStarRouting(design);

        printlog("Improving initial routing with A* ...");
        improve_routing(design, astar_routing);
        //sanity_check(design);

        printlog("Compute cells move cost ...");
        std::vector<std::pair<ophidian::circuit::CellInstance, double>> cells_costs;

        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); ++cell_it)
        {
            auto cell = *cell_it;
            if(placement.isFixed(cell))
                continue;
            if(cell_has_more_than_1_pin_in_same_net(design, cell))
                continue;
            
            std::unordered_set<net_type, ophidian::entity_system::EntityBaseHash> cell_nets;
            for(auto pin : netlist.pins(cell))
                cell_nets.insert(netlist.net(pin));

            double routed_length = 0;
            for(auto net : cell_nets)
                routed_length += design.global_routing().wirelength(net);
            cells_costs.push_back(std::make_pair(cell, routed_length));
        }
        std::sort(cells_costs.begin(), cells_costs.end(), [](std::pair<ophidian::circuit::CellInstance, double> cost_a, std::pair<ophidian::circuit::CellInstance, double> cost_b) {return cost_a.second > cost_b.second;});

        //printlog("Sanity check before movements ...");
        //sanity_check(design);
        printlog("Do not forget to map cell instances into GCells global_trouting.set_gcell_cell_instances(netlist, placement)!!!");
        global_routing.set_gcell_cell_instances(netlist, placement);

        printlog("Initing movements ...");
        auto moved_cells = 0;
        std::vector<std::pair<cell_type, point_type>>  movements;

        bool at_least_one_cell_moved = true;
        while(at_least_one_cell_moved)
        {
            if(moved_cells == design.routing_constraints().max_cell_movement() || time_out(600))
                break;

            at_least_one_cell_moved = false;
            for(auto pair : cells_costs)
            {
                auto cell = pair.first;
                auto moved = move_cell(design, cell, astar_routing);
                at_least_one_cell_moved = moved ? moved : at_least_one_cell_moved;
                if(moved)
                {
                    moved_cells++;
                    movements.push_back(std::make_pair(cell, placement.location(cell)));
                    //std::cout<<"# of moved cells: "<<moved_cells<<std::endl;
                }
                if(moved_cells == design.routing_constraints().max_cell_movement())
                    break;
                if(time_out(600))//10 minutes
                {
                    log() << "Time Out reached for movements." << std::endl;
                    break;
                }
            }
        }
        printlog("Writing solution ...");
        iccad_output_writer.write_ICCAD_2020_output(circuit_name + "_out.txt", movements);


        // printlog("Sanity check after movements ...");
        // sanity_check(design);

        log()<<"Number of moved cells: "<<moved_cells<<std::endl;
        int final_wirelength = global_routing.wirelength(nets);
        log() << "Circuit final wirelength = " << final_wirelength << std::endl;
        auto score = initial_wirelength - final_wirelength;
        log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;
    }
}
