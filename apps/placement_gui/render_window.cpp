/*
 * render_window.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: csguth
 */

#include "render_window.h"
#include <iostream>

#include <random>

render_window::render_window(application * app) :
		m_app(app), sf::RenderWindow(sf::VideoMode(853.3344, 480), "OpenGL",
				sf::Style::Default, sf::ContextSettings(0, 0, 0)), m_view(
				sf::FloatRect(0, 0, 60800, 34200)), m_lines_to_draw(0) {
	m_view.setViewport(sf::FloatRect(0, 0, 1.0, 1.0));
	m_view.setCenter(sf::Vector2f(0, 0));
	create_vertex_array(app->placement_cells());
}

render_window::~render_window() {
}

void render_window::create_vertex_array(
		const openeda::placement::cells& placement_cells) {

	static bool called;

	if (called)
		return;

	called = true;

	m_lines_to_draw = 0;
	m_vertex_array.reserve(4 * placement_cells.geometries().size());
	m_color_array.reserve(3 * placement_cells.geometries().size());

	std::ran

	for (auto & g : placement_cells.geometries()) {
		for (auto & poly : g) {
			for(std::size_t i = 0; i < 4; ++i)
			{
				m_vertex_array.push_back(poly.outer()[i].x());
				m_vertex_array.push_back(poly.outer()[i].y());
			}
		}
	}
	m_vertex_array.shrink_to_fit();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY)
	glVertexPointer(2, GL_DOUBLE, 0, m_vertex_array.data());
	glVertexPointer(3, GL_DOUBLE, 0, m_color_array.data());


}

void render_window::render() {

	setView(m_view);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_view.getTransform().getMatrix());
	glScalef(1.0, -1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);

	glDrawArrays(GL_QUADS, 0, m_vertex_array.size());

	pushGLStates();

	sf::VertexArray eixoX(sf::Lines, 2);
	eixoX[0].position = {0.0, 0.0};
	eixoX[1].position = {0.0, -1.0};
	sf::VertexArray eixoY(sf::Lines, 2);
	eixoY[0].position = {0.0, 0.0};
	eixoY[1].position = {1.0, 0.0};

	draw(eixoX);
	draw(eixoY);

	popGLStates();

	setView(getDefaultView());

	display();

}

void render_window::process_inputs() {
	sf::Event event;
	while (pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			close();
		else if (event.type == sf::Event::Resized) {
			glViewport(0, 0, event.size.width, event.size.height);
		} else if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
			case sf::Keyboard::Key::W:
			case sf::Keyboard::Key::Up:
				m_view.move(sf::Vector2f(0, -0.1 * m_view.getSize().y));
				break;
			case sf::Keyboard::Key::D:
			case sf::Keyboard::Key::Right:
				m_view.move(sf::Vector2f(0.1 * m_view.getSize().x, 0));
				break;
			case sf::Keyboard::Key::S:
			case sf::Keyboard::Key::Down:
				m_view.move(sf::Vector2f(0, 0.1 * m_view.getSize().y));
				break;
			case sf::Keyboard::Key::A:
			case sf::Keyboard::Key::Left:
				m_view.move(sf::Vector2f(-0.1 * m_view.getSize().x, 0));
				break;
			}
		} else if (event.type == sf::Event::MouseWheelMoved) {
			if (event.mouseWheel.delta > 0)
				m_view.zoom(1.0f / 1.1f);
			else if (event.mouseWheel.delta < 0)
				m_view.zoom(1.1f);
		} else if (event.type == sf::Event::MouseButtonPressed) {
			sf::Vector2i coord(event.mouseButton.x, event.mouseButton.y);
			std::cout << "coord " << coord.x << ", " << coord.y << std::endl;

			auto coord2 = mapPixelToCoords(coord, m_view);
			coord2.y *= -1.0;
			std::cout << "view coord " << coord2.x << ", " << coord2.y << std::endl;

			m_app->click({coord2.x, coord2.y});
		}
	}
}
