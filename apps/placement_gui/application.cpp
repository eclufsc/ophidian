/*
 * application.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: csguth
 */

#include "application.h"

#include <fstream>
#include <lef.h>
#include <def.h>
#include <verilog.h>

#include <boost/geometry.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

namespace bg = boost::geometry;
namespace trans = bg::strategy::transform;

application::application(std::string lef_file, std::string v_file,
		std::string def_file) :
		m_netlist(&m_std_cells), m_placement_library(&m_std_cells), m_placement_cells(
				&m_netlist) {
	std::ifstream dot_lef(lef_file.c_str(), std::ifstream::in);
	std::ifstream dot_v(v_file.c_str(), std::ifstream::in);
	std::ifstream dot_def(def_file.c_str(), std::ifstream::in);
	if (!dot_def.good() || !dot_v.good() || !dot_def.good())
		throw std::runtime_error("Error reading the files");

	openeda::netlist::verilog::read(dot_v, &m_netlist);
	dot_v.close();
	openeda::placement::lef::read(dot_lef, &m_std_cells, &m_placement_library);
	dot_lef.close();
	openeda::placement::def::read(dot_def, &m_netlist, &m_placement_library,
			&m_placement_cells);
	dot_lef.close();

	for (auto c : m_netlist.cell_system()) {
		auto geometry = m_placement_library.geometry(
				m_netlist.cell_std_cell(c.second));
		multi_polygon geo2;
		auto position = m_placement_cells.position(c.second);
		trans::translate_transformer<double, 2, 2> translate(position.x(),
				position.y());
		bg::transform(geometry, geo2, translate);
		bg::correct(geo2);

		for (auto & poly : geo2) {
			point ll { std::numeric_limits<double>::max(), std::numeric_limits<
					double>::max() };
			point ur { std::numeric_limits<double>::min(), std::numeric_limits<
					double>::min() };

			for (auto & p : poly.outer()) {
				ll.x(std::min(ll.x(), p.x()));
				ll.y(std::min(ll.y(), p.y()));
				ur.x(std::max(ur.x(), p.x()));
				ur.y(std::max(ur.y(), p.y()));
			}

			box cellbox(ll, ur);
			rtree_node cell_node(cellbox, c.second);
			m_position2cellentity.insert(cell_node);
		}
		m_placement_cells.geometry(c.second, geo2);
	}
}

application::~application() {
}

void application::click(point position) {

	std::vector<rtree_node> result;
	m_position2cellentity.query(boost::geometry::index::contains(position),
			std::back_inserter(result));

	for (auto & r : result) {
		std::cout << position.x() << ", " << position.y() << " maps to cell "
						<< m_netlist.cell_name(r.second) << std::endl;

		auto geo = m_placement_cells.geometry(r.second);
		std::cout << boost::geometry::wkt(geo) << std::endl;
	}





}
