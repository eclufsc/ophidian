#include "Placement.h"

namespace ophidian {
namespace placement {

Placement::Placement(const circuit::Netlist &netlist)
    : netlist_(netlist), locations_(netlist_.makeProperty<util::Location>(circuit::Cell())){

}

Placement::~Placement()
{

}

void Placement::placeCell(const circuit::Cell & cell, const util::Location & location)
{
    locations_[cell] = location;
}


} //namespace placement

} //namespace ophidian
