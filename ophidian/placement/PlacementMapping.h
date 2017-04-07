#ifndef PLACEMENTMAPPING_H
#define PLACEMENTMAPPING_H

#include <ophidian/placement/Placement.h>
#include <ophidian/placement/Library.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/geometry/Operations.h>

namespace ophidian {
namespace placement {
class PlacementMapping
{
public:
    PlacementMapping(const Placement & placement, const Library & library, const circuit::Netlist & netlist, const circuit::LibraryMapping & libraryMapping);

    util::MultiBox geometry(const circuit::Cell & cell) const;

    util::Location location(const circuit::Pin & pin) const;

private:
    const Placement & placement_;
    const Library & library_;
    const circuit::Netlist & netlist_;
    const circuit::LibraryMapping & libraryMapping_;
};
}
}

#endif // PLACEMENTMAPPING_H
