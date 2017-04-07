#include "PlacementMapping.h"

namespace ophidian {
namespace placement {
PlacementMapping::PlacementMapping(const Placement &placement, const Library &library, const circuit::Netlist &netlist, const circuit::LibraryMapping &libraryMapping)
    : placement_(placement), library_(library), netlist_(netlist), libraryMapping_(libraryMapping) {

}

util::MultiBox PlacementMapping::geometry(const circuit::Cell &cell) const
{
    auto stdCell = libraryMapping_.cellStdCell(cell);
    auto stdCellGeometry = library_.geometry(stdCell);
    auto cellLocation = placement_.location(cell);
    util::MultiBox cellGeometry = stdCellGeometry.translate(units::unit_cast<double>(cellLocation.x()), units::unit_cast<double>(cellLocation.y()));
    return cellGeometry;
}

util::Location PlacementMapping::location(const circuit::Pin &pin) const
{
    auto stdCellPin = libraryMapping_.pinStdCell(pin);
    auto pinOwner = netlist_.cell(pin);
    auto cellLocation = placement_.location(pinOwner);
    auto pinOffset = library_.pinOffset(stdCellPin);
    util::Location pinLocation(cellLocation.x() + pinOffset.x(), cellLocation.y() + pinOffset.y());
    return pinLocation;
}
}
}
