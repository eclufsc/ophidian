/*
 * def.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: csguth
 */

#include <def.h>
#include <iostream>

namespace openeda {
namespace placement {
namespace def {
namespace parser {
const char* DEFCommentChar = "#";
const char* DEFLineEndingChar = ";";

void get_next_token(std::istream &is, std::string &token,
		const char* beginComment) {
	do {
		is >> token;
		if (!strcmp(token.substr(0, strlen(beginComment)).c_str(),
				beginComment)) {
			getline(is, token);
			token = "";
		}
	} while (!is.eof() && (token.empty() || isspace(token[0])));
}

void get_next_n_tokens(std::istream &is, std::vector<std::string> &tokens,
		const unsigned numTokens, const char* beginComment) {
	tokens.clear();
	std::string token;
	unsigned count = 0;
	do {
		is >> token;
		if (!strcmp(token.substr(0, strlen(beginComment)).c_str(),
				beginComment)) {
			getline(is, token);
			token = "";
		}
		if (!token.empty() && !isspace(token[0])) {
			tokens.push_back(token);
			++count;
		}
	} while (!is.eof() && count < numTokens);
}

void read_init_def_components(std::istream &is, netlist::netlist* netlist,
		placement* cells) {
	//	cell* myCell = NULL;
	std::vector<std::string> tokens(1);

	parser::get_next_n_tokens(is, tokens, 2, parser::DEFCommentChar);
	assert(tokens[1] == DEFLineEndingChar);

	unsigned countComponents = 0;
	unsigned numComponents = atoi(tokens[0].c_str());

	entity::entity cell;

	// can do with while(1)
	while (countComponents <= numComponents) {
		//		std::cout << countComponents-1 << " components read" << std::endl;
		get_next_token(is, tokens[0], parser::DEFCommentChar);
		if (tokens[0] == "-") {
			++countComponents;
			parser::get_next_n_tokens(is, tokens, 2, parser::DEFCommentChar);
			//assert(cell2id.find(tokens[0]) != cell2id.end());
			cell = netlist->cell_insert(tokens[0], tokens[1]);
			//			cell = netlist->cell_find(tokens[0]);

			//			if (cell2id.find(tokens[0]) == cell2id.end()) {
			//				myCell = locateOrCreateCell(tokens[0]);
			//				myCell->type = macro2id[tokens[1]];
			//				macro* myMacro = &macros[macro2id[tokens[1]]];
			//				myCell->width = myMacro->width
			//						* static_cast<double>(LEFdist2Microns);
			//				myCell->height = myMacro->height
			//						* static_cast<double>(LEFdist2Microns);
			//			} else
			//				myCell = locateOrCreateCell(tokens[0]);
		} else if (tokens[0] == "+") {
			//			assert(myCell != NULL);
			get_next_token(is, tokens[0], DEFCommentChar);

			if (tokens[0] == "PLACED" || tokens[0] == "FIXED") {
				bool fixed = tokens[0] == "FIXED";
				//				myCell->isFixed = (tokens[0] == "FIXED");
				get_next_n_tokens(is, tokens, 5, DEFCommentChar);
				assert(tokens[0] == "(");
				assert(tokens[3] == ")");
				cells->cell_position(cell,
						{ std::stod(tokens[1]), std::stod(tokens[2]) });
				cells->cell_fixed(cell, fixed);

				//				myCell->init_x_coord = atoi(tokens[1].c_str());
				//				myCell->init_y_coord = atoi(tokens[2].c_str());
				//				myCell->cellorient = tokens[4];
				// NOTE: this contest does not allow flipping/rotation
				//				assert(myCell->cellorient == "N");
			}
		} else if (!strcmp(tokens[0].c_str(), DEFLineEndingChar)) {
			//			myCell = NULL;
		} else if (tokens[0] == "END") {
			get_next_token(is, tokens[0], DEFCommentChar);
			assert(tokens[0] == "COMPONENTS");
			break;
		}
	}
}

void read_def_pins(std::istream& is, netlist::netlist* netlist,
		placement* cells) {

	//	pin* myPin = NULL;
	std::vector<std::string> tokens(1);

	get_next_n_tokens(is, tokens, 2, DEFCommentChar);
	assert(tokens[1] == DEFLineEndingChar);

	unsigned countPins = 0;
	unsigned numPins = atoi(tokens[0].c_str());

	std::string pin_name;
	entity::entity current_pin;
	while (countPins <= numPins) {
		get_next_token(is, tokens[0], DEFCommentChar);
		if (tokens[0] == "-") {
			++countPins;
			get_next_token(is, tokens[0], DEFCommentChar);

			pin_name = tokens[0];

			//			assert(pin2id.find(tokens[0]) != pin2id.end());
			//			myPin = locateOrCreatePin(tokens[0]);
			// NOTE: pins in .def are only for PI/POs that are fixed/placed
			//			assert(myPin->type == PI_PIN || myPin->type == PO_PIN);
		} else if (tokens[0] == "+") {
			//			assert(myPin != NULL);
			get_next_token(is, tokens[0], DEFCommentChar);

			// NOTE: currently, we just store NET, DIRECTION, LAYER, FIXED/PLACED
			if (tokens[0] == "NET") {
				get_next_token(is, tokens[0], DEFCommentChar);
				//				assert(net2id.find(tokens[0]) != net2id.end());
			} else if (tokens[0] == "DIRECTION") {
				//				assert(myPin != NULL);
				get_next_token(is, tokens[0], DEFCommentChar);
				if (tokens[0] == "INPUT")
					current_pin = netlist->PI_insert(pin_name);
				else if (tokens[0] == "OUTPUT")
					current_pin = netlist->PO_insert(pin_name);

				//				if (myPin->type == PI_PIN)
				//					assert(tokens[0] == "INPUT");
				//				else if (myPin->type == PO_PIN)
				//					assert(tokens[0] == "OUTPUT");
			} else if (tokens[0] == "FIXED" || tokens[0] == "PLACED") {
				//				assert(myPin != NULL);
				//				myPin->isFixed = (tokens[0] == "FIXED");
				get_next_n_tokens(is, tokens, 5, DEFCommentChar);
				assert(tokens[0] == "(");
				assert(tokens[3] == ")");

				cells->pad_position(current_pin, { std::stod(tokens[1]),
						std::stod(tokens[2]) });
				//				myPin->x_coord = atof(tokens[1].c_str());
				//				myPin->y_coord = atof(tokens[2].c_str());
				// NOTE: this contest does not allow flipping/rotation
				assert(tokens[4] == "N");
			} else if (tokens[0] == "LAYER") {
				//				assert(myPin != NULL);
				get_next_token(is, tokens[0], DEFCommentChar);
				// NOTE: we assume the layer is previously defined from .lef
				// we don't save layer for a pin instance.
				//				assert(layer2id.find(tokens[0]) != layer2id.end());
				get_next_n_tokens(is, tokens, 8, DEFCommentChar);
				assert(tokens[0] == "(");
				assert(tokens[3] == ")");
				assert(tokens[4] == "(");
				assert(tokens[7] == ")");

				auto current_position = cells->pin_position(current_pin);
				current_position.x(
						current_position.x()
								+ 0.5
										* (std::stod(tokens[1])
												+ std::stod(tokens[5])));
				current_position.y(
						current_position.y()
								+ 0.5
										* (std::stod(tokens[2])
												+ std::stod(tokens[6])));
				cells->pad_position(current_pin, current_position);
				//				myPin->x_coord += 0.5
				//						* (atof(tokens[1].c_str()) + atof(tokens[5].c_str()));
				//				myPin->y_coord += 0.5
				//						* (atof(tokens[2].c_str()) + atof(tokens[6].c_str()));
			} else if (!strcmp(tokens[0].c_str(), DEFLineEndingChar)) {
				current_pin = entity::entity { };
				//				myPin = NULL;
			}
		} else if (tokens[0] == "END") {
			get_next_token(is, tokens[0], DEFCommentChar);
			assert(tokens[0] == "PINS");
			break;
		}
	}

}

void read_def_nets(std::istream& is) {
	//	net* myNet = NULL;
	//	pin* myPin = NULL;
	std::vector<std::string> tokens(1);

	get_next_n_tokens(is, tokens, 2, DEFCommentChar);
	assert(tokens[1] == DEFLineEndingChar);

	unsigned countNets = 0;
	unsigned numNets = atoi(tokens[0].c_str());

	while (countNets <= numNets) {
		get_next_token(is, tokens[0], DEFCommentChar);
		if (tokens[0] == "-") {
			get_next_token(is, tokens[0], DEFCommentChar);

			// all nets should be already found in .verilog
			//			assert(net2id.find(tokens[0]) != net2id.end());
			//			myNet = locateOrCreateNet(tokens[0]);
			//			unsigned myNetId = net2id.find(myNet->name)->second;

			// first is always source, rest are sinks
			get_next_n_tokens(is, tokens, 4, DEFCommentChar);
			assert(tokens[0] == "(");
			assert(tokens[3] == ")");
			// ( PIN PI/PO ) or ( cell_instance internal_pin )
			std::string pinName =
					tokens[1] == "PIN" ?
							tokens[2] : tokens[1] + ":" + tokens[2];
			//			assert(pin2id.find(pinName) != pin2id.end());
			//			myPin = locateOrCreatePin(pinName);
			//			assert(myPin->net == myNetId);

			do {
				get_next_token(is, tokens[0], DEFCommentChar);
				if (tokens[0] == DEFLineEndingChar)
					break;
				assert(tokens[0] == "(");
				get_next_n_tokens(is, tokens, 3, DEFCommentChar);
				assert(tokens.size() == 3);
				assert(tokens[2] == ")");
				pinName =
						tokens[0] == "PIN" ?
								tokens[1] : tokens[0] + ":" + tokens[1];
				//				assert(pin2id.find(pinName) != pin2id.end());
				//				myPin = locateOrCreatePin(pinName);
				//				myPin->net = myNetId;
				//				assert(myPin->net == myNetId);
			} while (tokens[2] == ")");
		} else if (!strcmp(tokens[0].c_str(), DEFLineEndingChar)) {
			//			myNet = NULL;
			//			myPin = NULL;
		} else if (tokens[0] == "END") {
			get_next_token(is, tokens[0], DEFCommentChar);
			assert(tokens[0] == "NETS");
			break;
		}
	}
}

}

void read(std::istream& dot_def, netlist::netlist* netlist, placement * cells) {

	std::cout << "reading .def file..." << std::endl;
	//	if (mode == INIT)
	//		cout << "  .def file       : " << input << endl;
	//	else
	//		cout << "  final .def file : " << input << endl;
	//
	//	ifstream dot_def(input.c_str());
	//	if (!dot_def.good()) {
	//		cerr << "read_def:: cannot open `" << input << "' for reading." << endl;
	//		exit(1);
	//	}

	double DEFdist2Microns = 0.0;

	std::vector<std::string> tokens(1);
	while (!dot_def.eof()) {
		parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);

		if (tokens[0] == parser::DEFLineEndingChar)
			continue;

		if (tokens[0] == "VERSION") {
			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
			//			DEFVersion = tokens[0];
			//#ifdef DEBUG
			//			cout << "def version: " << DEFVersion << endl;
			//#endif
		} else if (tokens[0] == "DIVIDERCHAR") {
			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
			unsigned index1 = tokens[0].find_first_of("\"");
			unsigned index2 = tokens[0].find_last_of("\"");
			assert(index1 != std::string::npos);
			assert(index2 != std::string::npos);
			assert(index2 > index1);
			//			DEFDelimiter = tokens[0].substr(index1 + 1, index2 - index1 - 1);
			//#ifdef DEBUG
			//			cout << "divide character: " << DEFDelimiter << endl;
			//#endif
		} else if (tokens[0] == "BUSBITCHARS") {
			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
			unsigned index1 = tokens[0].find_first_of("\"");
			unsigned index2 = tokens[0].find_last_of("\"");
			assert(index1 != std::string::npos);
			assert(index2 != std::string::npos);
			assert(index2 > index1);
			//			DEFBusCharacters = tokens[0].substr(index1 + 1,
			//					index2 - index1 - 1);
			//#ifdef DEBUG
			//			cout << "bus bit characters: " << DEFBusCharacters << endl;
			//#endif
		} else if (tokens[0] == "DESIGN") {
			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
			//			design_name = tokens[0];
			//#ifdef DEBUG
			//			cout << "design name: " << design_name << endl;
			//#endif
		} else if (tokens[0] == "UNITS") {
			parser::get_next_n_tokens(dot_def, tokens, 3,
					parser::DEFCommentChar);
			assert(tokens.size() == 3);
			assert(tokens[0] == "DISTANCE");
			assert(tokens[1] == "MICRONS");
			DEFdist2Microns = atoi(tokens[2].c_str());
			//			assert(DEFdist2Microns <= lib->dist2microns());
			//#ifdef DEBUG
			//			cout << "unit distance to microns: " << DEFdist2Microns << endl;
			//#endif
		} else if (tokens[0] == "DIEAREA"/* && mode == INIT*/) {
			parser::get_next_n_tokens(dot_def, tokens, 8,
					parser::DEFCommentChar);
			assert(tokens.size() == 8);
			assert(tokens[0] == "(" && tokens[3] == ")");
			assert(tokens[4] == "(" && tokens[7] == ")");
			//			lx = atof(tokens[1].c_str());
			//			by = atof(tokens[2].c_str());
			//			rx = atof(tokens[5].c_str());
			//			ty = atof(tokens[6].c_str());
		} else if (tokens[0] == "ROW"/* && mode == INIT*/) {
			parser::get_next_n_tokens(dot_def, tokens, 5,
					parser::DEFCommentChar);
			//			row* myRow = locateOrCreateRow(tokens[0]);
			//			myRow->name = tokens[0];
			//			myRow->site = site2id[tokens[1]];
			//			myRow->origX = atoi(tokens[2].c_str());
			//			myRow->origY = atoi(tokens[3].c_str());
			//			myRow->siteorient = tokens[4];
			//			// NOTE: this contest does not allow flipping/rotation
			//			assert(myRow->siteorient == "N");

			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
			if (tokens[0] == "DO") {
				parser::get_next_n_tokens(dot_def, tokens, 3,
						parser::DEFCommentChar);
				assert(tokens[1] == "BY");
				//				myRow->numSites = max(atoi(tokens[0].c_str()),
				//						atoi(tokens[2].c_str()));
				// NOTE: currenlty we only handle horizontal row sites
				assert(tokens[2] == "1");
				parser::get_next_token(dot_def, tokens[0],
						parser::DEFCommentChar);
				if (tokens[0] == "STEP") {
					parser::get_next_n_tokens(dot_def, tokens, 2,
							parser::DEFCommentChar);
					//					myRow->stepX = atoi(tokens[0].c_str());
					//					myRow->stepY = atoi(tokens[1].c_str());
					//					// NOTE: currenlty we only handle horizontal row sites & spacing = 0
					//					assert(
					//							myRow->stepX
					//									== sites[myRow->site].width
					//											* LEFdist2Microns);
					//					assert(myRow->stepY == 0);
				}
			}
			// else we do not currently store properties
		} else if (tokens[0] == "COMPONENTS") {
			//			if (mode == INIT)
			parser::read_init_def_components(dot_def, netlist, cells);
			//			else if (mode == FINAL)
			//				read_final_def_components(dot_def);
		} else if (tokens[0] == "PINS"/* && mode == INIT*/) {
			parser::read_def_pins(dot_def, netlist, cells);
		} else if (tokens[0] == "NETS"/* && mode == INIT*/) {
			parser::read_def_nets(dot_def);
		} else if (tokens[0] == "PROPERTYDEFINITIONS") {
			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
			while (tokens[0] != "PROPERTYDEFINITIONS")
				parser::get_next_token(dot_def, tokens[0],
						parser::DEFCommentChar);
		} else if (tokens[0] == "SPECIALNETS") {
			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
			while (tokens[0] != "SPECIALNETS")
				parser::get_next_token(dot_def, tokens[0],
						parser::DEFCommentChar);
		} else if (tokens[0] == "END") {
			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
			assert(tokens[0] == "DESIGN");
			break;
		} else {
			parser::get_next_token(dot_def, tokens[0], parser::DEFCommentChar);
		}
	}
	//	dot_def.close();
	std::cout << "reading .def file DONE" << std::endl;
}

}
} /* namespace placement */
} /* namespace openeda */

