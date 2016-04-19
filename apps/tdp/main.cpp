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

int main(int argc, char **argv) {

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

    // calls STA
    tdp.update_timing();

    // prints WNS late
    std::cout << "WNS Late: " << tdp.late_wns() << std::endl;

    // places cell `u1` at (1000,1500)
    tdp.place_cell(tdp.cell_find("u1"), timingdriven_placement::Point(1000,1500));

    // iterates over each cell and prints its name and geometry in WKT format
    for(auto cell : tdp.cells())
    {
        std::cout << tdp.cell_name(cell);
        std::cout << "\t" << geometry::wkt(tdp.cell_geometry(cell)) << std::endl;
    }

    // calls STA
    tdp.update_timing();

    // prints WNS late
    std::cout << "WNS Late: " << tdp.late_wns() << std::endl;

    std::cout << "bye :)" << std::endl;
	return 0;
}
