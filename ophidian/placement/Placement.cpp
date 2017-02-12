#include "Placement.h"

namespace ophidian {
namespace placement {

Placement::Placement(circuit::Netlist &netlist)
    : netlist_(netlist), locations_(netlist_.makeProperty<util::Location>(circuit::Cell())){

}

Placement::~Placement()
{

}

void Placement::place_cell(circuit::Cell cell, util::Location location)
{
    locations_[cell] = location;
}


} //namespace placement

} //namespace ophidian
