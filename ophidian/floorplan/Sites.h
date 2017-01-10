#ifndef OPHIDIAN_FLOORPLAN_SITES_H
#define OPHIDIAN_FLOORPLAN_SITES_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/util/Range.h>
#include <ophidian/util/Units.h>

namespace ophidian {
namespace floorplan {

class Site : public entity_system::EntityBase
{
public:
	using entity_system::EntityBase::EntityBase;
};

/*! An entity system of sites */
class Sites
{
public:
	using SitesIterator = entity_system::EntitySystem<Site>::const_iterator;

	//! Sites Constructor
	/*!
	   \brief Constructs an empty system with no Sites
	 */
	Sites();

	//! Sites Destructor
	/*!
	   \brief Destroys the site EntitySystem, including its properties.
	 */
	~Sites();

	//! Add Site
	/*!
	   \brief Adds a Site instance.
	   \return A handler for the created Site.
	 */
	Site add();

	//! Erase Site
	/*!
	   \brief erases a Site instance.
	   \param site Site entity to erase.
	 */
	void erase(Site site);

	//! Name setter
	/*!
	   \brief Set the name of a given site
	   \param site Site entity to set the name.
	   \param name Name of the site.
	 */
	void name(Site site, std::string name);

	//! Dimensions setter
	/*!
	   \brief Set the dimension of a given site
	   \param site Site entity to set the dimension.
	   \param dimension Dimension of the site.
	 */
	void upperRightCorner(Site site, util::Location loc);

	//! Name getter
	/*!
	   \brief Get the name of a given site
	   \param site Site entity to set the name.
	   \return Name of the site.
	 */
	std::string name(Site site) const
	{
		return names_[site];
	}

	//! Upper right corner getter
	/*!
	   \brief get the upper right corner of a given site
	   \param site Site entity to get the upper right corner.
	   \return Upper right corner of the site.
	 */
	util::Location upperRightCorner(Site site) const
	{
		return dimensions_[site];
	}

	//! Sites iterator
	/*!
	   \return Range iterator for the Sites.
	 */
	ophidian::util::Range<SitesIterator> sites_range() const
	{
		return ophidian::util::Range<Sites::SitesIterator>(sites_.begin(), sites_.end());
	}

private:
	entity_system::EntitySystem<Site> sites_;
	entity_system::Property<Site, std::string> names_;
	entity_system::Property<Site, util::Location> dimensions_;
};

}         //namespace floorplan

} //namespace ophidian

#endif // OPHIDIAN_FLOORPLAN_SITES_H
