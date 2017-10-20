#include "maincontroller.h"

MainController::MainController()
{

}

MainController::~MainController()
{

}

void MainController::setCanvas(Canvas * canvas)
{
    mCanvas = canvas;
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

    std::cout << "-------------" << std::endl;
    createQuads();
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
    DrawableBatch  * drawable = mCanvas->drawableQuads();

    for (auto cellIt = mDesign->netlist().begin(ophidian::circuit::Cell()); cellIt != mDesign->netlist().end(ophidian::circuit::Cell()); cellIt++)
    {
        std::vector<Quad> boxs;

        std::cout << mDesign->netlist().name(*cellIt) << std::endl;

        auto cellGeometry = mDesign->placementMapping().geometry(*cellIt);
        for (auto cellBoxIt = cellGeometry.begin(); cellBoxIt != cellGeometry.end(); cellBoxIt++)
        {
            Quad quad;
            quad.mCell = *cellIt;

            ophidian::geometry::Point p1((*cellBoxIt).min_corner().x(), (*cellBoxIt).min_corner().y());
            ophidian::geometry::Point p2((*cellBoxIt).max_corner().x(), (*cellBoxIt).min_corner().y());
            ophidian::geometry::Point p3((*cellBoxIt).max_corner().x(), (*cellBoxIt).max_corner().y());
            ophidian::geometry::Point p4((*cellBoxIt).min_corner().x(), (*cellBoxIt).max_corner().y());

            drawable->alloc(quad, p1, p2, p3, p4);
            boxs.push_back(quad);
        }

        mQuads[*cellIt] = boxs;
    }
}
