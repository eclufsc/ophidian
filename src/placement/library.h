//
// Created by renan on 1/14/16.
//

#ifndef OPENEDA_PLACEMENT_LIBRARY_H
#define OPENEDA_PLACEMENT_LIBRARY_H

#include "../geometry/geometry.h"

namespace openeda {
    namespace placement {
        typedef std::size_t macro_identifier;
        typedef std::size_t pin_identifier;

        class library {
        public:
            virtual ~library() {}

            virtual macro_identifier insert_macro(std::string name, geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) = 0;

            virtual std::string macro_name(macro_identifier id) = 0;
            virtual geometry::multi_polygon<geometry::polygon<geometry::point<double>>> macro_geometries(macro_identifier id) = 0;

            virtual pin_identifier insert_pin(macro_identifier owner, std::string name, geometry::point<double> offset, geometry::polygon<geometry::point<double>> geometry) = 0;

            virtual macro_identifier pin_owner(pin_identifier id) = 0;
            virtual std::string pin_name(pin_identifier id) = 0;
            virtual geometry::point<double> pin_offset(pin_identifier id) = 0;
            virtual geometry::polygon<geometry::point<double>> pin_geometry(pin_identifier id) = 0;
        };
    }
}

#endif //OPENEDA_LIBRARY_H
