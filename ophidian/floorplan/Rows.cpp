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

void Rows::origin(Row row, geometry::Location loc)
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

geometry::Location Rows::origin(Row row) const
{
    return origins_[row];
}

size_t Rows::numberOfSites(Row row) const
{
    return numberOfSites_[row];
}

Site Rows::site(Row row) const
{
    return siteTypeOfRow_[row];
}

ophidian::util::Range<Rows::RowsIterator> Rows::rows_range() const
{
    return util::Range<Rows::RowsIterator>(rows_.begin(), rows_.end());
}

} //namespace floorplan

} //namespace ophidian
