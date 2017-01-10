#include "Floorplan.h"

namespace ophidian {

namespace floorplan {

Floorplan::Floorplan() : chipOrigin_(0.0, 0.0), chipUpperRightCorner_(0.0, 0.0)
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
    sites_.name(site, name);
    sites_.upperRightCorner(site, loc);
    return site;
}

void Floorplan::erase(Site site)
{
    sites_.erase(site);
}

Row Floorplan::add(Row, util::Location loc, size_t num, Site site)
{
    auto row = rows_.add();
    rows_.origin(row, loc);
    rows_.numberOfSites(row, num);
    rows_.site(row, site);
    return row;
}

void Floorplan::erase(Row row)
{
    rows_.erase(row);
}

util::Location Floorplan::rowUpperRightCorner(Row row) const
{
    auto site = rows_.site(row);
    size_t numSites = rows_.numberOfSites(row);
    util::Location uRCorner = sites_.upperRightCorner(site);
    return util::Location(uRCorner.x() * numSites, uRCorner.y());
}

} //namespace floorplan

} //namespace ophidian
