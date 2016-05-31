#include <iostream>
#include "../timing-driven_placement/timingdriven_placement.h"

struct command_line_args {
    std::string dot_v;
    std::string dot_def;
    std::string dot_lef;
    std::string dot_lib_late;
    std::string dot_lib_early;
    double clock;
    command_line_args(char **argv){
        dot_v = argv[1];
        dot_def = argv[2];
        dot_lef = argv[3];
        dot_lib_late = argv[4];
        dot_lib_early = argv[5];
        clock = std::stod(argv[6]);
    }
};

struct net_weight {
    double value;
    net_weight() : value(std::numeric_limits<double>::lowest()) {

    }
};

int main(int argc, char **argv) {

    omp_set_num_threads(omp_get_max_threads());

    using namespace ophidian;

    if (argc != 7) {

        std::cerr << "invalid arguments." << std::endl;
        std::cerr << "usage: " << argv[0] << " <.v> <.def> <.lef> <LATE.lib> <EARLY.lib> <clock in ps>"
                  << std::endl;
        return -1;
    }

    // reads command line arguments
    command_line_args args(argv);

    // initializes the TDP module
    timingdriven_placement::timingdriven_placement tdp(args.dot_v, args.dot_def, args.dot_lef, args.dot_lib_late, args.dot_lib_early, args.clock);

    //    // calls STA
    tdp.update_timing();

    // prints WNS late
    std::cout << "WNS Late: " << tdp.late_wns() << std::endl;

    // places cell `u1` at (1000,1500)
    tdp.place_cell(tdp.cell_find("u1"), timingdriven_placement::Point(1000,1500));

    //     iterates over each cell and prints its name and geometry in WKT format
    for(auto cell : tdp.cells())
    {
        std::cout << tdp.cell_name(cell);
        std::cout << "\t" << geometry::wkt(tdp.cell_geometry(cell)) << std::endl;
    }


    // compute net weights as -slack
    entity_system::vector_property< net_weight > net_weights;
    tdp.net_register_property(net_weights);

    for(std::size_t i{0}; i < 10; ++i)
    {
        for(auto net : tdp.nets())
            net_weights.at(tdp.net_lookup(net)).value = 0.0;
        for(auto pin : tdp.pins())
        {
            auto net = tdp.pin_net(pin);
            auto index = tdp.net_lookup(net);
            auto slack = std::min(tdp.late_rise_slack(pin), tdp.late_fall_slack(pin));
            //            std::cout << "slack for pin " << tdp.pin_name(pin) << ": " << slack << std::endl;
            net_weights.at(index).value = std::max(net_weights.at(index).value, -slack.value());
        }

        for(auto net : tdp.nets())
        {
            std::size_t index = tdp.net_lookup(net);
            //            std::cout << tdp.net_name(net) << " weight " << net_weights[index].value << std::endl;
        }
        std::cout << std::endl;

        for(auto cell : tdp.cells())
        {
            auto curr_position = tdp.cell_position(cell);
            //            std::cout << "  calculating new position for cell " << tdp.cell_name(cell) << " ("<<curr_position.x()<<", " << curr_position.y()<<")" << std::endl;
            const std::vector<timingdriven_placement::Pin> & cell_pins = tdp.cell_pins(cell);
            timingdriven_placement::Point point{0.0, 0.0};
            double weight_sum{0.0};
            for(auto cell_pin : cell_pins)
            {
                auto pin_position = tdp.pin_position(cell_pin);
                //                std::cout << "    cell pin " << tdp.pin_name(cell_pin) << " ("<<pin_position.x()<<", " << pin_position.y()<<")" << std::endl;
                auto net = tdp.pin_net(cell_pin);
                double weight{net_weights[tdp.net_lookup(net)].value};
                //                std::cout << "       net " << tdp.net_name(net) << " weight " << weight << std::endl;
                const std::vector<timingdriven_placement::Net> & net_pins = tdp.net_pins(net);
                for(auto pin : net_pins)
                {
                    if(tdp.pin_owner(pin) == cell) continue;
                    auto net_pin_position = tdp.pin_position(pin);
                    //                    std::cout << "        net pin " << tdp.pin_name(pin) << " ("<<net_pin_position.x()<<", " << net_pin_position.y()<<")" << std::endl;
                    point.x( point.x() + tdp.pin_position(pin).x()*weight );
                    point.y( point.y() + tdp.pin_position(pin).y()*weight );
                    weight_sum += weight;
                }
            }
            point.x( point.x()/weight_sum );
            point.y( point.y()/weight_sum );
            //            std::cout << "  moving cell " << tdp.cell_name(cell) << " from " << tdp.cell_position(cell).x() << ", " << tdp.cell_position(cell).y();
            //            std::cout << " to " << point.x() << ", " << point.y() << std::endl;
            tdp.place_cell(cell, point);
        }

        // calls STA
        tdp.update_timing();
        // prints WNS late
        std::cout << "WNS Late: " << tdp.late_wns() << std::endl;
    }


    std::cout << "bye :)" << std::endl;
    return 0;
}
