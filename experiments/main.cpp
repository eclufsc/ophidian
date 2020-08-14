#include <iostream>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/routing/AStarRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/log.h>
#include <chrono>

using namespace std;
using namespace ophidian::util;
// int THREADS_DEFAULT_VALUE = 1;

std::chrono::time_point<std::chrono::steady_clock> start_time;

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
        std::vector<ophidian::routing::AStarSegment> initial_segments;
        for(auto segment : global_routing.segments(net))
            initial_segments.push_back(ophidian::routing::AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));

        auto max_wirelength = pair.first;
        global_routing.unroute(net);
        std::vector<ophidian::routing::AStarSegment> segments;
        auto result = astar_routing.route_net(net, segments, max_wirelength, false);
        if(result)
        {
            astar_routing.apply_segments_to_global_routing(segments);
            auto after_wl = global_routing.wirelength(net);
            routed_nets++;
            if(max_wirelength < after_wl)
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
        if(time_out(1200))//20 minutes
        {
            log() << "Time Out reached for optmize routing." << std::endl;
            break;
        }
    }
    std::cout<<"routed "<<routed_nets<<" of "<<netlist.size_net()<<" non routed "<<non_routed<<std::endl;
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


bool move_cell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, ophidian::routing::AStarRouting & astar_routing, int max_wirelength)
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
                routed_all_nets = astar_routing.route_net(net, segments, max_wirelength, false);
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

void test_greedy_solution()
{
    std::vector<std::string> circuit_names = {
        //"case1",
        //"case2",
        //"case3",
        //"case3_no_extra_demand",
        //"case4",
        //"case5",
        "case6",
    };

    // std::string benchmarks_path = "./input_files/iccad2020/cases/";
    std::string benchmarks_path = "./benchmarks/"; //Evaluation Machine
    // std::string benchmarks_path = "./input_files/iccad20/"; //Tiago
    // std::string benchmarks_path = "./benchmarks/"; //Tesla
    for (auto circuit_name : circuit_names)
    {
        start_time = std::chrono::steady_clock::now();
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

        // std::vector<ophidian::circuit::Net> nets(netlist.begin_net(), netlist.end_net());
        // int initial_wirelength = global_routing.wirelength(nets);
        // log() << "Circuit initial wirelength = " << initial_wirelength << std::endl;

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
            if(moved_cells == design.routing_constraints().max_cell_movement() || time_out(3300))
                break;

            at_least_one_cell_moved = false;
            for(auto pair : cells_costs)
            {
                auto cell = pair.first;
                auto max_wirelength = pair.second;
                auto moved = move_cell(design, cell, astar_routing, max_wirelength);
                at_least_one_cell_moved = moved ? moved : at_least_one_cell_moved;
                if(moved)
                {
                    moved_cells++;
                    movements.push_back(std::make_pair(cell, placement.location(cell)));
                    //std::cout<<"# of moved cells: "<<moved_cells<<std::endl;
                }
                if(moved_cells == design.routing_constraints().max_cell_movement())
                    break;
                if(time_out(3300))//55 minutes
                {
                    log() << "Time Out reached for movements." << std::endl;
                    break;
                }
            }
        }
        printlog("Writing solution ...");
        iccad_output_writer.write_ICCAD_2020_output("./sheiny/" + circuit_name + "_out.txt", movements);


        // printlog("Sanity check after movements ...");
        // sanity_check(design);

        log()<<"Number of moved cells: "<<moved_cells<<std::endl;
        // int final_wirelength = global_routing.wirelength(nets);
        // log() << "Circuit final wirelength = " << final_wirelength << std::endl;
        // auto score = initial_wirelength - final_wirelength;
        // log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        // double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        // log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;
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

void run_for_circuit(ophidian::design::Design & design, std::string circuit_name, std::string output) {
    auto & netlist = design.netlist();
    auto& global_routing = design.global_routing();
    ophidian::routing::AStarRouting astar_routing{design};
    //ophidian::routing::ILPRouting ilpRouting(design, circuit_name);
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<ophidian::circuit::Net> fixed_nets;
    std::vector<ophidian::circuit::Net> routed_nets;

    std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements;
    // std::log() << "routing nets" << std::endl;
    //auto result = ilpRouting.route_nets(nets, fixed_nets, routed_nets, movements);
    // std::log() << "result " << result << std::endl;


    for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++)
    {
        global_routing.unroute(*net_it);
        std::vector<ophidian::routing::AStarSegment> segments;
        astar_routing.route_net(*net_it, segments);
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

int main(int argc, char** argv) {
    test_greedy_solution();
    return 0;

    greetings();

    bool input_found{false};
    string input_file{};
    string circuit_name{};

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
    
    run_for_circuit(design, circuit_name, output);

    return 0;
}
