#include "Floorplan.h"

namespace ophidian {

namespace floorplan {

Floorplan::Floorplan()
    : chipOrigin_(0.0, 0.0), chipUpperRightCorner_(0.0, 0.0),
      origins_(rows_), numberOfSites_(rows_), siteTypeOfRow_(rows_),
      names_(sites_), dimensions_(sites_)
{

}

Floorplan::~Floorplan()
{

}

void Floorplan::chipOrigin(util::Location loc)
{
    chipOrigin_ = loc;
}

void Floorplan::chipUpperRightCorner(util::Location loc)
{
    chipUpperRightCorner_ = loc;
}

Site Floorplan::add(Site, std::string name, util::Location loc)
{
    auto site = sites_.add();
    names_[site] = name;
    dimensions_[site] = loc;
    return site;
}

void Floorplan::erase(Site site)
{
    sites_.erase(site);
}

Row Floorplan::add(Row, util::Location loc, size_t num, Site site)
{
    auto row = rows_.add();
    origins_[row] = loc;
    numberOfSites_[row] = num;
    siteTypeOfRow_[row] = site;
    return row;
}

void Floorplan::erase(Row row)
{
    rows_.erase(row);
}

util::Location Floorplan::rowUpperRightCorner(Row row) const
{
    auto site = siteTypeOfRow_[row];
    size_t numSites = numberOfSites_[row];
    util::Location uRCorner = dimensions_[site];
    return util::Location(uRCorner.x() * numSites, uRCorner.y());
}

} //namespace floorplan

} //namespace ophidian
