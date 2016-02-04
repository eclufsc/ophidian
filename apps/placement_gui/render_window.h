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

	std::unordered_map<openeda::entity::entity, std::pair<std::size_t, std::size_t> > m_entity2index;

	void update_selected_vertex_array(
			const std::set<openeda::entity::entity> &selected);
	void update_cells_vertex_array(openeda::entity::entity cell);

public:
	render_window(application * app);
	virtual ~render_window();

	void create_vertex_array(const openeda::placement::cells& placement_cells);
	void render();

	void process_inputs();

};

#endif /* RENDER_WINDOW_H_ */
