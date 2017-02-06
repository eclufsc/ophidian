#include "Placement.h"

namespace ophidian {
namespace placement {

Placement::Placement()
    : locations_(cells_){

}

Placement::~Placement()
{

}

Cell Placement::add(Cell, util::Location location)
{
    auto cell = cells_.add();
    locations_[cell] = location;
    return cell;
}

void Placement::erase(Cell cell)
{
    cells_.erase(cell);
}


} //namespace placement

} //namespace ophidian
