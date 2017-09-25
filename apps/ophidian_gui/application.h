#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <string>
#include <ophidian/design/Design.h>
#include <ophidian/design/DesignBuilder.h>

using namespace ophidian::design;

namespace control {

class Application
{

public:
    Application();
    ~Application();

    void buildICCAD2017(std::string lef, std::string def, std::string verilog);
    void buildICCAD2015(std::string lef, std::string def, std::string verilog);

private:
    DesignBuilder * mBuilder;
    Design * mDesign;

};

}

#endif // APPLICATION_H
