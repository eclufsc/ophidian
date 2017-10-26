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
}

void MainController::buildICCAD2017(std::string lef, std::string def, std::string verilog)
{

}

void MainController::buildICCAD2015(std::string lef, std::string def, std::string verilog)
{
    if (mBuilder != nullptr)
    {
        delete mBuilder;
        mCanvas->clear();
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

    ophidian::geometry::Point chipUpper = mDesign->floorplan().chipUpperRightCorner().toPoint();

    std::stringstream concat;
    concat << chipUpper.x() << "μ X " << chipUpper.y() << "μ";
    std::string dieArea = concat.str();

    /* Saying to MainWindow display the information */
    emit on_circuit_labelsChanged("test", QString::fromStdString(dieArea), cells, pins, nets);

    createQuads();
}

void MainController::mousePress(const ophidian::geometry::Point &p)
{
    //mState->mouse_press(p);
    //std::cout << mIndex.hasQuad(p) << std::endl;
    std::string name = "";
    std::string type = "";

    if (mIndex.hasQuad(p))
    {
        Quad quad = mIndex.quadContaining(p);
        name = mDesign->netlist().name(quad.mCell);
        type = mDesign->standardCells().name( mDesign->libraryMapping().cellStdCell(quad.mCell) );
        ophidian::geometry::Point origin = mDesign->placement().cellLocation(quad.mCell).toPoint();
        emit on_selected_cellChanged(QString::fromStdString(name), QString::fromStdString(type), origin.x(), origin.y(), 0);
    } else
    {
        emit on_selected_cellChanged(QString::fromStdString(name), QString::fromStdString(type), p.x(), p.y(), 0);
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

    DrawableBatch<4> * drawable = mCanvas->drawableQuads();


    sf::Transform mirror;
    mirror.scale(1.0, -1.0);

    for (auto cellIt = mDesign->netlist().begin(ophidian::circuit::Cell()); cellIt != mDesign->netlist().end(ophidian::circuit::Cell()); cellIt++)
    {

        std::vector<Quad> quads;
        std::vector<Form> forms;

        ophidian::geometry::MultiBox cellGeometry = mDesign->placementMapping().geometry(*cellIt);

        for (auto cellBoxIt = cellGeometry.begin(); cellBoxIt != cellGeometry.end(); cellBoxIt++)
        {
            Quad quad;
            std::vector<ophidian::geometry::Point> points;

            quad.mCell = *cellIt;

            points.push_back( ophidian::geometry::Point( (*cellBoxIt).min_corner().x(), (*cellBoxIt).min_corner().y()) );
            points.push_back( ophidian::geometry::Point( (*cellBoxIt).max_corner().x(), (*cellBoxIt).min_corner().y()) );
            points.push_back( ophidian::geometry::Point( (*cellBoxIt).max_corner().x(), (*cellBoxIt).max_corner().y()) );
            points.push_back( ophidian::geometry::Point( (*cellBoxIt).min_corner().x(), (*cellBoxIt).max_corner().y()) );

            drawable->alloc(quad, points);
            mIndex.quadCreate(quad, *cellBoxIt);

            forms.push_back(quad);
            quads.push_back(quad);
        }

        mQuads[*cellIt] = quads;
        drawable->transform(forms, mirror);

        if (forms.size() > 1) {
            drawable->paint(forms, sf::Color::Blue);
        } else {
            drawable->paint(forms, sf::Color(200, (rand() % 50), (rand() % 130 + 125)));
        }

    }
}
