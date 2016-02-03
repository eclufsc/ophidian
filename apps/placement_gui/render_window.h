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

	std::vector<GLdouble> m_vertex_array;
	std::vector<GLdouble> m_color_array;
	sf::View m_view;
	std::size_t m_lines_to_draw;


public:
	render_window(application * app);
	virtual ~render_window();

	void create_vertex_array(const openeda::placement::cells& placement_cells);
	void render();

	void process_inputs();
};

#endif /* RENDER_WINDOW_H_ */
