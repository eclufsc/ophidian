/*
 * lef.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#include <lef.h>
#include <string>
#include <cstring>
#include <cassert>
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/union.hpp>
#include <boost/geometry/io/wkt/wkt.hpp>
#include <iostream>
namespace openeda {
namespace placement {
namespace lef {
namespace parser {

const char* LEFCommentChar = "#";
const char* LEFLineEndingChar = ";";

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

void read_lef_site(std::istream &is) {
	std::vector<std::string> tokens(1);

	get_next_token(is, tokens[0], LEFCommentChar);
//  mySite = locateOrCreateSite(tokens[0]);

	get_next_token(is, tokens[0], LEFCommentChar);
	while (tokens[0] != "END") {
		if (tokens[0] == "SIZE") {
			get_next_n_tokens(is, tokens, 4, LEFCommentChar);
//      mySite->width  = atof(tokens[0].c_str());
//      mySite->height = atof(tokens[2].c_str());
		} else if (tokens[0] == "CLASS") {
			get_next_n_tokens(is, tokens, 2, LEFCommentChar);
			assert(tokens[1] == LEFLineEndingChar);
//      mySite->type = tokens[0];
		} else if (tokens[0] == "SYMMETRY") {
			// NOTE: this contest does not allow flipping/rotation
			// even though symmetries are specified for a site
			get_next_token(is, tokens[0], LEFCommentChar);
			while (tokens[0] != LEFLineEndingChar) {
//				mySite->symmetries.push_back(tokens[0]);
				get_next_token(is, tokens[0], LEFCommentChar);
			}
			assert(tokens[0] == LEFLineEndingChar);
		} else {
//      exit(1);
		}
		get_next_token(is, tokens[0], LEFCommentChar);
	}
	get_next_token(is, tokens[0], LEFCommentChar);
	assert(tokens[0] == "core");
}

// assumes the LAYER keyword has already been read in
void read_lef_layer(std::istream &is) {
//  layer* myLayer;
	std::vector<std::string> tokens(1);

	get_next_token(is, tokens[0], LEFCommentChar);
//  myLayer = locateOrCreateLayer(tokens[0]);

	get_next_token(is, tokens[0], LEFCommentChar);
	while (tokens[0] != "END") {
		if (tokens[0] == "TYPE") {
			get_next_n_tokens(is, tokens, 2, LEFCommentChar);
			assert(tokens[1] == LEFLineEndingChar);
//      myLayer->type = tokens[0];
		} else if (tokens[0] == "DIRECTION") {
			get_next_n_tokens(is, tokens, 2, LEFCommentChar);
			assert(tokens[1] == LEFLineEndingChar);
//      myLayer->direction = tokens[0];
		} else if (tokens[0] == "PITCH") {
			get_next_n_tokens(is, tokens, 2, LEFCommentChar);
//      myLayer->xPitch = atof(tokens[0].c_str());
			if (tokens[1] == LEFLineEndingChar) {
//        myLayer->yPitch = myLayer->xPitch;
			} else {
//        myLayer->yPitch = atof(tokens[1].c_str());
				get_next_token(is, tokens[0], LEFCommentChar);
				assert(tokens[0] == LEFLineEndingChar);
			}
		} else if (tokens[0] == "OFFSET") {
			get_next_n_tokens(is, tokens, 2, LEFCommentChar);
//      myLayer->xOffset = atof(tokens[0].c_str());
			if (tokens[1] == LEFLineEndingChar) {
//        myLayer->yOffset = myLayer->xOffset;
			} else {
//        myLayer->yOffset = atof(tokens[1].c_str());
				get_next_token(is, tokens[0], LEFCommentChar);
				assert(tokens[0] == LEFLineEndingChar);
			}
		} else if (tokens[0] == "WIDTH") {
			get_next_n_tokens(is, tokens, 2, LEFCommentChar);
			assert(tokens[1] == LEFLineEndingChar);
//      myLayer->width = atof(tokens[0].c_str());
		} else {
//      cerr << "read_lef_layer:: unsupported keyword " << tokens[0] << endl;
//      exit(4);
		}
		get_next_token(is, tokens[0], LEFCommentChar);
	}

	get_next_token(is, tokens[0], LEFCommentChar);
//  assert(myLayer->name == tokens[0]);
}

// assumes the keyword SITE has already been read in
void read_lef_macro_site(std::istream &is) {
	std::vector<std::string> tokens(1);
	get_next_token(is, tokens[0], LEFCommentChar);

//	mySite = locateOrCreateSite(tokens[0]);
//	myMacro->sites.push_back(site2id.find(mySite->name)->second);

	// does not support [sitePattern]
	unsigned numArgs = 0;
	do {
		get_next_token(is, tokens[0], LEFCommentChar);
		++numArgs;
	} while (tokens[0] != LEFLineEndingChar);

	if (numArgs > 1) {
//		cout << "read_lef_macro_site:: WARNING -- bypassing " << numArgs
//				<< " additional field in " << "MACRO " << myMacro->name
//				<< " SITE " << sites[site2id.find(mySite->name)->second].name
//				<< "." << endl;
//		cout
//				<< "It is likely that the fields are from [sitePattern], which are currently not expected."
//				<< endl;
	}
}

// assumes the keyword PIN has already been read in
void read_lef_macro_pin(std::istream &is) {
//	macro_pin myPin;
	std::vector<std::string> tokens(1);
	get_next_token(is, tokens[0], LEFCommentChar);
	std::string pinName = tokens[0];
	get_next_token(is, tokens[0], LEFCommentChar);
	while (tokens[0] != "END") {
		if (tokens[0] == "DIRECTION") {
			get_next_n_tokens(is, tokens, 2, LEFCommentChar);
			assert(tokens[1] == LEFLineEndingChar);
//			myPin.direction = tokens[0];
		} else if (tokens[0] == "PORT") {
			get_next_token(is, tokens[0], LEFCommentChar);
			while (tokens[0] != "END") {
				if (tokens[0] == "LAYER") {
					get_next_n_tokens(is, tokens, 2, LEFCommentChar);
					assert(tokens[1] == LEFLineEndingChar);
//					layer* myLayer = locateOrCreateLayer(tokens[0]);
//					myPin.layer = layer2id.find(myLayer->name)->second;
				} else if (tokens[0] == "RECT") {
					get_next_n_tokens(is, tokens, 2, LEFCommentChar);
//					myPin.xLL = min(atof(tokens[0].c_str()), myPin.xLL);
					if (tokens[1] == "ITERATE") {
						get_next_n_tokens(is, tokens, 3, LEFCommentChar);
//						myPin.yLL = min(atof(tokens[0].c_str()), myPin.yLL);
//						myPin.xUR = max(atof(tokens[1].c_str()), myPin.xUR);
//						myPin.yUR = max(atof(tokens[2].c_str()), myPin.yUR);
					} else {
//						myPin.yLL = min(atof(tokens[1].c_str()), myPin.yLL);
						get_next_n_tokens(is, tokens, 2, LEFCommentChar);
//						myPin.xUR = max(atof(tokens[0].c_str()), myPin.xUR);
//						myPin.yUR = max(atof(tokens[1].c_str()), myPin.yUR);
					}
					get_next_token(is, tokens[0], LEFCommentChar);
					assert(tokens[0] == LEFLineEndingChar);
				} else {
//					cerr << "read_lef_macro_pin:: unsupported keyword " << tokens[0] << endl;
//          exit(4);
				}
				get_next_token(is, tokens[0], LEFCommentChar);
			}
		} else if (tokens[0] == "USE") {
			while (tokens[0] != LEFLineEndingChar)
				get_next_token(is, tokens[0], LEFCommentChar);
		} else {
//			cerr << "read_lef_macro_pin:: unsupported keyword " << tokens[0] << endl;
			exit(5);
		}
		get_next_token(is, tokens[0], LEFCommentChar);
	}
	get_next_token(is, tokens[0], LEFCommentChar);
	assert(pinName == tokens[0]);
//	myMacro->pins[pinName] = myPin;
//	if (pinName == FFClkPortName)
//		myMacro->isFlop = true;
}

// assumes the keyword MACRO has already been read in
void read_lef_macro(std::istream &is, standard_cell::standard_cells * std_cells,
		library* lib) {
//	macro* myMacro;
	std::vector<std::string> tokens(1);

	get_next_token(is, tokens[0], LEFCommentChar);
	auto std_cell = std_cells->create(tokens[0]);

	geometry::multi_polygon<geometry::polygon<geometry::point<double> > > geometry;

//	myMacro = locateOrCreateMacro(tokens[0]);
	geometry::polygon<geometry::point<double> > bounding_rectangle;

	get_next_token(is, tokens[0], LEFCommentChar);
	while (tokens[0] != "END") {
		if (tokens[0] == "CLASS") {
			get_next_n_tokens(is, tokens, 2, LEFCommentChar);
			assert(tokens[1] == LEFLineEndingChar);
//			myMacro->type = tokens[0];
		} else if (tokens[0] == "ORIGIN") {
			get_next_n_tokens(is, tokens, 3, LEFCommentChar);
			assert(tokens[2] == LEFLineEndingChar);
//			myMacro->xOrig = atof(tokens[0].c_str());
//			myMacro->yOrig = atof(tokens[1].c_str());
		} else if (tokens[0] == "SIZE") {
			get_next_n_tokens(is, tokens, 4, LEFCommentChar);
			assert(tokens[1] == "BY");
			assert(tokens[3] == LEFLineEndingChar);
			std::vector<geometry::point<double> > points { { 0.0, 0.0 }, { 0.0,
					lib->dist2microns() * atof(tokens[2].c_str()) }, { lib->dist2microns() * atof(tokens[0].c_str()), lib->dist2microns() * atof(
					tokens[2].c_str()) }, { lib->dist2microns() * atof(tokens[0].c_str()), 0.0 }, {
					0.0, 0.0 } };
			boost::geometry::append(bounding_rectangle, points);
//			myMacro->width = atof(tokens[0].c_str());
//			myMacro->height = atof(tokens[2].c_str());
		} else if (tokens[0] == "SITE")
			read_lef_macro_site(is);
		else if (tokens[0] == "PIN")
			read_lef_macro_pin(is);
		else if (tokens[0] == "SYMMETRY") {
			// NOTE: this contest does not allow flipping/rotation
			// even though symmetries are specified for a macro
			get_next_token(is, tokens[0], LEFCommentChar);
			while (tokens[0] != LEFLineEndingChar)
				get_next_token(is, tokens[0], LEFCommentChar);
			assert(tokens[0] == LEFLineEndingChar);
		} else if (tokens[0] == "FOREIGN") {
			get_next_n_tokens(is, tokens, 4, LEFCommentChar);
			assert(tokens[3] == LEFLineEndingChar);
		} else if (tokens[0] == "OBS") {
			get_next_token(is, tokens[0], LEFCommentChar);
			// NOTE: this contest does not handle other than LAYER keyword for OBS
			while (tokens[0] != "END") {
				if (tokens[0] == "LAYER") {
					get_next_n_tokens(is, tokens, 2, LEFCommentChar);
					assert(tokens[1] == LEFLineEndingChar);
					// NOTE: this contest only checks legality on metal 1 layer
					if (tokens[0] == "metal1") {
						get_next_token(is, tokens[0], LEFCommentChar);
						while (tokens[0] != "END" && tokens[0] != "LAYER") {
							assert(tokens[0] == "RECT");
							get_next_n_tokens(is, tokens, 5, LEFCommentChar);

							std::vector<geometry::point<double> > points { {

							lib->dist2microns() * atof(tokens[0].c_str()), lib->dist2microns() * atof(tokens[1].c_str()) },
									{ lib->dist2microns() * atof(tokens[0].c_str()), lib->dist2microns() * atof(
											tokens[3].c_str()) }, { lib->dist2microns() * atof(
											tokens[2].c_str()), lib->dist2microns() * atof(
											tokens[3].c_str()) }, { lib->dist2microns() * atof(
											tokens[2].c_str()), lib->dist2microns() * atof(
											tokens[1].c_str()) }, { lib->dist2microns() * atof(
											tokens[0].c_str()), lib->dist2microns() * atof(
											tokens[1].c_str()) } };
							geometry::polygon<geometry::point<double> > current_polygon;
							boost::geometry::append(current_polygon, points);
							geometry.push_back(current_polygon);
							get_next_token(is, tokens[0], LEFCommentChar);
						}
					} else
						get_next_token(is, tokens[0], LEFCommentChar);
				} else
					get_next_token(is, tokens[0], LEFCommentChar);
			}
		} else {
			exit(2);
		}
		get_next_token(is, tokens[0], LEFCommentChar);
	}
	get_next_token(is, tokens[0], LEFCommentChar);
	if(geometry.empty())
		geometry.push_back(bounding_rectangle);
	lib->geometry(std_cell, geometry);
//	assert(myMacro->name == tokens[0]);
	return;
}

}

void read(std::istream& dot_lef, standard_cell::standard_cells* std_cells,
		library* lib) {
	std::cout << "reading .lef file..." << std::endl;
	std::vector<std::string> tokens(1);
	while (!dot_lef.eof()) {
		parser::get_next_token(dot_lef, tokens[0], parser::LEFCommentChar);

		if (tokens[0] == parser::LEFLineEndingChar)
			continue;

		if (tokens[0] == "VERSION") {
			parser::get_next_token(dot_lef, tokens[0], parser::LEFCommentChar);
		} else if (tokens[0] == "NAMECASESENSITIVE") {
			parser::get_next_token(dot_lef, tokens[0], parser::LEFCommentChar);
		} else if (tokens[0] == "BUSBITCHARS") {
			parser::get_next_token(dot_lef, tokens[0], parser::LEFCommentChar);
			unsigned index1 = tokens[0].find_first_of("\"");
			unsigned index2 = tokens[0].find_last_of("\"");
			assert(index1 != std::string::npos);
			assert(index2 != std::string::npos);
			assert(index2 > index1);
		} else if (tokens[0] == "DIVIDERCHAR") {
			parser::get_next_token(dot_lef, tokens[0], parser::LEFCommentChar);
			unsigned index1 = tokens[0].find_first_of("\"");
			unsigned index2 = tokens[0].find_last_of("\"");
			assert(index1 != std::string::npos);
			assert(index2 != std::string::npos);
			assert(index2 > index1);
		} else if (tokens[0] == "UNITS") {
			parser::get_next_n_tokens(dot_lef, tokens, 3,
					parser::LEFCommentChar);
			assert(tokens.size() == 3);
			assert(tokens[0] == "DATABASE");
			assert(tokens[1] == "MICRONS");
            lib->dist2microns(std::atoi(tokens[2].c_str()));
			parser::get_next_n_tokens(dot_lef, tokens, 3,
					parser::LEFCommentChar);
			assert(tokens[0] == parser::LEFLineEndingChar);
			assert(tokens[1] == "END");
			assert(tokens[2] == "UNITS");
		} else if (tokens[0] == "MANUFACTURINGGRID") {
			parser::get_next_token(dot_lef, tokens[0],
					parser::LEFLineEndingChar);
		} else if (tokens[0] == "SITE")
			parser::read_lef_site(dot_lef);
		else if (tokens[0] == "LAYER")
			parser::read_lef_layer(dot_lef);
		else if (tokens[0] == "MACRO")
			parser::read_lef_macro(dot_lef, std_cells, lib);
		else if (tokens[0] == "END") {
			parser::get_next_token(dot_lef, tokens[0], parser::LEFCommentChar);
			assert(tokens[0] == "LIBRARY");
			break;
		}
	}
	std::cout << "reading .lef file DONE" << std::endl;
//	dot_lef.close();

}

}
} /* namespace placement */
} /* namespace openeda */

