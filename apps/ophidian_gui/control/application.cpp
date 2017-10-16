#include "application.h"

Application::Application()
{

}

Application::~Application()
{
    delete mBuilder;
}

void Application::buildICCAD2017(std::string lef, std::string def, std::string verilog)
{

}

void Application::buildICCAD2015(std::string lef, std::string def, std::string verilog)
{
    mBuilder = new ICCAD2015ContestDesignBuilder(lef, def, verilog);
    mDesign = &mBuilder->build();
    size_t cells = mDesign->netlist().size(ophidian::circuit::Cell());
    size_t pins = mDesign->netlist().size(ophidian::circuit::Pin());
    size_t nets = mDesign->netlist().size(ophidian::circuit::Net());

    emit changeCircuitBox("test", 1, cells, pins, nets);
    std::cout << "Emit update" << std::endl;

}

ophidian::geometry::Point Application::chipSize()
{
    return mDesign->floorplan().chipUpperRightCorner().toPoint();
}
