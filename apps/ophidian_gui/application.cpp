#include "application.h"

namespace control {

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
    std::cout << "Criando" << std::endl;
    mBuilder = new ICCAD2015ContestDesignBuilder(lef, def, verilog);
    mDesign = &mBuilder->build();
    size_t quantCells = mDesign->netlist().size(ophidian::circuit::Cell());
    std::cout << quantCells << std::endl;
    std::cout << "Termino" << std::endl;
}

}

