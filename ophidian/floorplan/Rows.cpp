#include "Rows.h"

namespace ophidian {
namespace floorplan {

Rows::Rows() : origins_(rows_), numberOfSites_(rows_), siteTypeOfRow_(rows_)
{

}

Rows::~Rows()
{

}

Row Rows::add()
{
    return rows_.add();
}

void Rows::erase(Row row)
{
    rows_.erase(row);
}

void Rows::origin(Row row, util::Location loc)
{
    origins_[row] = loc;
}

void Rows::numberOfSites(Row row, size_t num)
{
    numberOfSites_[row] = num;
}

void Rows::site(Row row, Site site)
{
    siteTypeOfRow_[row] = site;
}

} //namespace floorplan

} //namespace ophidian
