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

        auto box_width = max_x - min_x;
        auto box_height = max_y - min_y;

        stats_file << net_name << "," << pins.size() << "," << stwl << "," << routed_length << "," << routed_length_no_vias << "," << box_width << "," << box_height << std::endl;
    }
    stats_file.close();
}


TEST_CASE("run ILP for iccad20 benchmarks", "[iccad20]") {
    std::vector<std::string> circuit_names = {
        // "case1",
        //"case1N4",
        // "case2",
        //"case3",
        "case4",
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
        // "case3",
         "case4",
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
        //run_circuit(design, circuit_name);
        run_mcf_multithreading(design);
    }


}
