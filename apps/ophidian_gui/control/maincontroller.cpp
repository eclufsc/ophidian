#include "maincontroller.h"

MainController::MainController()
{
    srand((unsigned)time(0));
}

MainController::~MainController()
{

}

void MainController::setCanvas(Canvas * canvas)
{
    mCanvas = canvas;
    mDrawable = mCanvas->drawableQuads();
}

void MainController::buildICCAD2017(std::string lef, std::string def, std::string verilog)
{

}

void MainController::buildICCAD2015(std::string lef, std::string def, std::string verilog)
{
    if (mBuilder != nullptr)
    {
        delete mBuilder;
        mDrawable->clear();
    }

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

    createQuads();
}

void MainController::mousePress(const ophidian::geometry::Point &p)
{
    //mState->mouse_press(p);
    //std::cout << mIndex.hasQuad(p) << std::endl;
    if (mIndex.hasQuad(p))
    {
        Quad quad = mIndex.quadContaining(p);
        std::string name = mDesign->netlist().name(quad.mCell);
        //std::string type_maybe = mDesign->standardCells().name(quad.mCell);
        ophidian::geometry::Point origin = mDesign->placement().cellLocation(quad.mCell).toPoint();
        emit selectedCell(QString::fromStdString(name), QString::fromStdString(name), origin.x(), origin.y(), 0);
    }
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
    //const double rowHeight = units::unit_cast<double>(mDesign->floorplan().rowUpperRightCorner(*mDesign->floorplan().rowsRange().begin()).y());


    sf::Transform mirror;
    mirror.scale(1.0, -1.0);

    for (auto cellIt = mDesign->netlist().begin(ophidian::circuit::Cell()); cellIt != mDesign->netlist().end(ophidian::circuit::Cell()); cellIt++)
    {
        std::vector<Quad> boxs;

        ophidian::geometry::MultiBox cellGeometry = mDesign->placementMapping().geometry(*cellIt);

        for (auto cellBoxIt = cellGeometry.begin(); cellBoxIt != cellGeometry.end(); cellBoxIt++)
        {
            Quad quad;
            quad.mCell = *cellIt;

            ophidian::geometry::Point p1((*cellBoxIt).min_corner().x(), (*cellBoxIt).min_corner().y());
            ophidian::geometry::Point p2((*cellBoxIt).max_corner().x(), (*cellBoxIt).min_corner().y());
            ophidian::geometry::Point p3((*cellBoxIt).max_corner().x(), (*cellBoxIt).max_corner().y());
            ophidian::geometry::Point p4((*cellBoxIt).min_corner().x(), (*cellBoxIt).max_corner().y());

            mDrawable->alloc(quad, p1, p2, p3, p4);
            mIndex.quadCreate(quad, *cellBoxIt);

            boxs.push_back(quad);
        }

        mQuads[*cellIt] = boxs;
        mDrawable->transform(boxs, mirror);

        if (boxs.size() > 1) {
            mDrawable->paint(boxs, sf::Color::Blue);
        } else {
            mDrawable->paint(boxs, sf::Color(200, (rand() % 50), (rand() % 130 + 125)));
        }

    }
}
