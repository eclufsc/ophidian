#include "Sites.h"

namespace ophidian {

namespace floorplan {


Sites::Sites() : names_(sites_), dimensions_(sites_)
{

}

Sites::~Sites()
{

}

Site Sites::add()
{
    return sites_.add();
}

void Sites::erase(Site site)
{
    sites_.erase(site);
}

void Sites::name(Site site, std::string name)
{
    names_[site] = name;
}

void Sites::dimension(Site site, geometry::Location loc)
{
    dimensions_[site] = loc;
}

std::string Sites::name(Site site) const
{
    return names_[site];
}

geometry::Location Sites::dimension(Site site) const
{
    return dimensions_[site];
}

ophidian::util::Range<Sites::SitesIterator> Sites::sites_range() const
{
    return ophidian::util::Range<Sites::SitesIterator>(sites_.begin(), sites_.end());
}


} //namespace floorplan

} //namespace ophidian

