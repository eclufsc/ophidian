/*
 * application.h
 *
 *  Created on: Feb 3, 2016
 *      Author: csguth
 */

#ifndef APPS_PLACEMENT_GUI_GUI_H_
#define APPS_PLACEMENT_GUI_GUI_H_


#include "render_window.h"
#include "application.h"

class GUI {

	application m_application;
	render_window m_window;
public:
	GUI(std::string lef_file, std::string v_file, std::string def_file);
	virtual ~GUI();

	void run();
};

#endif /* APPS_PLACEMENT_GUI_GUI_H_ */
