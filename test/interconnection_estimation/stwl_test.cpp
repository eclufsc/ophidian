//
// Created by renan on 3/10/16.
//

#include "../catch.hpp"

#include "../placement/placement.h"
#include "../../src/interconnection_estimation/stwl_estimation.h"

TEST_CASE("interconnection_estimation/stwl test with two points", "[interconnection_estimation][stwl]") {
    openeda::standard_cell::standard_cells std_cells;
    openeda::netlist::netlist netlist(&std_cells);
    openeda::placement::library lib { &std_cells };

    // create lib
    auto INV_X1 = std_cells.cell_create("INV_X1");
    auto INV_X2 = std_cells.cell_create("INV_X2");

    auto INV_X1_z = std_cells.pin_create(INV_X1, "z");
    auto INV_X2_a = std_cells.pin_create(INV_X2, "a");

    using point = openeda::geometry::point<double>;
    using polygon = openeda::geometry::polygon<point>;
    using multipolygon = openeda::geometry::multi_polygon< polygon >;

    auto X1_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 128.0,
                                                                       200.0 }, point { 128.0, 0.0 }, point { 0.0, 0.0 } };

    auto X2_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 256.0,
                                                                       200.0 }, point { 256.0, 0.0 }, point { 0.0, 0.0 } };


    multipolygon X1 { polygon { X1_points } };
    multipolygon X2 { polygon { X2_points } };

    lib.geometry(INV_X1, X1);
    lib.geometry(INV_X2, X2);

    lib.pin_offset(INV_X1_z, {80.0, 50.0});
    lib.pin_offset(INV_X2_a, {10.0, 60.0});

    openeda::placement::placement placement { &netlist, &lib };

    auto u1 = netlist.cell_insert("u1", "INV_X1");
    auto u2 = netlist.cell_insert("u2", "INV_X2");

    auto u1_z = netlist.pin_insert(u1, "z");
    auto u2_a = netlist.pin_insert(u2, "a");

    auto n1 = netlist.net_insert("n1");
    netlist.connect(n1, u1_z);
    netlist.connect(n1, u2_a);

    placement.cell_position(u1, {0.0, 0.0});
    placement.cell_position(u2, {130.0, 200.0});

    openeda::interconnection_estimation::stwl_estimation stwl_estimation(netlist, placement);
    double stwl = stwl_estimation.estimate(n1);
    REQUIRE( stwl == 270.0 );

}

TEST_CASE("interconnection_estimation/stwl test with three points", "[interconnection_estimation][stwl]") {
    openeda::standard_cell::standard_cells std_cells;
    openeda::netlist::netlist netlist(&std_cells);
    openeda::placement::library lib { &std_cells };

    // create lib
    auto INV_X1 = std_cells.cell_create("INV_X1");
    auto INV_X2 = std_cells.cell_create("INV_X2");
    auto INV_X3 = std_cells.cell_create("INV_X3");

    auto INV_X1_z = std_cells.pin_create(INV_X1, "z");
    auto INV_X2_a = std_cells.pin_create(INV_X2, "a");
    auto INV_X3_a = std_cells.pin_create(INV_X3, "a");

    using point = openeda::geometry::point<double>;
    using polygon = openeda::geometry::polygon<point>;
    using multipolygon = openeda::geometry::multi_polygon< polygon >;

    auto X1_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 128.0,
                                                                       200.0 }, point { 128.0, 0.0 }, point { 0.0, 0.0 } };

    auto X2_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 256.0,
                                                                       200.0 }, point { 256.0, 0.0 }, point { 0.0, 0.0 } };
    auto X3_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 384.0,
                                                                       200.0 }, point { 384.0, 0.0 }, point { 0.0, 0.0 } };


    multipolygon X1 { polygon { X1_points } };
    multipolygon X2 { polygon { X2_points } };
    multipolygon X3 { polygon { X3_points } };

    lib.geometry(INV_X1, X1);
    lib.geometry(INV_X2, X2);
    lib.geometry(INV_X3, X3);

    lib.pin_offset(INV_X1_z, {80.0, 50.0});
    lib.pin_offset(INV_X2_a, {10.0, 60.0});
    lib.pin_offset(INV_X3_a, {5.0, 40.0});

    openeda::placement::placement placement { &netlist, &lib };

    auto u1 = netlist.cell_insert("u1", "INV_X1");
    auto u2 = netlist.cell_insert("u2", "INV_X2");
    auto u3 = netlist.cell_insert("u3", "INV_X3");

    auto u1_z = netlist.pin_insert(u1, "z");
    auto u2_a = netlist.pin_insert(u2, "a");
    auto u3_a = netlist.pin_insert(u3, "a");

    auto n1 = netlist.net_insert("n1");
    netlist.connect(n1, u1_z);
    netlist.connect(n1, u2_a);
    netlist.connect(n1, u3_a);

    placement.cell_position(u1, {0.0, 0.0});
    placement.cell_position(u2, {130.0, 200.0});
    placement.cell_position(u3, {400.0, 0.0});

    openeda::interconnection_estimation::stwl_estimation stwl_estimation(netlist, placement);
    double stwl = stwl_estimation.estimate(n1);
    REQUIRE( stwl == 545.0 );

}

TEST_CASE("interconnection_estimation/stwl test with four points", "[interconnection_estimation][stwl]") {
    openeda::standard_cell::standard_cells std_cells;
    openeda::netlist::netlist netlist(&std_cells);
    openeda::placement::library lib { &std_cells };

    // create lib
    auto INV_X1 = std_cells.cell_create("INV_X1");
    auto INV_X2 = std_cells.cell_create("INV_X2");
    auto INV_X3 = std_cells.cell_create("INV_X3");
    auto INV_X4 = std_cells.cell_create("INV_X4");

    auto INV_X1_z = std_cells.pin_create(INV_X1, "z");
    auto INV_X2_a = std_cells.pin_create(INV_X2, "a");
    auto INV_X3_a = std_cells.pin_create(INV_X3, "a");
    auto INV_X4_a = std_cells.pin_create(INV_X4, "a");

    using point = openeda::geometry::point<double>;
    using polygon = openeda::geometry::polygon<point>;
    using multipolygon = openeda::geometry::multi_polygon< polygon >;

    auto X1_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 128.0,
                                                                       200.0 }, point { 128.0, 0.0 }, point { 0.0, 0.0 } };

    auto X2_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 256.0,
                                                                       200.0 }, point { 256.0, 0.0 }, point { 0.0, 0.0 } };
    auto X3_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 384.0,
                                                                       200.0 }, point { 384.0, 0.0 }, point { 0.0, 0.0 } };
    auto X4_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 512.0,
                                                                       200.0 }, point { 512.0, 0.0 }, point { 0.0, 0.0 } };


    multipolygon X1 { polygon { X1_points } };
    multipolygon X2 { polygon { X2_points } };
    multipolygon X3 { polygon { X3_points } };
    multipolygon X4 { polygon { X4_points } };

    lib.geometry(INV_X1, X1);
    lib.geometry(INV_X2, X2);
    lib.geometry(INV_X3, X3);
    lib.geometry(INV_X4, X4);

    lib.pin_offset(INV_X1_z, {80.0, 50.0});
    lib.pin_offset(INV_X2_a, {10.0, 60.0});
    lib.pin_offset(INV_X3_a, {5.0, 40.0});
    lib.pin_offset(INV_X4_a, {7.0, 45.0});

    openeda::placement::placement placement { &netlist, &lib };

    auto u1 = netlist.cell_insert("u1", "INV_X1");
    auto u2 = netlist.cell_insert("u2", "INV_X2");
    auto u3 = netlist.cell_insert("u3", "INV_X3");
    auto u4 = netlist.cell_insert("u4", "INV_X4");

    auto u1_z = netlist.pin_insert(u1, "z");
    auto u2_a = netlist.pin_insert(u2, "a");
    auto u3_a = netlist.pin_insert(u3, "a");
    auto u4_a = netlist.pin_insert(u4, "a");

    auto n1 = netlist.net_insert("n1");
    netlist.connect(n1, u1_z);
    netlist.connect(n1, u2_a);
    netlist.connect(n1, u3_a);
    netlist.connect(n1, u4_a);

    placement.cell_position(u1, {0.0, 0.0});
    placement.cell_position(u2, {130.0, 200.0});
    placement.cell_position(u3, {400.0, 0.0});
    placement.cell_position(u4, {200.0, 400.0});

    openeda::interconnection_estimation::stwl_estimation stwl_estimation(netlist, placement);
    double stwl = stwl_estimation.estimate(n1);
    REQUIRE( stwl == 797.0 );

}