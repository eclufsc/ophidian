/*
 * main.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: csguth
 */

#include <iostream>
#include <string>

#include <fstream>
#include <string>
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

void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, float zoom,
		sf::View & view) {
	const sf::Vector2f beforeCoord { window.mapPixelToCoords(pixel) };
	view.zoom(zoom);
	const sf::Vector2f afterCoord { window.mapPixelToCoords(pixel) };
	const sf::Vector2f offsetCoords { beforeCoord - afterCoord };
	view.move(offsetCoords);
}

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
		trans::translate_transformer<double, 2, 2> translate(
				position.x() / placement_library.dist2microns(),
				position.y() / placement_library.dist2microns());
		bg::transform(geometry, geo2, translate);
		bg::correct(geo2);
		placement_cells.geometry(c.second, geo2);
	}

	/* Code adapted from the SFML 2 "Window" example */

	sf::RenderWindow window(sf::VideoMode(853.3344, 480), "OpenGL",
			sf::Style::Default, sf::ContextSettings(0, 0, 0));

	std::vector<GLdouble> vertex_array;
	vertex_array.reserve(4 * (placement_cells.geometries().size() - 1));

	std::size_t count = 0;
	for (auto & g : placement_cells.geometries()) {
//		std::cout << "GEOMETRY   " << std::endl;
		for (auto & poly : g) {
			for (std::size_t i = 1; i < poly.outer().size(); ++i) {
				vertex_array.push_back(poly.outer()[i - 1].x());
				vertex_array.push_back(poly.outer()[i - 1].y());
				vertex_array.push_back(poly.outer()[i].x());
				vertex_array.push_back(poly.outer()[i].y());
				++count;
			}
		}
	}
	vertex_array.shrink_to_fit();
	std::size_t max_index = std::distance(vertex_array.begin(),
			std::max_element(vertex_array.begin(), vertex_array.end()));
	std::size_t min_index = std::distance(vertex_array.begin(),
			std::min_element(vertex_array.begin(), vertex_array.end()));
	std::cout << "max index " << max_index << std::endl;
	std::cout << "value " << vertex_array[max_index] << std::endl;
	std::cout << "min index " << min_index << std::endl;
	std::cout << "value " << vertex_array[min_index] << std::endl;
	std::cout << count << " lines" << std::endl;

	sf::View view1(sf::FloatRect(0, 0, 5641.86, 3060.9));
	view1.setViewport(sf::FloatRect(0, 0, 1.0, 1.0));
	view1.setCenter(sf::Vector2f(0, 0));
	std::cout << "center: " << view1.getCenter().x << ", "
			<< view1.getCenter().y << std::endl;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_DOUBLE, 0, vertex_array.data());
	sf::Clock clock;
	clock.restart();

	int max;
	glGetIntegerv(GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV, &max);
	std::cout << "max " << max << " current " << vertex_array.size()
			<< std::endl;

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
					view1.move(sf::Vector2f(0, -0.1 * view1.getSize().y));
					break;
				case sf::Keyboard::Key::D:
					view1.move(sf::Vector2f(0.1 * view1.getSize().x, 0));
					break;
				case sf::Keyboard::Key::S:
					view1.move(sf::Vector2f(0, 0.1 * view1.getSize().y));
					break;
				case sf::Keyboard::Key::A:
					view1.move(sf::Vector2f(-0.1 * view1.getSize().x, 0));
					break;
				}
				std::cout << "center: " << view1.getCenter().x << ", "
						<< view1.getCenter().y << std::endl;
			} else if (event.type == sf::Event::MouseWheelMoved) {
				if (event.mouseWheel.delta > 0)
					zoomViewAt( { event.mouseWheel.x, event.mouseWheel.y },
							window, (1.f / 1.1f), view1);
				else
					zoomViewAt( { event.mouseWheel.x, event.mouseWheel.y },
							window, 1.1f, view1);
				auto position = sf::Mouse::getPosition(window);
				std::cout << "position: " << position.x << ", " << position.y
						<< std::endl;

			} else if (event.type == sf::Event::MouseButtonPressed) {
				auto position = sf::Mouse::getPosition(window);
				std::cout << "position: " << position.x << ", " << position.y
						<< std::endl;
			}
		}

		float elapsed = clock.getElapsedTime().asSeconds();
		clock.restart();
		float fps = 1.f / elapsed;
		std::cout << "FPS: " << fps << std::endl;
		window.setView(view1);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(view1.getTransform().getMatrix());
		glScalef(1.0, -1.0, 1.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glBegin(GL_LINES);
//		glColor3f(1.0, 0.0, 0.0);
//		glVertex2f(0.0, 0.0);
//		glVertex2f(0.0, 3420.0);
//		glColor3f(0.0, 1.0, 0.0);
//		glVertex2f(0.0, 0.0);
//		glVertex2f(3420.0, 0.0);
//		glEnd();

		glColor3f(1.0, 0.0, 0.0);
		glDrawArrays(GL_LINES, 0, count);

		window.pushGLStates();

		sf::VertexArray eixoX(sf::Lines, 2);
		eixoX[0].position = {0.0, 0.0};
		eixoX[1].position = {0.0, -1.0};
		sf::VertexArray eixoY(sf::Lines, 2);
		eixoY[0].position = {0.0, 0.0};
		eixoY[1].position = {1.0, 0.0};

		window.draw(eixoX);
		window.draw(eixoY);

		window.popGLStates();

		window.setView(window.getDefaultView());

		window.pushGLStates();

		sf::Text fps_txt;
		fps_txt.setString(sf::String(std::to_string(fps).c_str()));
		fps_txt.setPosition(0.0, 0.0);
		fps_txt.setCharacterSize(30);
		window.draw(fps_txt);

		window.popGLStates();

		window.display();

	}

	return 0;
}

