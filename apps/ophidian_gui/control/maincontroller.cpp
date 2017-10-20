#include "maincontroller.h"

MainController::MainController()
{

}

MainController::~MainController()
{

}

void MainController::buildICCAD2017(std::string lef, std::string def, std::string verilog)
{

}

void MainController::buildICCAD2015(std::string lef, std::string def, std::string verilog)
{
    /* Building */
    mBuilder = new ophidian::design::ICCAD2015ContestDesignBuilder(lef, def, verilog);
    mDesign = &mBuilder->build();

    /* Creating property Cells to Quads */
    mQuads = mDesign->netlist().makeProperty<std::vector<Quad>>(ophidian::circuit::Cell());

    /* Getting information of netlist */
    size_t cells = mDesign->netlist().size(ophidian::circuit::Cell());
    size_t pins = mDesign->netlist().size(ophidian::circuit::Pin());
    size_t nets = mDesign->netlist().size(ophidian::circuit::Net());

    /* Saying to MainWindow display the information */
    emit changeCircuitBox("test", 1, cells, pins, nets);
}

void MainController::mousePress(const ophidian::geometry::Point &p)
{
    //mState->mouse_press(p);
}

void MainController::mouseMove(const ophidian::geometry::Point &p)
{
    //mSstate->mouse_move(p);
}

void MainController::mouseRelease()
{
    //mSstate->mouse_release();
}

void MainController::createQuads()
{
    for (auto cellIt = mDesign->netlist().begin(ophidian::circuit::Cell()); cellIt != mDesign->netlist().end(ophidian::circuit::Cell()); cellIt++)
    {

    }
}
