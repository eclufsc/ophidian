#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>

/*
void write_statistics_for_circuit(ophidian::design::Design & design, std::string circuit_name) {
    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());

    std::ofstream stats_file("stats/" + circuit_name + "_nets.csv");
    stats_file << "net,pins,stwl,routed_length,routed_length_no_vias,box_width,box_height" << std::endl;
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

        auto routes = design.global_routing().routes(net);
        auto routed_length_no_vias = 0;
        auto via_length = 0;
        for (auto route : routes) {
            auto start = design.global_routing().start(route);
            auto end = design.global_routing().end(route);
            routed_length_no_vias += (std::abs(start.x().value() - end.x().value()) + std::abs(start.y().value() - end.y().value()));

            auto start_layer = design.global_routing().start_layer(route);
            auto end_layer = design.global_routing().end_layer(route);
            auto start_layer_index = design.routing_library().layerIndex(start_layer);
            auto end_layer_index = design.routing_library().layerIndex(end_layer);
            via_length += std::abs(start_layer_index - end_layer_index);
        }
        routed_length_no_vias /= 10;
        if (routed_length_no_vias == 0) {
            routed_length_no_vias = 1;
        }
        auto routed_length = routed_length_no_vias + via_length;

        auto box_width = max_x - min_x;
        auto box_height = max_y - min_y;

        stats_file << net_name << "," << pins.size() << "," << stwl << "," << routed_length << "," << routed_length_no_vias << "," << box_width << "," << box_height << std::endl;
    }
    stats_file.close();
}
*/



void run_ilp_for_circuit(ophidian::design::Design & design, std::string circuit_name) {
    ophidian::routing::ILPRouting ilpRouting(design, circuit_name);
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    // std::vector<ophidian::circuit::Net> nets = {design.netlist().find_net("N376")};

    std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements; 
    std::cout << "routing nets" << std::endl;
    auto result = ilpRouting.route_nets(nets, movements);
    std::cout << "result " << result << std::endl;

    if(result){
        iccad_output_writer.write_ICCAD_2020_output("", movements);
    }
   
    //write_statistics_for_circuit(design, circuit_name);
}

TEST_CASE("run ILP for iccad20 benchmarks", "[iccad20]") {
    std::vector<std::string> circuit_names = {
        // "case1",
        //"case1N4",
        //"case2",
        // "case3",
        "case3_no_blockages",
        // "case3_no_extra_demand"
    };

    std::string benchmarks_path = "./input_files/iccad20/";
    // std::string benchmarks_path = "./input_files/iccad2020/cases/";
    for (auto circuit_name : circuit_names) {
        std::cout << "running circuit " << circuit_name << std::endl;

        std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";

        std::cout << "file " << iccad_2020_file << std::endl;

        auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
        run_ilp_for_circuit(design, circuit_name);
    }
}

TEST_CASE("iccad20 case 3 no extra demand benchmark", "[iccad20case3]") {
    std::string circuit_name = "case3_no_extra_demand";
    std::string benchmarks_path = "./input_files/iccad20/";
    std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";
    auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};
    auto design = ophidian::design::Design();
    ophidian::design::factory::make_design_iccad2020(design, iccad_2020);

    ophidian::routing::ILPRouting ilpRouting(design, circuit_name);
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);
    auto & global_routing = design.global_routing();

    using tuple = std::tuple<int, int, int>;
    std::vector<std::pair<tuple, tuple>> initial_segments;
    initial_segments.push_back(std::make_pair(std::make_tuple(21, 18, 2), std::make_tuple(21, 19, 2)));
    initial_segments.push_back(std::make_pair(std::make_tuple(21, 17, 2), std::make_tuple(21, 18, 2)));
    initial_segments.push_back(std::make_pair(std::make_tuple(21, 19, 1), std::make_tuple(21, 19, 2)));
    initial_segments.push_back(std::make_pair(std::make_tuple(21, 17, 1), std::make_tuple(21, 17, 2)));
    initial_segments.push_back(std::make_pair(std::make_tuple(21, 18, 1), std::make_tuple(21, 18, 2)));
    initial_segments.push_back(std::make_pair(std::make_tuple(21, 17, 1), std::make_tuple(22, 17, 1)));
    initial_segments.push_back(std::make_pair(std::make_tuple(21, 18, 1), std::make_tuple(22, 18, 1)));

    auto net = design.netlist().find_net("N2548");
    auto segments = global_routing.segments(net);

    std::vector<std::pair<tuple, tuple>> gr_segments;
    for(auto segment : segments){
        auto box = global_routing.box(segment);
        auto start = box.min_corner();
        auto end = box.max_corner();
        auto start_layer = global_routing.layer_start(segment);
        auto end_layer = global_routing.layer_end(segment);
        // auto gcell_start = global_routing.gcell_start(segment);
        // auto gcell_end = global_routing.gcell_end(segment);
        auto start_layer_index = design.routing_library().layerIndex(start_layer);
        auto end_layer_index = design.routing_library().layerIndex(end_layer);

        gr_segments.push_back(std::make_pair(
            std::make_tuple( (((int)start.x().value() - 5) / 10 +1), (((int)start.y().value() - 5) / 10 +1), start_layer_index),
            std::make_tuple( (((int)end.x().value() - 5) / 10 +1), (((int)end.y().value() - 5) / 10 +1), end_layer_index)));
    }

    // TODO: compare if initial_segments and gr_segments are equal!


    std::vector<ophidian::circuit::Net> nets = {design.netlist().find_net("N2548")};
    std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements; 
    
    std::cout << "routing nets" << std::endl;
    auto result = ilpRouting.route_nets(nets, movements);
    std::cout << "result " << result << std::endl;

    if(result){
        iccad_output_writer.write_ICCAD_2020_output();
    }

    int wirelength = 0;
    for(auto net : nets){
        auto gcells = global_routing.gcells(net);
        auto net_name = design.netlist().name(net);
        wirelength += gcells.size();
        std::cout << "Net: " << net_name << " = " << gcells.size() << std::endl;
    }
    std::cout << "wirelength : " << wirelength << std::endl;
}

/*
TEST_CASE("write statistics for iccad20 benchmarks", "[iccad20]") {
    std::vector<std::string> circuit_names = {
        "case1",
        "case2",
        "case3",
    };
    std::string benchmarks_path = "./input_files/iccad2020/cases/";

    for (auto circuit_name : circuit_names) {
        std::cout << "running circuit " << circuit_name << std::endl;

        std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";

        auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design_iccad2020(design, iccad_2020);

        write_statistics_for_circuit(design, circuit_name);
    }
}
*/
