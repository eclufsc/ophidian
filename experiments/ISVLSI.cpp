#include <sys/time.h>

// #define CATCH_CONFIG_MAINs
#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/routing/AStarRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/parser/GuideWriter.h>
#include <ophidian/util/log.h>
#include <chrono>
#include "run_ilp.h"
#include "date21.h"
// std::chrono::time_point<std::chrono::steady_clock> start_time;
using namespace ophidian::util;

using movement_container_type = std::unordered_map< ophidian::circuit::CellInstance, ophidian::util::LocationDbu, ophidian::entity_system::EntityBaseHash>; 

namespace ISVLSI{

struct mevement { 
    ophidian::circuit::CellInstance cell;
    ophidian::util::LocationDbu position;
    int improvement = 0;

    mevement(ophidian::circuit::CellInstance & c, ophidian::util::LocationDbu & p) : cell(c), position(p)
    {}

    mevement(ophidian::circuit::CellInstance & c, ophidian::util::LocationDbu & p, int i) : cell(c), position(p), improvement(i)
    {}

    bool operator==(const mevement &mv) const {
        return cell == mv.cell;
    }

    bool operator<(const mevement &mv) const {
        return improvement < mv.improvement;
    }
};

ophidian::util::LocationDbu pin_gcell_location(ophidian::design::Design & design, ophidian::circuit::PinInstance & pin)
{
    auto pin_loc = design.placement().location(pin);
    auto layer_indexes = design.routing_library().index(design.placement().geometry(pin).layers_names());
    //TODO: consider all gcells of the pin
    auto gcell = design.global_routing().gcell_graph()->nearest_gcell(pin_loc, *layer_indexes.begin());
    auto loc = design.global_routing().gcell_graph()->center_of_box(gcell);
    return loc;
}

ophidian::util::LocationDbu cell_gcell_location(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell)
{
    auto cell_loc = design.placement().location(cell);
    auto layer_indexes = design.routing_library().index(design.placement().geometry(cell).layers_names());
    //TODO: consider all gcells of the pin
    auto gcell = design.global_routing().gcell_graph()->nearest_gcell(cell_loc, *layer_indexes.begin());
    auto loc = design.global_routing().gcell_graph()->center_of_box(gcell);
    return loc;
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
            auto location = pin_gcell_location(design, net_pin);
            x_positions.push_back(location.x().value());
            y_positions.push_back(location.y().value());
        }
    }
    auto cell_location = cell_gcell_location(design, cell);
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

void associeted_wirelength(
    ophidian::design::Design & design, 
    std::unordered_map< ophidian::circuit::Net, int, ophidian::entity_system::EntityBaseHash> & nets_STWL,
    std::unordered_map< ophidian::circuit::CellInstance, int, ophidian::entity_system::EntityBaseHash> & cells_STWL )
{
    auto & netlist = design.netlist();
    //calculate the wirelength (FLUTE) of each net
    for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); ++net_it){
        auto net = *net_it;
        if(netlist.name(net) == "pin1"){
            printlog("removing net pin1");
            continue;
        }
        std::vector<ophidian::interconnection::Flute::Point> net_points;
        auto net_pins = netlist.pins(net);
        net_points.reserve(net_pins.size());
        for(auto pin : net_pins)
        {
            auto pin_location = pin_gcell_location(design, pin);
            net_points.push_back(pin_location);
        }
        auto & flute = ophidian::interconnection::Flute::instance();
        auto tree = flute.create(net_points);
        auto number_of_segments = tree->size(ophidian::interconnection::SteinerTree::Segment());
        auto steiner_tree_length = tree->length().value();
        nets_STWL.insert(std::make_pair(net, steiner_tree_length));
    }

    for (auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); ++cell_it) 
    {
        auto cell = *cell_it;
        int wirelength = 0;
        const auto & pins = design.netlist().pins(cell);
        for(auto pin : pins){
            auto net = design.netlist().net(pin);
            wirelength += nets_STWL[net];
        }
        cells_STWL.insert(std::make_pair(cell, wirelength));
    }
}


int evaluate(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, ophidian::util::LocationDbu & loc)
{
    auto initial_location = design.placement().location(cell);
    design.placement().place(cell, loc);

    int wirelength = 0;
    const auto & pins = design.netlist().pins(cell);
    for(auto pin : pins){
        auto net = design.netlist().net(pin);
        
        if(design.netlist().name(net) == "pin1"){
            printlog("removing net pin1");
            continue;
        }
        std::vector<ophidian::interconnection::Flute::Point> net_points;
        auto net_pins = design.netlist().pins(net);
        net_points.reserve(net_pins.size());
        for(auto pin : net_pins)
        {
            auto pin_location = pin_gcell_location(design, pin);
            net_points.push_back(pin_location);
        }
        auto & flute = ophidian::interconnection::Flute::instance();
        auto tree = flute.create(net_points);
        auto steiner_tree_length = tree->length().value();

        wirelength += steiner_tree_length;
    }

    design.placement().place(cell, initial_location);
    return wirelength;
}

void cells_to_median(ophidian::design::Design & design, std::set<mevement> & possible_movements)
{
    auto & netlist = design.netlist();

    //calculate the wirelength (FLUTE) of each net
    std::unordered_map< ophidian::circuit::Net, int, ophidian::entity_system::EntityBaseHash> nets_STWL;
    std::unordered_map< ophidian::circuit::CellInstance, int, ophidian::entity_system::EntityBaseHash> cells_STWL;
    associeted_wirelength(design, nets_STWL, cells_STWL);
    
    for (auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); ++cell_it) 
    {
        auto cell = *cell_it;
        auto median_gcell = calculate_median_gcell(design, cell);
        auto new_position = design.global_routing().gcell_graph()->center_of_box(median_gcell);
        auto new_wrl = evaluate(design, cell, new_position);
        auto improvement = cells_STWL[cell] - new_wrl;
        possible_movements.emplace(cell, new_position, improvement);
    }
}
 
bool fileExists(std::string fileName)
{
    ifstream infile(fileName);
    return infile.good();
}


void write_csv(ophidian::design::Design & design, std::string csv_file_name, std::set<mevement> & possible_movements) {
    if(!fileExists(csv_file_name)){
        std::ofstream csv_file(csv_file_name, std::ofstream::out);
        csv_file << "cell,position_dbu_x,position_dbu_y,improvement" << std::endl;
    }

    std::ofstream csv_file(csv_file_name, std::ofstream::app);

    for(auto mv : possible_movements)
    {
        auto cell_name = design.netlist().name(mv.cell);
        csv_file << cell_name << "," <<  mv.position.x().value() << "," << mv.position.y().value() << "," << mv.improvement << std::endl;
    }

}


void write_csv(ophidian::design::Design & design, std::string csv_file_name, movement_container_type & movements) {
    if(!fileExists(csv_file_name)){
        std::ofstream csv_file(csv_file_name, std::ofstream::out);
        csv_file << "cell,position_dbu_x,position_dbu_y" << std::endl;
    }

    std::ofstream csv_file(csv_file_name, std::ofstream::app);

    for(auto mv : movements)
    {
        auto cell_name = design.netlist().name(mv.first);
        csv_file << cell_name << "," <<  mv.second.x().value() << "," << mv.second.y().value() << std::endl;
    }
}

TEST_CASE("ILSVLSI paper all cells to median code", "[ILSVLSI]")
{
    //iccad 2019 benchmarks
    std::vector<std::string> circuit_names = {

        "ispd18_test1",
        // "ispd18_test2",
        // "ispd18_test3",
        // "ispd18_test4",
        // "ispd18_test5",
        // "ispd18_test6",
        // "ispd18_test7",
        // "ispd18_test8",
        // "ispd18_test9",
        // "ispd18_test10",

        // "ispd19_test1",
        // "ispd19_test2",
        // "ispd19_test3",
        // "ispd19_test4",
        // "ispd19_test5",
        // "ispd19_test6",
        // "ispd19_test7",
        // "ispd19_test8",
        // "ispd19_test9",
        // "ispd19_test10",
    };

    std::string benchmarks_path = "./input_files/circuits";

    for (auto circuit_name : circuit_names) {
        std::cout << "running circuit " << circuit_name << std::endl;

        std::string def_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.def";
        std::string lef_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.lef";
        std::string guide_file = benchmarks_path + "/cu_gr_solution_tiago/guides/" + circuit_name + ".solution_cugr.guide";
        // std::string guide_file = benchmarks_path + "/cu_gr_solution_nopatch_connected/" + circuit_name + ".solution_cugr_ophidian.guide";
        ophidian::parser::Def def;
        ophidian::parser::Lef lef;
        ophidian::parser::Guide guide;
        def = ophidian::parser::Def{def_file};
        lef = ophidian::parser::Lef{lef_file};
        guide = ophidian::parser::Guide{guide_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design(design, def, lef, guide);


        // movement_container_type movements;

        std::set<mevement> possible_movements;
        cells_to_median(design, possible_movements);


        auto csv_file_name = circuit_name + "_movements_to_median.csv";
        write_csv_header(csv_file_name);
        write_csv(design, csv_file_name, possible_movements);

        std::cout << "Memory usage in peak= " << ophidian::util::mem_use::get_peak() << " MB" << std::endl << std::endl;    
    }
}

TEST_CASE("ILSVLSI paper ILP in all circuit", "[ILSVLSI_ILP]")
{
    //iccad 2019 benchmarks
    std::vector<std::string> circuit_names = {
        // "ispd19_sample4"
        "ispd18_test1",
        "ispd18_test2",
        "ispd18_test3",
        "ispd18_test4",
        "ispd18_test5",
        "ispd18_test6",
        "ispd18_test7",
        "ispd18_test8",
        "ispd18_test9",
        "ispd18_test10",
        // "ispd19_test1",
        // "ispd19_test2",
        // "ispd19_test3",
        // "ispd19_test4",
        // "ispd19_test5",
        // "ispd19_test6",
        // "ispd19_test7",
        // "ispd19_test8",
        // "ispd19_test9",
        // "ispd19_test10",
    };

    std::string benchmarks_path = "./input_files/circuits";

    for (auto circuit_name : circuit_names) {
        int iterations = 50;
        std::string log_filename = circuit_name + "_" + std::to_string(iterations) + "_iterations_log.csv";

        std::cout << "running circuit " << circuit_name << std::endl;

        std::string def_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.def";
        std::string lef_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.lef";
        std::string guide_file = benchmarks_path + "/cadence_solution/" + circuit_name + ".guide";
        // std::string guide_file = benchmarks_path + "/cu_gr_solution_tiago/guides/" + circuit_name + ".solution_cugr.guide";
        // std::string guide_file = benchmarks_path + "/cu_gr_solution_nopatch_connected/" + circuit_name + ".solution_cugr_ophidian.guide";
        ophidian::parser::Def def;
        ophidian::parser::Lef lef;
        ophidian::parser::Guide guide;
        def = ophidian::parser::Def{def_file};
        lef = ophidian::parser::Lef{lef_file};
        guide = ophidian::parser::Guide{guide_file, true};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design(design, def, lef, guide);

        //ispd19_sample4
        // auto net_2037 = design.netlist().find_net("n_2037");
        // draw_gcell_svg(design, "n_2037");
        // auto wrl = design.global_routing().wirelength(net_2037);
        // auto vias = design.global_routing().number_of_vias(net_2037);
        // log() << "wirelength : " << wrl << " vias : " << vias << std::endl;


        std::vector<ophidian::circuit::Net> nets{design.netlist().begin_net(), design.netlist().end_net()};
        auto initial_wrl = design.global_routing().wirelength(nets) ;
        log() << "Initial wirelength " << initial_wrl << std::endl;
        auto initial_vias = design.global_routing().number_of_vias(nets);
        log() << "Initial Vias " << initial_vias << std::endl;
        log() << "Design connected? " << design.global_routing().is_connected(design.netlist(), nets) << std::endl;

        auto number_of_nets = nets.size();
        auto number_of_cells = design.netlist().size_cell_instance();
        

        UCal::Engine engine(design);
        movement_container_type movements;
        
       
        if(!fileExists(log_filename)){
            std::ofstream iter_log_file(log_filename, std::ofstream::out);
            // iter_log_file << "iter,acum_time,acum_mov,time,num_mov,wirelength,reduction" << std::endl;

            iter_log_file << "Circuit,nets,std,Iteration,movements,total_movements,cells_moved,wrl_baseline,wrl_optimized,wrl_reduction,via_baseline,via_optimized,via_reduction,runtime,acumulated" << std::endl;

            // Circuit	# nets	# std	Iterations	# movements	total movements	% cells moved	baseline	optimized	reduction %	baseline	optimized	reduction %	runtime	acumulated
        }
        std::ofstream iter_log_file(log_filename, std::ofstream::app);
        
        auto start_time = std::chrono::steady_clock::now();

        int num_mov = movements.size();

        auto last_reduction = 0.0;
        for (int i = 0; i < iterations; i++)
        {
            log() << "Initing ILP iteration : " << i << std::endl;
            auto iteration_start_time = std::chrono::steady_clock::now();
            auto report_json = engine.run(movements,iteration_start_time);
            auto current_time = std::chrono::steady_clock::now();

            std::chrono::duration<double> iteration_time = current_time-iteration_start_time;
            std::chrono::duration<double> technique_time = current_time-start_time;

            int number_movements = movements.size() - num_mov;
            num_mov = movements.size();

            log() << "Iteration time : " << iteration_time.count() << " sec | " << (iteration_time.count()/60) << " min" << std::endl;
            log() << "Number of movements : " << number_movements << std::endl;
            log() << "Technique time : " << technique_time.count() << " sec | " << (technique_time.count()/60) << " min" << std::endl;
            log() << "TOTAL of movements : " << movements.size() << std::endl;

            auto wrl = design.global_routing().wirelength(nets);
            auto wrl_reduction = (1.0 - wrl/(double)initial_wrl);
            log() << "reduction " <<  wrl_reduction << " %" << std::endl;

            auto vias = design.global_routing().number_of_vias(nets);
            auto via_reduction = (1.0 - vias/(double)initial_vias);
            log() << "Via reduction " <<  via_reduction << " %" << std::endl;

            auto percentage_cell_moved = number_movements/(double)number_of_cells;

            //  iter_log_file << "iter,acum_time,acum_mov,time,num_mov" << std::endl;
            // iter_log_file << "Circuit,nets,std,Iteration,movements,total_movements,cells_moved,wrl_baseline,wrl_optimized,wrl_reduction,via_baseline,via_optimized,via_reduction,runtime,acumulated" << std::endl;
            iter_log_file << circuit_name << "," << 
                            number_of_nets << "," << 
                            number_of_cells << "," << 
                            i << "," <<
                            num_mov << "," << 
                            number_movements << "," << 
                            percentage_cell_moved << "," << 
                            initial_wrl << "," << 
                            wrl << "," << 
                            wrl_reduction << "," << 
                            initial_vias << "," <<
                            vias << "," <<
                            via_reduction << "," <<
                            iteration_time.count() << "," << 
                            technique_time.count() << std::endl;
            /*
            for(auto mov : movements)
            {
                auto cell = mov.first;
                design.placement().fixLocation(cell);
            }
            */

            //stop criteria
            double diff = wrl_reduction - last_reduction;
            last_reduction = wrl_reduction;
            if(diff <= 0.0001){
                log() << "Stoping by stop criteria!" << std::endl;
                break;
            }
        }
        iter_log_file.close();

        log() << "Design connected? " << design.global_routing().is_connected(design.netlist(), nets) << std::endl;

        auto end_wrl = design.global_routing().wirelength(nets);
        log() <<"Initial wrl " << initial_wrl << std::endl;
        log() << "Final wrl " << end_wrl << std::endl;
        log() << "Improve " << initial_wrl - end_wrl << std::endl;
        log() << "reduction " << (1.0 - end_wrl/(double)initial_wrl)*100.0 << " %" << std::endl;

        auto csv_file_name = circuit_name + "_" + std::to_string(iterations) + "_iterations_movements_ILP_to_median_comercial.csv";
        write_csv(design, csv_file_name, movements);

        auto guide_file_name = circuit_name + "_iterations.guide";
        ophidian::parser::write_guide(design, guide_file_name);

        log() << "Memory usage in peak= " << ophidian::util::mem_use::get_peak() << " MB" << std::endl << std::endl;    
    }
}




}//end namespace