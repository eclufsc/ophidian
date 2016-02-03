/*
 * application.h
 *
 *  Created on: Feb 3, 2016
 *      Author: csguth
 */

#ifndef APPS_PLACEMENT_GUI_APPLICATION_H_
#define APPS_PLACEMENT_GUI_APPLICATION_H_

#include "../placement/library.h"
#include "../placement/cells.h"

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <utility>

using point = openeda::geometry::point<double>;
using polygon = openeda::geometry::polygon<point>;
using multi_polygon = openeda::geometry::multi_polygon<polygon>;

using box = typename boost::geometry::model::box<openeda::geometry::point<double> >;
typedef std::pair<box, openeda::entity::entity> rtree_node;
class rtree_node_comparator {
public:
	bool operator()(const rtree_node & node1, const rtree_node & node2) const {
		return node1.second == node2.second;
	}
};

typedef boost::geometry::index::rtree<rtree_node,
		boost::geometry::index::rstar<16>,
		boost::geometry::index::indexable<rtree_node>, rtree_node_comparator> rtree;

class application {

	openeda::standard_cell::standard_cells m_std_cells;
	openeda::netlist::netlist m_netlist;
	openeda::placement::library m_placement_library;
	openeda::placement::cells m_placement_cells;

	rtree m_position2cellentity;

public:

	application(std::string lef_file, std::string v_file, std::string def_file);
	virtual ~application();

	void click(point position);

	const openeda::placement::cells& placement_cells() const {
		return m_placement_cells;
	}

};

#endif /* APPS_PLACEMENT_GUI_APPLICATION_H_ */
