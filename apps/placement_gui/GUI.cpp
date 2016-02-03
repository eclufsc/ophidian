/*
 * application.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: csguth
 */

#include "GUI.h"

GUI::GUI(std::string lef_file, std::string v_file, std::string def_file) :
		m_application(lef_file, v_file, def_file),
		m_window(&m_application){
}

GUI::~GUI() {
}

void GUI::run() {

	sf::Clock clock;
	clock.restart();

	while (m_window.isOpen()) {
		m_window.process_inputs();
		m_window.render();
	}
}

