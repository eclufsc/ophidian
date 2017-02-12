#include "Library.h"

namespace ophidian {
namespace placement {

Library::Library(standard_cell::StandardCells &std_cells)
    : std_cells_(std_cells), geometries_(std_cells_.makeProperty<geometry::MultiBox>(standard_cell::Cell())),
      pinOffsets_(std_cells_.makeProperty<util::Location>(standard_cell::Pin())) {

}

void Library::geometry(standard_cell::Cell cell, geometry::MultiBox geometry)
{
    geometries_[cell] = geometry;
}

void Library::pinOffset(standard_cell::Pin pin, util::Location offset)
{
    pinOffsets_[pin] = offset;
}



}
}
