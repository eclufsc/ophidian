//
// Created by renan on 1/14/16.
//

#ifndef OPENEDA_PLACEMENT_PLACEMENT_INTERFACE_H_H
#define OPENEDA_PLACEMENT_PLACEMENT_INTERFACE_H_H

#include "../geometry/geometry.h"
#include "../netlist/netlist_interface.h"

namespace openeda {
    namespace placement {
        typedef std::size_t block_identifier;

        class placement_interface {
        public:
            virtual ~placement_interface() {}

            virtual void place_cell(netlist::cell_identifier id, geometry::point<double> position) = 0;
            virtual void change_cell_geometries(netlist::cell_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) = 0;

            virtual geometry::point<double> cell_position(netlist::cell_identifier id) = 0;
            virtual geometry::multi_polygon<geometry::polygon<geometry::point<double>>> cell_geometries(netlist::cell_identifier id) = 0;

            virtual void place_pin(netlist::pin_identifier id, geometry::point<double> position) = 0;
            virtual void change_pin_geometries(netlist::pin_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) = 0;

            virtual geometry::point<double> pin_position(netlist::pin_identifier id) = 0;
            virtual geometry::multi_polygon<geometry::polygon<geometry::point<double>>> pin_geometries(netlist::pin_identifier id) = 0;

            virtual block_identifier insert_block(std::string name, geometry::point<double> position, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) = 0;
            virtual void place_block(block_identifier id, geometry::point<double> position) = 0;
            virtual void change_block_geometries(block_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) = 0;

            virtual std::string block_name(block_identifier id) = 0;
            virtual geometry::point<double> block_position(block_identifier id) = 0;
            virtual geometry::multi_polygon<geometry::polygon<geometry::point<double>>> block_geometries(block_identifier id) = 0;
        };
    }
}

#endif //OPENEDA_PLACEMENT_INTERFACE_H_H
