//
// Created by renan on 1/14/16.
//

#ifndef OPENEDA_PLACEMENT_LEF_LIBRARY_H
#define OPENEDA_PLACEMENT_LEF_LIBRARY_H

#include "library.h"

namespace openeda {
    namespace placement {
        class lef_library : public library  {
        public:
            lef_library() { }

            virtual macro_identifier insert_macro(std::string name,
                                                  geometry::multi_polygon<geometry::polygon<geometry::point<double>>> geometries);

            virtual std::string macro_name(macro_identifier id);

            virtual geometry::multi_polygon<geometry::polygon<geometry::point<double>>> macro_geometries(macro_identifier id);

            virtual pin_identifier insert_pin(macro_identifier owner, std::string name, geometry::point<double> offset,
                                              geometry::polygon<geometry::point<double>> geometry);

            virtual macro_identifier pin_owner(pin_identifier id);

            virtual std::string pin_name(pin_identifier id);

            virtual geometry::point<double> pin_offset(pin_identifier id);

            virtual geometry::polygon<geometry::point<double>> pin_geometry(pin_identifier id);
        };
    }
}


#endif //OPENEDA_LEF_LIBRARY_H
