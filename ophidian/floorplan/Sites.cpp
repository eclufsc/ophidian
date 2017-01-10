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

void Sites::upperRightCorner(Site site, util::Location loc)
{
	dimensions_[site] = loc;
}


} //namespace floorplan

} //namespace ophidian
