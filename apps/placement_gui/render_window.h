/*
 * render_window.h
 *
 *  Created on: Feb 3, 2016
 *      Author: csguth
 */

#ifndef RENDER_WINDOW_H_
#define RENDER_WINDOW_H_

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "application.h"

class render_window: public sf::RenderWindow {

	application * m_app;

	sf::VertexArray m_cells;
	sf::VertexArray m_selected;
	sf::View m_view;

	std::unordered_map<openeda::entity::entity,
			std::pair<std::size_t, std::size_t> > m_entity2index;

	void update_selected_vertex_array(
			const std::set<openeda::entity::entity> &selected);
	void update_cells_vertex_array(openeda::entity::entity cell);
	void create_vertex_array();

public:
	class state {
	public:
		virtual ~state() {}
		virtual state* handle_input(sf::Event& event, const render_window* window) = 0;
		virtual void update(render_window* window) = 0;
	};
private:
	state * m_state;

public:
	render_window(application * app);
	virtual ~render_window();

	void render();

	void process_inputs();

	bool white_space(sf::Vector2f coord) const;

};

class idle_state: public render_window::state {
public:
	render_window::state* handle_input(sf::Event& event, const  render_window* window);
	void update(render_window* window);
};

class selected_state: public render_window::state {
	sf::Vector2f m_position;
public:
	selected_state(sf::Vector2f position);
	render_window::state* handle_input(sf::Event& event, const render_window* window);
	void update(render_window* window);
};

class dragging_state: public render_window::state {
public:
	render_window::state* handle_input(sf::Event& event, const render_window* window);
	void update(render_window* window);
};

#endif /* RENDER_WINDOW_H_ */
