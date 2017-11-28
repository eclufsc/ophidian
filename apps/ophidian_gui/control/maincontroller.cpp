#include "maincontroller.h"

MainController::MainController()
{
    srand((unsigned)time(0));
}

MainController::~MainController()
{
    delete mBuilder;
}

void MainController::setCanvas(Canvas * canvas)
{
    mCanvas = canvas;
}

void MainController::buildICCAD2017(std::string cells_lef, std::string tech_lef, std::string placed_def)
{
    if (mBuilder != nullptr)
    {
        delete mBuilder;
        mCanvas->clear();
    }

    /* Building */
    mBuilder = new ophidian::design::ICCAD2017ContestDesignBuilder(cells_lef, tech_lef, placed_def);
    init();
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
    init();
}

void MainController::init()
{
    mDesign = & mBuilder->build();

    /* Creating property Cells to Quads */
    mCellToQuads = mDesign->netlist().makeProperty<std::pair<std::vector<Quad>, fixed_t>>(ophidian::circuit::Cell());

    /* Getting information of netlist */
    size_t cells = mDesign->netlist().size(ophidian::circuit::Cell());
    size_t pins = mDesign->netlist().size(ophidian::circuit::Pin());
    size_t nets = mDesign->netlist().size(ophidian::circuit::Net());

    ophidian::geometry::Point chipUpper = mDesign->floorplan().chipUpperRightCorner().toPoint();

    std::stringstream concat;
    concat << chipUpper.x() << "μ X " << chipUpper.y() << "μ";
    std::string dieArea = concat.str();

    /* Getting name of file
    size_t pos = 0;
    std::string name;
    while ((pos = lef.find("/")) != std::string::npos) {
        name = lef.substr(0, pos);
        lef.erase(0, pos + 1);
    }
    name = lef.substr(0, lef.find("."));
    */

    /* Saying to MainWindow display the information */
    emit on_circuit_labelsChanged("Circuit", QString::fromStdString(dieArea), cells, pins, nets);

    createQuads();
}

void MainController::mousePress(const ophidian::geometry::Point &p)
{
    std::string name = "";
    std::string type = "";

    if (mIndex.hasQuad(p))
    {
        Quad quad = mIndex.quadContaining(p);
        type = mDesign->netlist().name(quad.mCell);
        name = mDesign->standardCells().name( mDesign->libraryMapping().cellStdCell(quad.mCell) );
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

void MainController::createQuads()
{
    // Chip Area
    mCanvas->createBoundaries(mDesign->floorplan().chipUpperRightCorner().toPoint());

    // Row information
    ophidian::geometry::Point rowSize = mDesign->floorplan().rowUpperRightCorner(*(mDesign->floorplan().rowsRange().begin())).toPoint();
    unsigned int height = 0, width = 0;

    // Cells
    for (auto cellIt = mDesign->netlist().begin(ophidian::circuit::Cell()); cellIt != mDesign->netlist().end(ophidian::circuit::Cell()); cellIt++)
    {

        std::vector<Quad> quads;

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

            if (height < (*cellBoxIt).max_corner().y() - (*cellBoxIt).min_corner().y())
                height = (*cellBoxIt).max_corner().y() - (*cellBoxIt).min_corner().y();

            if (width < (*cellBoxIt).max_corner().x() - (*cellBoxIt).min_corner().x())
                width = (*cellBoxIt).max_corner().x() - (*cellBoxIt).min_corner().x();

            quads.push_back(quad);
        }

        // placement of dev ... does not have isFixed ()
        // if (quads.size() > 1 || width > rowSize.x() || height > rowSize.y()) {
        if (mDesign->placement().isFixed(*cellIt)) {
            mCellToQuads[*cellIt] = std::make_pair(quads, true);
            mCanvas->paint(quads, sf::Color::Blue);
        } else {
            mCellToQuads[*cellIt] = std::make_pair(quads, false);
            unsigned int random = rand();
            mCanvas->paint(quads, sf::Color((random % 10 + 195), (random % 75), (random % 120 + 135)));
        }

        height = width = 0;
    }
}

bool MainController::isFixed(const ophidian::circuit::Cell & cell)
{
    return mCellToQuads[cell].second;
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
    return mCellToQuads[cell].first;
}

WireQuad MainController::wireQuadOfCell(const ophidian::circuit::Cell & cell)
{
    return mCanvas->createWireQuad(mCellToQuads[cell].first);
}

void MainController::clear(WireQuad & wire)
{
    mCanvas->clear(wire);
}

void MainController::transform(Quad quad, const sf::Transform & trans)
{
    mCanvas->transform(mCellToQuads[quad.mCell].first, trans);
}

void MainController::transform(WireQuad wire, const sf::Transform & trans)
{
    std::vector<Line> lines;
    for (const auto & l : wire.mLines)
        lines.push_back(l);

    mCanvas->transform(lines, trans);
}

void MainController::update(Quad quad)
{
    Quad first = mCellToQuads[quad.mCell].first.front();
    auto newOrigin = mCanvas->points(first).front();

    mDesign->placement().placeCell(first.mCell, ophidian::util::LocationDbu(newOrigin.position.x, newOrigin.position.y));

    ophidian::geometry::MultiBox cellGeometry = mDesign->placementMapping().geometry(first.mCell);

    auto quadOfBox = mCellToQuads[first.mCell].first.begin();
    for (auto cellBoxIt = cellGeometry.begin(); cellBoxIt != cellGeometry.end(); cellBoxIt++, quadOfBox++)
    {
        mIndex.quadRemove(*quadOfBox);
        mIndex.quadCreate(*quadOfBox, *cellBoxIt);
    }
}

void MainController::remove(Quad quad, WireQuad wire)
{
    auto quads = mCellToQuads[quad.mCell].first;
    mCanvas->desalloc(quads);

    std::vector<Line> lines;
    for (const auto & l : wire.mLines)
        lines.push_back(l);
    mCanvas->desalloc(lines);

    auto quadsOfBox = mCellToQuads[quad.mCell].first;
    for (auto quadIt = quadsOfBox.begin(); quadIt != quadsOfBox.end(); quadIt++)
    {
        mIndex.quadRemove(*quadIt);
    }

    mDesign->netlist().erase(quad.mCell);
    /* Just hedge the points to the origin, not to need to
     * find a cell to put in place (quantities of different quads).
     */

    size_t cells = mDesign->netlist().size(ophidian::circuit::Cell());
    size_t pins = mDesign->netlist().size(ophidian::circuit::Pin());
    size_t nets = mDesign->netlist().size(ophidian::circuit::Net());
    emit on_circuit_labelsChanged("removing", "removing", cells, pins, nets);
}

ophidian::geometry::Point MainController::chipBoundaries()
{
    return mDesign->floorplan().chipUpperRightCorner().toPoint();
}

void MainController::slot1()
{
    /* - Implement your algorithm or functionality here.
     */

    if (mBuilder != nullptr)
        updateAfterSlots();
}

void MainController::slot2()
{
    /* - Implement your algorithm or functionality here.
     */

    if (mBuilder != nullptr)
        updateAfterSlots();
}

void MainController::slot3()
{
    /* - Implement your algorithm or functionality here.
     */

    if (mBuilder != nullptr)
        updateAfterSlots();
}

void MainController::updateAfterSlots()
{
    mCanvas->clear();
    mIndex.clear();
    createQuads();
}
