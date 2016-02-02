/*
 * main.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: csguth
 */

#include <iostream>
#include <string>

#include <fstream>
#include "netlist.h"
#include "verilog.h"

#include "library.h"
#include "lef.h"
#include "def.h"
#include "../placement/cells.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

using point = openeda::geometry::point<double>;
using polygon = openeda::geometry::polygon<point>;
using multi_polygon = openeda::geometry::multi_polygon<polygon>;

namespace bg = boost::geometry;
namespace trans = bg::strategy::transform;

int main(int argc, char **argv) {

	if (argc != 4) {
		std::cerr << "Usage: " << argv[0]
				<< " <.lef file> <.v file> <.def file>" << std::endl;
		return -1;
	}

	std::ifstream dot_lef(argv[1], std::ifstream::in);
	std::ifstream dot_v(argv[2], std::ifstream::in);
	std::ifstream dot_def(argv[3], std::ifstream::in);

	if (!dot_def.good() || !dot_v.good() || !dot_def.good())
		std::cerr << "Error reading the files" << std::endl;

	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::netlist::verilog::read(dot_v, &netlist);
	dot_v.close();
	openeda::placement::library placement_library(&std_cells);
	openeda::placement::lef::read(dot_lef, &std_cells, &placement_library);
	dot_lef.close();
	openeda::placement::cells placement_cells(&netlist);
	openeda::placement::def::read(dot_def, &netlist, &placement_library,
			&placement_cells);
	dot_lef.close();

	for (auto c : netlist.cell_system()) {
		auto geometry = placement_library.geometry(
				netlist.cell_std_cell(c.second));
		multi_polygon geo2;
		auto position = placement_cells.position(c.second);
		trans::translate_transformer<double, 2, 2> translate(position.x(),
				position.y());
		bg::transform(geometry, geo2, translate);
		placement_cells.geometry(c.second, geo2);
	}

	/* Code adapted from the SFML 2 "Window" example */

	sf::RenderWindow window(sf::VideoMode(853.3344, 480), "OpenGL",
			sf::Style::Default, sf::ContextSettings(32));

	sf::View view1(sf::FloatRect(0, 0, 60800, 34200));
	view1.setViewport(sf::FloatRect(0.05, 0.05, 0.95, 0.95));
	view1.setCenter(sf::Vector2f(7214680.000000, 1402200.000000));

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized) {
				glViewport(0, 0, event.size.width, event.size.height);
			} else if (event.type == sf::Event::KeyPressed) {
				std::cout << "key pressed: " << event.key.code << std::endl;
				switch (event.key.code) {
				case sf::Keyboard::Key::W:
					view1.move(sf::Vector2f(0, -3420));
					break;
				case sf::Keyboard::Key::D:
					view1.move(sf::Vector2f(3420, 0));
					break;
				case sf::Keyboard::Key::S:
					view1.move(sf::Vector2f(0, +3420));
					break;
				case sf::Keyboard::Key::A:
					view1.move(sf::Vector2f(-3420, 0));
					break;
				}
			}
		}

		window.setView(view1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::size_t i = 0;
		for (auto geometry : placement_cells.geometries()) {
			for (auto polygon : geometry) {
				auto point = polygon.outer().front();
				auto lx = view1.getCenter().x - view1.getSize().x / 2;
				auto rx = view1.getCenter().x + view1.getSize().x / 2;
				auto by = view1.getCenter().y - view1.getSize().y / 2;
				auto ty = view1.getCenter().y + view1.getSize().y / 2;

				if(!(point.x() < rx && point.x() > lx && point.y() > by && point.y() < ty))
					continue;

				sf::VertexArray array(sf::LinesStrip, polygon.outer().size());
				sf::Transform transform;
				for (std::size_t i = 0; i < polygon.outer().size(); ++i) {
					array[i].position = sf::Vector2f(polygon.outer()[i].x(),
							polygon.outer()[i].y());
					array[i].color = sf::Color::Blue;
				}
				window.draw(array);
			}
			i++;
//			if (i == 100)
//				break;
		}

		window.display();

	}

	return 0;
}

