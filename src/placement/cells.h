/*
 * cells.h
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#ifndef SRC_PLACEMENT_CELLS_H_
#define SRC_PLACEMENT_CELLS_H_


#include <netlist.h>
#include <property.h>
#include "../geometry/geometry.h"
#include <boost/bimap.hpp>

namespace openeda {
namespace placement {

class cells : public entity::property {
	std::vector<geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > m_geometries;
	std::vector<geometry::point<double> > m_positions;

	boost::bimap<entity::entity, std::size_t> m_mapping;
public:
	cells(openeda::netlist::netlist * netlist);
	virtual ~cells();
	void create(entity::entity e);
	void destroy(entity::entity e);
	void position(entity::entity e, geometry::point<double>);
	geometry::point<double> position(entity::entity e) const {
		return m_positions[m_mapping.left.at(e)];
	}
	void geometry(entity::entity e, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > geometry);
	geometry::multi_polygon<geometry::polygon<geometry::point<double> > > geometry(entity::entity e) const {
		return m_geometries[m_mapping.left.at(e)];
	}

	const std::vector<geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > & geometries() const {
		return m_geometries;
	}

};

} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_CELLS_H_ */
