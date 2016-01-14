//
// Created by renan on 1/14/16.
//

#ifndef OPENEDA_PLACEMENT_PLACEMENT_H
#define OPENEDA_PLACEMENT_PLACEMENT_H

#include "placement_interface.h"

namespace openeda {
    namespace placement {
        class placement : public placement_interface {

        public:

            placement() { }

            virtual void place_cell(netlist::cell_identifier id, geometry::point<double> position);

            virtual void change_cell_geometries(netlist::cell_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries);

            virtual geometry::point<double> cell_position(netlist::cell_identifier id);

            virtual geometry::multi_polygon<geometry::polygon<geometry::point<double>>> cell_geometries(netlist::cell_identifier id);

            virtual void place_pin(netlist::pin_identifier id, geometry::point<double> position);

            virtual void change_pin_geometries(netlist::pin_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries);

            virtual geometry::point<double> pin_position(netlist::pin_identifier id);

            virtual geometry::multi_polygon<geometry::polygon<geometry::point<double>>> pin_geometries(netlist::pin_identifier id);

            virtual block_identifier insert_block(std::string name, geometry::point<double> position,
                                                  geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries);

            virtual void place_block(block_identifier id, geometry::point<double> position);

            virtual void change_block_geometries(block_identifier id, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries);

            virtual std::string block_name(block_identifier id);

            virtual geometry::point<double> block_position(block_identifier id);

            virtual geometry::multi_polygon<geometry::polygon<geometry::point<double>>> block_geometries(block_identifier id);
        };
    }
}


#endif //OPENEDA_PLACEMENT_H
