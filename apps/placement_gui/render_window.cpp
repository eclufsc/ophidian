/*
 * render_window.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: csguth
 */

#include "render_window.h"
#include <iostream>

#include <random>
#include <utility>

render_window::render_window(application * app) :
		m_app(app), sf::RenderWindow(sf::VideoMode(853.3344, 480), "OpenGL",
				sf::Style::Default, sf::ContextSettings(0, 0, 0)), m_view(
				sf::FloatRect(0, 0, 60800, 34200)) {
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

	std::default_random_engine engine;
	std::uniform_int_distribution<uint8_t> distribution { 0, 255 };

	for (auto e : m_app->netlist().cell_system()) {
		auto geometry = m_app->placement_cells().geometry(e.second);
		m_entity2index[e.second] = std::make_pair(m_cells.getVertexCount(), 0);
		std::size_t vertex_count = 0;
		uint8_t blue = distribution(engine);
		bool macro = geometry.size() > 1;
		for (auto & polygon : geometry) {
			for (std::size_t i = 0; i < 4; ++i) {
				sf::Vertex vertex;
				vertex.position = sf::Vector2f {
						static_cast<float>(polygon.outer()[i].x()),
						static_cast<float>(-polygon.outer()[i].y()) };
				vertex.color = sf::Color { 0, 0, blue };
				if (macro)
					vertex.color = sf::Color { 100, 100, 100 };
				m_cells.append(vertex);
				++vertex_count;
			}
		}
		m_entity2index[e.second].second = vertex_count;
	}

}

void render_window::render() {
	clear(sf::Color::White);
	setView(m_view);
	draw(&m_cells[0], m_cells.getVertexCount(), sf::PrimitiveType::Quads);
	draw(&m_selected[0], m_selected.getVertexCount(),
			sf::PrimitiveType::LinesStrip);
	sf::VertexArray axis { sf::LinesStrip, 3 };
	axis[0].position = sf::Vector2f { 0.0, -34200.0 };
	axis[0].color = sf::Color::Green;
	axis[1].position = sf::Vector2f { 0.0, 0.0 };
	axis[1].color = sf::Color { 122, 122, 0 };
	axis[2].position = sf::Vector2f { 34200.0, 0.0 };
	axis[2].color = sf::Color::Red;
	draw(axis);
	display();
}

void render_window::update_selected_vertex_array(
		const std::set<openeda::entity::entity>& selected) {
	std::cout << "there are " << selected.size() << " cells selected "
			<< std::endl;
	m_selected.clear();
	for (auto e : selected) {
		auto geometry = m_app->placement_cells().geometry(e);
		auto & front = geometry.front();
		m_selected.resize(front.outer().size());
		for (std::size_t i = 0; i < m_selected.getVertexCount(); ++i) {
			m_selected[i].position.x = front.outer()[i].x();
			m_selected[i].position.y = -front.outer()[i].y();
			m_selected[i].color = sf::Color::Magenta;
		}
	}

}

void render_window::update_cells_vertex_array(openeda::entity::entity cell) {

	std::size_t current_index = m_entity2index[cell].first;
	auto geometry = m_app->placement_cells().geometry(cell);
	for (auto & polygon : geometry) {
		for (std::size_t i = 0; i < 4; ++i) {
			sf::Vertex & vertex = m_cells[current_index++];
			vertex.position = sf::Vector2f {
					static_cast<float>(polygon.outer()[i].x()),
					static_cast<float>(-polygon.outer()[i].y()) };
		}
	}
	assert(
			current_index
					== m_entity2index[cell].first
							+ m_entity2index[cell].second);

}

void render_window::process_inputs() {
	sf::Event event;

	static sf::Vector2f offset;
	static bool moved;
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
			moved = false;
			sf::Vector2i coord(event.mouseButton.x, event.mouseButton.y);
			std::cout << "coord " << coord.x << ", " << coord.y << std::endl;

			auto coord2 = mapPixelToCoords(coord, m_view);
			coord2.y *= -1.0;
			std::cout << "view coord " << coord2.x << ", " << coord2.y
					<< std::endl;
			m_app->click( { coord2.x, coord2.y });

			std::set<openeda::entity::entity> selected =
					m_app->selected_cells();

			update_selected_vertex_array(selected);

			if (!selected.empty()) {
				auto cell_position = m_app->placement_cells().position(
						*selected.begin());
				offset.x = coord2.x - cell_position.x();
				offset.y = coord2.y - cell_position.y();
			}

		} else if (event.type == sf::Event::MouseMoved) {

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				moved = true;
				auto selected = m_app->selected_cells();
				for (auto c : selected) {
					std::cout << "moving " << m_app->netlist().cell_name(c)
							<< " to " << event.mouseMove.x << ", "
							<< event.mouseMove.y << std::endl;

					sf::Vector2f final_position = mapPixelToCoords(
							{ event.mouseMove.x, event.mouseMove.y }, m_view);

					final_position.y *= -1.f;

					m_app->place_cell_and_update_index(c,
							{ final_position.x - offset.x, final_position.y
									- offset.y });
					update_cells_vertex_array(c);
				}
			} else
				moved = false;
		} else if (event.type == sf::Event::MouseButtonReleased) {
			sf::Vector2i coord(event.mouseButton.x, event.mouseButton.y);
			std::cout << "coord " << coord.x << ", " << coord.y << std::endl;

			auto coord2 = mapPixelToCoords(coord, m_view);
			coord2.y *= -1.0;
			std::cout << "view coord " << coord2.x << ", " << coord2.y
					<< std::endl;

			if (moved)
				m_app->release_click( { coord2.x, coord2.y });
			std::set<openeda::entity::entity> selected =
					m_app->selected_cells();

			update_selected_vertex_array(selected);
		}
	}
}
