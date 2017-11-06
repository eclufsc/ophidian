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

Canvas * MainController::getCanvas()
{
    return mCanvas;
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
    mCellToQuads = mDesign->netlist().makeProperty<std::vector<Quad>>(ophidian::circuit::Cell());

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
    std::string name = "";
    std::string type = "";

    if (mIndex.hasQuad(p))
    {
        Quad quad = mIndex.quadContaining(p);
        name = mDesign->netlist().name(quad.mCell);
        type = mDesign->standardCells().name( mDesign->libraryMapping().cellStdCell(quad.mCell) );
        ophidian::geometry::Point origin = mDesign->placement().cellLocation(quad.mCell).toPoint();
        emit on_selected_cellChanged(QString::fromStdString(name), QString::fromStdString(type), origin.x(), origin.y(), 0);
    }
    else
    {
        emit on_selected_cellChanged(QString::fromStdString(name), QString::fromStdString(type), p.x(), p.y(), 0);
    }
}

void MainController::mouseMove(const ophidian::geometry::Point &p)
{
    emit on_selected_cellChanged("moving", "moving", p.x(), p.y(), 0);
}

void MainController::mouseRelease()
{
    //mSstate->mouse_release();
}

void MainController::createQuads()
{
    // Chip Area
    mCanvas->createBoundaries(mDesign->floorplan().chipUpperRightCorner().toPoint());

    // Cells
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

            mCanvas->alloc(quad, points);
            mIndex.quadCreate(quad, *cellBoxIt);

            forms.push_back(quad);
            quads.push_back(quad);
        }

        mCellToQuads[*cellIt] = quads;

        if (forms.size() > 1) {
            mCanvas->paint(Quad(), forms, sf::Color::Blue);
        } else {
            mCanvas->paint(Quad(), forms, sf::Color(200, (rand() % 50), (rand() % 130 + 125)));
        }
    }
}

bool MainController::isFixed(const ophidian::circuit::Cell & cell)
{
    return (mCellToQuads[cell].size() > 1);
}

bool MainController::hasQuad(const ophidian::geometry::Point & p)
{
    return mIndex.hasQuad(p);
}

Quad MainController::quadAt(const ophidian::geometry::Point & p)
{
    return mIndex.quadContaining(p);
}

std::vector<Quad> MainController::quadsCell(const ophidian::circuit::Cell & cell)
{
    return mCellToQuads[cell];
}

WireQuad MainController::wireQuadOfCell(const ophidian::circuit::Cell & cell)
{
    return mCanvas->createWireQuad(mCellToQuads[cell]);
}

void MainController::clear(WireQuad & wire)
{
    mCanvas->clear(wire);
}

void MainController::transform(Quad quad, const sf::Transform & trans)
{
    Quad first = mCellToQuads[quad.mCell].front();

    std::vector<Form> forms;
    for (const auto & q : mCellToQuads[first.mCell])
        forms.push_back(q);

    mCanvas->transform(first, forms, trans);
}

void MainController::transform(WireQuad wire, const sf::Transform & trans)
{
    std::vector<Form> forms;
    for (const auto & q : wire.mLines)
        forms.push_back(q);

    mCanvas->transform(Line(), forms, trans);
}

void MainController::update(Quad quad)
{
    Quad first = mCellToQuads[quad.mCell].front();
    auto newOrigin = mCanvas->points(first).front();

    mDesign->placement().placeCell(first.mCell, ophidian::util::LocationDbu(newOrigin.position.x, newOrigin.position.y));

    ophidian::geometry::MultiBox cellGeometry = mDesign->placementMapping().geometry(first.mCell);

    auto quadOfBox = mCellToQuads[first.mCell].begin();
    for (auto cellBoxIt = cellGeometry.begin(); cellBoxIt != cellGeometry.end(); cellBoxIt++, quadOfBox++)
    {
        mIndex.quadRemove(*quadOfBox);
        mIndex.quadCreate(*quadOfBox, *cellBoxIt);
    }
}

ophidian::geometry::Point MainController::chipBoundaries()
{
    return mDesign->floorplan().chipUpperRightCorner().toPoint();
}
