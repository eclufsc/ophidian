#include "PlacementMapping.h"

namespace ophidian {
namespace placement {
PlacementMapping::PlacementMapping(const Placement &placement, const Library &library, const circuit::Netlist &netlist, const circuit::LibraryMapping &libraryMapping)
    : mPlacement(placement), mLibrary(library), mNetlist(netlist), mLibraryMapping(libraryMapping) {

}

geometry::MultiBox PlacementMapping::geometry(const circuit::Cell &cell) const
{
    auto stdCell = mLibraryMapping.cellStdCell(cell);
    auto stdCellGeometry = mLibrary.geometry(stdCell);
    auto cellLocation = mPlacement.cellLocation(cell);
    geometry::MultiBox cellGeometry = stdCellGeometry.translate(cellLocation.toPoint());
    return cellGeometry;
}

util::LocationDbu PlacementMapping::location(const circuit::Pin &pin) const
{
    auto stdCellPin = mLibraryMapping.pinStdCell(pin);
    auto pinOwner = mNetlist.cell(pin);
    auto cellLocation = mPlacement.cellLocation(pinOwner);
    auto pinOffset = mLibrary.pinOffset(stdCellPin);
    util::LocationDbu pinLocation(cellLocation.x() + pinOffset.x(), cellLocation.y() + pinOffset.y());
    return pinLocation;
}
}
}
