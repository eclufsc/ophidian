#include "maincontroller.h"

MainController::MainController()
{
    srand((unsigned)time(0));
}

MainController::~MainController()
{
    delete mDesign;
}

void MainController::setCanvas(Canvas * canvas)
{
    mCanvas = canvas;
}

void MainController::selectedCell(const cell_type & cell)
{
    auto name = mDesign->netlist().name(cell);
    auto type = mDesign->standard_cells().name( mDesign->netlist().std_cell(cell) );
    auto origin = mDesign->placement().location(cell);

    auto size = cellSize(cell);

    emit on_send_cellChanged(QString::fromStdString(name),
                             QString::fromStdString(type),
                             size.x(), size.y(),
                             units::unit_cast<double>(origin.x()),
                             units::unit_cast<double>(origin.y()));
}

void MainController::mousePress(const point_type & p)
{
    if (mIndex.hasQuad(p))
        selectedCell(mIndex.quadContaining(p).mCell);
    else
        emit on_send_cellChanged("", "", 0, 0, p.x(), p.y());
}

void MainController::mouseMove(const point_type & p)
{
    emit on_send_cellChanged("moving", "moving", -1, -1, p.x(), p.y());
}

void MainController::buildICCAD2017(std::string cells_lef, std::string tech_lef, std::string placed_def)
{
    mCanvas->clear();

    if (mDesign != nullptr)
    {
        delete mDesign;
    }

    mDesign = new ophidian::design::Design{};

    auto lef = ophidian::parser::Lef{cells_lef};
    lef.read_file(tech_lef);
    auto def = ophidian::parser::Def{placed_def};

    ophidian::design::factory::make_design_iccad2017(*mDesign, def, lef);
    init();
}

void MainController::buildICCAD2015(std::string lef_file, std::string def_file, std::string verilog_file)
{
    mCanvas->clear();

    if (mDesign != nullptr)
    {
        delete mDesign;
    }

    mDesign = new ophidian::design::Design{};
    
    auto def = ophidian::parser::Def{def_file};
    auto lef = ophidian::parser::Lef{lef_file};
    auto verilog = ophidian::parser::Verilog{verilog_file};

    ophidian::design::factory::make_design_iccad2015(*mDesign, def, lef, verilog);
    init();
}

void MainController::init()
{
    mCellToQuads = mDesign->netlist().makeProperty<std::vector<Quad>>(cell_type());

    auto amountCells = mDesign->netlist().size_cell_instance();
    auto amountPins  = mDesign->netlist().size_pin_instance();
    auto amountNets  = mDesign->netlist().size_net();

    auto chipUpper = mDesign->floorplan().chip_upper_right_corner();

    std::stringstream concat;
    concat << units::unit_cast<double>(chipUpper.x()) << "μ X ";
    concat << units::unit_cast<double>(chipUpper.y()) << "μ";
    std::string dieArea = concat.str();

    emit on_send_circuitChanged("Circuit", QString::fromStdString(dieArea), amountCells, amountPins, amountNets);

    redraw();
}

void MainController::createQuads()
{
    auto urc = mDesign->floorplan().chip_upper_right_corner();
    auto x = units::unit_cast<double>(urc.x());
    auto y = units::unit_cast<double>(urc.y());
    mCanvas->createBoundaries({x,y});

    for (auto cellIt = mDesign->netlist().begin_cell_instance(); cellIt != mDesign->netlist().end_cell_instance(); ++cellIt)
    {
        std::vector<Quad> quads;

        auto cellGeometry = mDesign->placement().geometry(*cellIt);

        for (auto cellBoxIt = cellGeometry.begin(); cellBoxIt != cellGeometry.end(); ++cellBoxIt)
        {
            Quad quad(*cellIt);
            std::vector<point_type> points(4);

            point_type min{units::unit_cast<double>((*cellBoxIt).min_corner().x()),
                           units::unit_cast<double>((*cellBoxIt).min_corner().y())};
            point_type max{units::unit_cast<double>((*cellBoxIt).max_corner().x()),
                           units::unit_cast<double>((*cellBoxIt).max_corner().y())};

            points[0] = point_type(min.x(), min.y());
            points[1] = point_type(max.x(), min.y());
            points[2] = point_type(max.x(), max.y());
            points[3] = point_type(min.x(), max.y());

            mCanvas->alloc(quad, points);

            auto new_box = box_type{min,max};

            mIndex.quadCreate(quad, new_box);

            quads.push_back(quad);
        }

         mCellToQuads[*cellIt] = quads;

        if (isFixed(*cellIt))
        {
            mCanvas->paint(quads, sf::Color::Blue);
        }
        else
        {
            unsigned int random = rand();
            mCanvas->paint(quads, sf::Color((random % 10 + 195), (random % 75), (random % 120 + 135)));
        }
    }
}

bool MainController::isFixed(const cell_type & cell)
{
    // return mDesign->placement().isFixed(cell);
    return false;
}

bool MainController::hasCell(std::string name)
{
    auto cell = mDesign->netlist().find_cell_instance(name);

    try {
        // mDesign->placement().is_fixed(cell);
        return true;
    } catch (const std::out_of_range & e) {
        return false;
    }
}

bool MainController::hasQuad(const point_type & p)
{
    return mIndex.hasQuad(p);
}

Quad MainController::quadAt(const point_type & p)
{
    return mIndex.quadContaining(p);
}

std::vector<Quad> MainController::quadsCell(const cell_type & cell)
{
    return mCellToQuads[cell];
}

std::vector<Quad> MainController::quadsCell(std::string name)
{
    auto cell = mDesign->netlist().find_cell_instance(name);

    try {
        return mCellToQuads[cell];
    } catch (const std::out_of_range & e) {
        return std::vector<Quad>();
    }
}

WireQuad MainController::wireQuadOfCell(const cell_type & cell)
{
    auto local = mDesign->placement().location(cell);
    point_type point{units::unit_cast<double>(local.x()), units::unit_cast<double>(local.y())};

    return mCanvas->createWireQuad(cell, point, cellSize(cell));
}

void MainController::clear(WireQuad & wire)
{
    mCanvas->clear(wire);
}

void MainController::transform(Quad quad, const sf::Transform & trans)
{
    mCanvas->transform(mCellToQuads[quad.mCell], trans);
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
    Quad first = mCellToQuads[quad.mCell].front();
    auto newOrigin = mCanvas->points(first).front();

    mDesign->placement().place(
        first.mCell,
        ophidian::placement::Placement::point_type{
            ophidian::placement::Placement::unit_type{newOrigin.position.x},
            ophidian::placement::Placement::unit_type{newOrigin.position.y}
        }
    );

    auto cellGeometry = mDesign->placement().geometry(first.mCell);

    auto quadOfBox = mCellToQuads[first.mCell].begin();
    for (auto cellBoxIt = cellGeometry.begin(); cellBoxIt != cellGeometry.end(); cellBoxIt++, quadOfBox++)
    {
        point_type min{units::unit_cast<double>((*cellBoxIt).min_corner().x()),
                        units::unit_cast<double>((*cellBoxIt).min_corner().y())};
        point_type max{units::unit_cast<double>((*cellBoxIt).max_corner().x()),
                        units::unit_cast<double>((*cellBoxIt).max_corner().y())};

        auto new_box = box_type{min,max};

        mIndex.quadRemove(*quadOfBox);
        mIndex.quadCreate(*quadOfBox, new_box);
    }
}

void MainController::remove(Quad quad, WireQuad wire)
{
    auto quads = mCellToQuads[quad.mCell];
    mCanvas->desalloc(quads);

    std::vector<Line> lines;
    for (const auto & l : wire.mLines)
        lines.push_back(l);
    mCanvas->desalloc(lines);

    auto quadsOfBox = mCellToQuads[quad.mCell];
    for (auto quadIt = quadsOfBox.begin(); quadIt != quadsOfBox.end(); quadIt++)
    {
        mIndex.quadRemove(*quadIt);
    }

    mDesign->netlist().erase(quad.mCell);

    auto cells = mDesign->netlist().size_cell_instance();
    auto pins = mDesign->netlist().size_pin_instance();
    auto nets = mDesign->netlist().size_net();
    emit on_send_circuitChanged("removing", "removing", cells, pins, nets);
}

MainController::point_type MainController::chipBoundaries()
{
    auto point = mDesign->floorplan().chip_upper_right_corner();
    point_type result{units::unit_cast<double>(point.x()),
                        units::unit_cast<double>(point.y())};

    return result;
}

void MainController::slot1()
{
    redraw();
}

void MainController::slot2()
{
    redraw();
}

void MainController::slot3()
{
    redraw();
}

void MainController::redraw()
{
    mCanvas->clear();
    mIndex.clear();
    createQuads();
}

MainController::point_type MainController::cellSize(const cell_type & cell)
{
    unsigned int width = 0, height = 0;
    auto cellGeometry = mDesign->placement().geometry(cell);
    for (auto cellBoxIt = cellGeometry.begin(); cellBoxIt != cellGeometry.end(); cellBoxIt++)
    {
        point_type min{units::unit_cast<double>((*cellBoxIt).min_corner().x()),
                        units::unit_cast<double>((*cellBoxIt).min_corner().y())};
        point_type max{units::unit_cast<double>((*cellBoxIt).max_corner().x()),
                        units::unit_cast<double>((*cellBoxIt).max_corner().y())};

        if (width < (max.x() - min.x()))
            width = max.x() - min.x();

        height += max.y() - min.y();
    }

    return point_type(width, height);
}


void MainController::drawSVG(SVGMapper & mapper, const box_type & viewBox)
{
    if (mDesign->netlist().size_cell_instance() == 0)
        return;

    auto chipUpper = mDesign->floorplan().chip_upper_right_corner();

    std::vector<Quad> quadsInArea = mIndex.quadsContaining(viewBox);

    for (auto quadIt = quadsInArea.begin(); quadIt != quadsInArea.end(); quadIt++)
    {
        auto points = mCanvas->points(*quadIt);

        double yMin = points[0].position.y <= viewBox.min_corner().y() ? 0 : points[0].position.y - viewBox.min_corner().y();
        double xMax = points[2].position.x - viewBox.min_corner().x() <= viewBox.max_corner().x() - viewBox.min_corner().x() ?
                      points[2].position.x - viewBox.min_corner().x() : viewBox.max_corner().x() - viewBox.min_corner().x();

        point_type min((points[0].position.x - viewBox.min_corner().x())/100, yMin/100);
        point_type max(xMax/100, (points[2].position.y - viewBox.min_corner().y())/100);
        box_type box(min, max);

        QString style;
        if (isFixed(quadIt->mCell))
        {
            style += "fill:rgb(0,0,255)";
        }
        else
        {
            unsigned int random = rand();
            style += "fill:rgb(";
            style += QString::number((random % 10 + 195)) + ",";
            style += QString::number((random % 75)) + ",";
            style += QString::number((random % 120 + 135)) + ")";
        }

        mapper.add(box);
        mapper.map(box, style.toStdString());
    }

    // Left
    if (viewBox.min_corner().x() <= 0)
    {
        double bY = viewBox.min_corner().y() >= 0 ? 0 : -viewBox.min_corner().y()/100;
        double uY = viewBox.max_corner().y() <= units::unit_cast<double>(chipUpper.y()) ? (viewBox.max_corner().y()-viewBox.min_corner().y())/100 :
                                               (units::unit_cast<double>(chipUpper.y())-viewBox.min_corner().y())/100;

        point_type bottomY(-viewBox.min_corner().x()/100, bY);
        point_type upperY (-viewBox.min_corner().x()/100, uY);

        auto line = line_type{bottomY, upperY};
        mapper.add(line);
        mapper.map(line, "stroke:rgb(255,255,255)");
    }

    // Rigth
    if (viewBox.max_corner().x() >= units::unit_cast<double>(chipUpper.x()))
    {
        double bY = viewBox.min_corner().y() >= 0 ? 0 : -viewBox.min_corner().y()/100;
        double uY = viewBox.max_corner().y() <= units::unit_cast<double>(chipUpper.y()) ? (viewBox.max_corner().y()-viewBox.min_corner().y())/100 :
                                               (units::unit_cast<double>(chipUpper.y())-viewBox.min_corner().y())/100;
        point_type bottomY((units::unit_cast<double>(chipUpper.x())-viewBox.min_corner().x())/100, bY);
        point_type upperY ((units::unit_cast<double>(chipUpper.x())-viewBox.min_corner().x())/100, uY);

        auto line = line_type{bottomY, upperY};
        mapper.add(line);
        mapper.map(line, "stroke:rgb(255,255,255)");
    }

    // Bottom
    if (viewBox.min_corner().y() <= 0)
    {
        double lX = viewBox.min_corner().x() >= 0 ? 0 : -viewBox.min_corner().x()/100;
        double rX = viewBox.max_corner().x() <= units::unit_cast<double>(chipUpper.x()) ? (viewBox.max_corner().x()-viewBox.min_corner().x())/100 :
                                                (units::unit_cast<double>(chipUpper.x())-viewBox.min_corner().x())/100;
        point_type leftX (lX, -viewBox.min_corner().y()/100);
        point_type rightX(rX, -viewBox.min_corner().y()/100);

        auto line = line_type{leftX, rightX};
        mapper.add(line);
        mapper.map(line, "stroke:rgb(255,255,255)");
    }

    // Upper
    if (viewBox.max_corner().y() >= units::unit_cast<double>(chipUpper.y()))
    {
        double bY = viewBox.min_corner().x() >= 0 ? 0 : -viewBox.min_corner().x()/100;
        double uY = viewBox.max_corner().x() <= units::unit_cast<double>(chipUpper.x()) ? (viewBox.max_corner().x()-viewBox.min_corner().x())/100 :
                                                (units::unit_cast<double>(chipUpper.x())-viewBox.min_corner().x())/100;
        point_type bottomY(bY, (units::unit_cast<double>(chipUpper.x())-viewBox.min_corner().y())/100);
        point_type upperY (uY, (units::unit_cast<double>(chipUpper.x())-viewBox.min_corner().y())/100);

        auto line = line_type{bottomY, upperY};
        mapper.add(line);
        mapper.map(line, "stroke:rgb(255,255,255)");
    }
}
