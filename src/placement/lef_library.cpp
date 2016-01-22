//
// Created by renan on 1/14/16.
//

#include "lef_library.h"

namespace openeda {
    namespace placement {

        macro_identifier lef_library::insert_macro(std::string name,
                                                   geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries) {
            return 0;
        }

        std::string lef_library::macro_name(macro_identifier id) {
            return "";
        }

        geometry::multi_polygon<geometry::polygon<geometry::point<double>>> lef_library::macro_geometries(macro_identifier id) {
            return geometry::multi_polygon<geometry::polygon<geometry::point<double>>>();
        }

        pin_identifier lef_library::insert_pin(macro_identifier owner, std::string name, geometry::point<double> offset,
                                               geometry::polygon<geometry::point<double>> geometry) {
            return 0;
        }

        macro_identifier lef_library::pin_owner(pin_identifier id) {
            return 0;
        }

        std::string lef_library::pin_name(pin_identifier id) {
            return "";
        }

        geometry::point<double> lef_library::pin_offset(pin_identifier id) {
            return geometry::point<double>();
        }

        geometry::polygon<geometry::point<double>> lef_library::pin_geometry(pin_identifier id) {
            return geometry::polygon<geometry::point<double>>();
        }
    }
}
