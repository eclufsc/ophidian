#include "canvascontroller.h"

CanvasController::CanvasController()
{

}

CanvasController::~CanvasController()
{

}

void CanvasController::buildICCAD2017(std::string lef, std::string def, std::string verilog)
{

}

void CanvasController::buildICCAD2015(std::string lef, std::string def, std::string verilog)
{
    /* Building */
    mBuilder = new ophidian::design::ICCAD2015ContestDesignBuilder(lef, def, verilog);
    mDesign = &mBuilder->build();

    /* Creating property Cells to Quads */
    //mQuads = std::make_unique(netlist.makeProperty<std::vector<Quad>>(ophidian::circuit::Cell()));

    /* Getting information of netlist */
    size_t cells = mDesign->netlist().size(ophidian::circuit::Cell());
    size_t pins = mDesign->netlist().size(ophidian::circuit::Pin());
    size_t nets = mDesign->netlist().size(ophidian::circuit::Net());

    /* Saying to MainWindow display the information */
    emit changeCircuitBox("test", 1, cells, pins, nets);
}

void CanvasController::mousePress(const ophidian::geometry::Point &p)
{
    //mState->mouse_press(p);
}

void CanvasController::mouseMove(const ophidian::geometry::Point &p)
{
    //mSstate->mouse_move(p);
}

void CanvasController::mouseRelease()
{
    //mSstate->mouse_release();
}
