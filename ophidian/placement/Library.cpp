#include "Library.h"

namespace ophidian {
namespace placement {

Library::Library(const standard_cell::StandardCells &std_cells)
    : std_cells_(std_cells), geometries_(std_cells_.makeProperty<util::MultiBox>(standard_cell::Cell())),
      pinOffsets_(std_cells_.makeProperty<util::Location>(standard_cell::Pin())) {

}

void Library::geometry(const standard_cell::Cell &cell, const util::MultiBox &geometry)
{
    geometries_[cell] = geometry;
}

void Library::pinOffset(const standard_cell::Pin &pin, const util::Location &offset)
{
    pinOffsets_[pin] = offset;
}



}
}
