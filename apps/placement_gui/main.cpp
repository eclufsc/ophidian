/*
 * main.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: csguth
 */

#include <iostream>

#include "GUI.h"

int main(int argc, char **argv) {

	if (argc != 4) {
		std::cerr << "Usage: " << argv[0]
				<< " <.lef file> <.v file> <.def file>" << std::endl;
		return -1;
	}



	GUI app(argv[1], argv[2], argv[3]);
	app.run();
	return 0;
}

