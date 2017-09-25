#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <ophidian/design/Design.h>
#include <ophidian/design/DesignBuilder.h>

using namespace ophidian::design;

class Application
{
public:
    Application();
    ~Application();

    void buildICCAD2017(std::string lef, std::string def);
    void buildICCAD2015(std::string lef, std::string def);

private:
    Design * design;

};

#endif // APPLICATION_H
