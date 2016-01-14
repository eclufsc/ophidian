//
// Created by renan on 1/14/16.
//

#include "placement.h"

namespace openeda {
    namespace placement {

        void placement::place_cell(netlist::cell_identifier id, geometry::point<double> position) {

        }

        void placement::change_cell_geometries(netlist::cell_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) {

        }

        geometry::point<double> placement::cell_position(netlist::cell_identifier id) {
            return geometry::point<double>();
        }

        geometry::multi_polygon<geometry::polygon<geometry::point<double>>> placement::cell_geometries(netlist::cell_identifier id) {
            return geometry::multi_polygon<geometry::polygon<geometry::point<double>>>();
        }

        void placement::place_pin(netlist::pin_identifier id, geometry::point<double> position) {

        }

        void placement::change_pin_geometries(netlist::pin_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) {

        }

        geometry::point<double> placement::pin_position(netlist::pin_identifier id) {
            return geometry::point<double>();
        }

        geometry::multi_polygon<geometry::polygon<geometry::point<double>>> placement::pin_geometries(netlist::pin_identifier id) {
            return geometry::multi_polygon<geometry::polygon<geometry::point<double>>>();
        }

        block_identifier placement::insert_block(std::string name, geometry::point<double> position,
                                                 geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) {
            return 0;
        }

        void placement::place_block(block_identifier id, geometry::point<double> position) {

        }

        void placement::change_block_geometries(block_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) {

        }

        std::string placement::block_name(block_identifier id) {
            return "";
        }

        geometry::point<double> placement::block_position(block_identifier id) {
            return geometry::point<double>();
        }

        geometry::multi_polygon<geometry::polygon<geometry::point<double>>> placement::block_geometries(block_identifier id) {
            return geometry::multi_polygon<geometry::polygon<geometry::point<double>>>();
        }
    }
}
