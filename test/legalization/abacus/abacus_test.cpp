//
// Created by renan on 4/29/16.
//

#include <fstream>
#include <sys/time.h>
#include "../../catch.hpp"

#include "lef.h"
#include "def.h"

#include "def2placement.h"
#include "lef2library.h"
#include "lefdef2floorplan.h"
#include "abacus.h"
#include "legalization_check.h"

TEST_CASE("legalization/ legalizing simple after random movements","[legalization][abacus]") {
    ophidian::parsing::lef lef("benchmarks/simple/simple.lef");
    ophidian::parsing::def def("benchmarks/simple/simple.def");

    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement placement(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    ophidian::placement::def2placement(def, placement);
    ophidian::placement::lef2library(lef, lib);
    ophidian::floorplan::lefdef2floorplan(lef, def, floorplan);

    int min_movement = -100;
    int max_movement = 100;
    std::default_random_engine random_generator;
    std::uniform_int_distribution<int> movement_distribution(min_movement, max_movement);
    double site_width = floorplan.sites_properties().dimensions().first->x();
    double row_height = floorplan.sites_properties().dimensions().first->y();
    for (auto cell : netlist.cell_system()) {
        if (!placement.cell_fixed(cell)) {
            int movement_x = movement_distribution(random_generator);
            int movement_y = movement_distribution(random_generator);
            ophidian::geometry::point<double> current_position = placement.cell_position(cell);
            double new_x = std::min(std::max(floorplan.chip_origin().x(), current_position.x() + movement_x), floorplan.chip_boundaries().x() - placement.cell_dimensions(cell).x());
            new_x = std::floor(new_x / site_width) * site_width;
            double new_y = std::min(std::max(floorplan.chip_origin().y(), current_position.y() + movement_y), floorplan.chip_boundaries().y() - placement.cell_dimensions(cell).y());
            new_y = std::floor(new_y / row_height) * row_height;
            ophidian::geometry::point<double> new_position(new_x, new_y);
            placement.cell_position(cell, new_position);
        }
    }

    ophidian::legalization::legalization_check legalization_check(&floorplan, &placement);

    REQUIRE(!legalization_check.check_legality());

    ophidian::legalization::abacus::abacus abacus(&floorplan, &placement);
    abacus.legalize_placement();

    REQUIRE(legalization_check.check_legality());
}

bool run_circuit(std::string circuit_name) {
    struct timeval start_time, end_time;

    std::cout << "running abacus regression test for circuit " << circuit_name << std::endl;

    ophidian::parsing::lef lef("benchmarks/" + circuit_name + "/" + circuit_name + ".lef");
    ophidian::parsing::def def("benchmarks/" + circuit_name + "/" + circuit_name + ".def");

    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement placement(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    ophidian::placement::def2placement(def, placement);
    ophidian::placement::lef2library(lef, lib);
    ophidian::floorplan::lefdef2floorplan(lef, def, floorplan);

    int min_movement = -100;
    int max_movement = 100;
    std::default_random_engine random_generator;
    std::uniform_int_distribution<int> movement_distribution(min_movement, max_movement);
    double site_width = floorplan.sites_properties().dimensions().first->x();
    double row_height = floorplan.sites_properties().dimensions().first->y();
    for (auto cell : netlist.cell_system()) {
        if (!placement.cell_fixed(cell)) {
            int movement_x = movement_distribution(random_generator);
            int movement_y = movement_distribution(random_generator);
            ophidian::geometry::point<double> current_position = placement.cell_position(cell);
            double new_x = std::min(std::max(floorplan.chip_origin().x(), current_position.x() + movement_x), floorplan.chip_boundaries().x() - placement.cell_dimensions(cell).x());
            new_x = std::floor(new_x / site_width) * site_width;
            double new_y = std::min(std::max(floorplan.chip_origin().y(), current_position.y() + movement_y), floorplan.chip_boundaries().y() - placement.cell_dimensions(cell).y());
            new_y = std::floor(new_y / row_height) * row_height;
            ophidian::geometry::point<double> new_position(new_x, new_y);
            placement.cell_position(cell, new_position);
        }
    }

    ophidian::legalization::legalization_check legalization_check(&floorplan, &placement);

    REQUIRE(!legalization_check.check_legality());

    ophidian::legalization::abacus::abacus abacus(&floorplan, &placement);
    gettimeofday(&start_time, NULL);
    abacus.legalize_placement();
    gettimeofday(&end_time, NULL);
    std::cout << "runtime " << end_time.tv_sec - start_time.tv_sec << std::endl;

    REQUIRE(legalization_check.check_legality());
}

TEST_CASE("legalization/ legalizing iccad 2014 circuits after random movements","[legalization][abacus][regression][iccad2014]") {
    run_circuit("vga_lcd");
    run_circuit("b19");
    run_circuit("mgc_edit_dist");
    run_circuit("mgc_matrix_mult");
}

TEST_CASE("legalization/ legalizing iccad 2015 circuits after random movements","[legalization][abacus][regression][iccad2015]") {
    run_circuit("superblue18");
    run_circuit("superblue16");
    run_circuit("superblue4");
    run_circuit("superblue1");
    run_circuit("superblue5");
    run_circuit("superblue3");
}
