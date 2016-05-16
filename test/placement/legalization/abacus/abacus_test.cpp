//
// Created by renan on 4/29/16.
//

#include <fstream>
#include <lef.h>
#include <def.h>
#include <sys/time.h>
#include "../../../catch.hpp"
#include "abacus.h"
#include "legalization_check.h"

TEST_CASE("legalization/ legalizing simple after random movements","[legalization][abacus]") {
    std::ifstream lef("benchmarks/simple/simple.lef", std::ifstream::in);
    std::ifstream def("benchmarks/simple/simple.def", std::ifstream::in);
    REQUIRE(lef.good());
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement placement(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    ophidian::placement::lef::read(lef, &std_cells, &lib, &floorplan);
    ophidian::placement::def::read(def, &netlist, &placement, &floorplan);

    int min_movement = -100;
    int max_movement = 100;
    std::default_random_engine random_generator;
    std::uniform_int_distribution<int> movement_distribution(min_movement, max_movement);
    double site_width = floorplan.sites_properties().dimensions().first->x();
    double row_height = floorplan.sites_properties().dimensions().first->y();
    for (auto cell : netlist.cell_system()) {
        int movement_x = movement_distribution(random_generator);
        int movement_y = movement_distribution(random_generator);
        ophidian::geometry::point<double> current_position = placement.cell_position(cell.first);
        double new_x = std::min(std::max(floorplan.chip_origin().x(), current_position.x() + movement_x), floorplan.chip_boundaries().x());
        new_x = std::floor(new_x / site_width) * site_width;
        double new_y = std::min(std::max(floorplan.chip_origin().y(), current_position.y() + movement_y), floorplan.chip_boundaries().y());
        new_y = std::floor(new_y / row_height) * row_height;
        ophidian::geometry::point<double> new_position(new_x, new_y);
        placement.cell_position(cell.first, new_position);
    }

    ophidian::placement::legalization::legalization_check legalization_check(&floorplan, &placement);

    REQUIRE(!legalization_check.check_legality());

    ophidian::placement::legalization::abacus::abacus abacus(&floorplan, &placement);
    abacus.legalize_placement();

    REQUIRE(legalization_check.check_legality());
}

TEST_CASE("legalization/ legalizing vga after random movements","[legalization][abacus]") {
    struct timeval start_time, end_time;

    std::ifstream lef("benchmarks/vga_lcd/techlib.lef", std::ifstream::in);
    std::ifstream def("benchmarks/vga_lcd/vga_lcd.def", std::ifstream::in);
    REQUIRE(lef.good());
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement placement(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    ophidian::placement::lef::read(lef, &std_cells, &lib, &floorplan);
    ophidian::placement::def::read(def, &netlist, &placement, &floorplan);

    int min_movement = -100;
    int max_movement = 100;
    std::default_random_engine random_generator;
    std::uniform_int_distribution<int> movement_distribution(min_movement, max_movement);
    double site_width = floorplan.sites_properties().dimensions().first->x();
    double row_height = floorplan.sites_properties().dimensions().first->y();
    for (auto cell : netlist.cell_system()) {
        if (!placement.cell_fixed(cell.first)) {
            int movement_x = movement_distribution(random_generator);
            int movement_y = movement_distribution(random_generator);
            ophidian::geometry::point<double> current_position = placement.cell_position(cell.first);
            double new_x = std::min(std::max(floorplan.chip_origin().x(), current_position.x() + movement_x), floorplan.chip_boundaries().x());
            new_x = std::floor(new_x / site_width) * site_width;
            double new_y = std::min(std::max(floorplan.chip_origin().y(), current_position.y() + movement_y), floorplan.chip_boundaries().y());
            new_y = std::floor(new_y / row_height) * row_height;
            ophidian::geometry::point<double> new_position(new_x, new_y);
            placement.cell_position(cell.first, new_position);
        }
    }

    ophidian::placement::legalization::legalization_check legalization_check(&floorplan, &placement);

    REQUIRE(!legalization_check.check_legality());

    ophidian::placement::legalization::abacus::abacus abacus(&floorplan, &placement);
    gettimeofday(&start_time, NULL);
    abacus.legalize_placement();
    gettimeofday(&end_time, NULL);
    float runtime = end_time.tv_sec - start_time.tv_sec;
    std::cout << "runtime " << runtime << std::endl;

    REQUIRE(legalization_check.check_legality());
}
