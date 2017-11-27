#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <vector>
#include <unordered_map>
#include <QObject>

#include <sstream>
#include <string>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Range.h>
#include <ophidian/util/Units.h>
#include <ophidian/design/Design.h>
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/placement/PlacementMapping.h>
#include <ophidian/entity_system/Property.h>

#include "Model/forms.h"
#include "view/canvas.h"
#include "control/spatialindex.h"
#include "view/drawablebatch.h"

class MainController : public QObject
{
    Q_OBJECT

public:

    explicit MainController();
    ~MainController();

    void setCanvas(Canvas * canvas);
    Canvas * getCanvas();

    void mousePress(const ophidian::geometry::Point & p);
    void mouseMove(const ophidian::geometry::Point & p);
    void mouseRelease();

    bool isFixed(const ophidian::circuit::Cell & cell);
    bool hasQuad(const ophidian::geometry::Point & p);
    Quad quadAt(const ophidian::geometry::Point & p);

    std::vector<Quad> quadsCell(const ophidian::circuit::Cell & cell);
    WireQuad wireQuadOfCell(const ophidian::circuit::Cell & cell);

    void transform(Quad quad, const sf::Transform & trans);
    void transform(WireQuad wire, const sf::Transform & trans);

    void update(Quad quad);
    void remove(Quad quad, WireQuad wire);
    void clear(WireQuad & wire);

    ophidian::geometry::Point chipBoundaries();

    /* Funções auxiliares */
    void slot1();
    void slot2();
    void slot3();

public slots:
    void buildICCAD2017(std::string lef, std::string def, std::string verilog);
    void buildICCAD2015(std::string lef, std::string def, std::string verilog);

    /*
    void resetState();
    void animateQuads(ophidian::gui::batch_animation * animation);
     */

signals:
    void on_circuit_labelsChanged(QString name, QString die, size_t cells, size_t pins, size_t nets);
    void on_selected_cellChanged(QString name, QString type,  double x, double y, int worstSlack);
    void allocQuad(Quad & quad, const ophidian::geometry::Point p1, const ophidian::geometry::Point p2, const ophidian::geometry::Point p3, const ophidian::geometry::Point p4);

private:
    void createQuads();
    Canvas * mCanvas;
    SpatialIndex mIndex;

    typedef bool fixed_t;

    ophidian::design::Design * mDesign{nullptr};
    ophidian::design::DesignBuilder * mBuilder{nullptr};
    ophidian::entity_system::Property<ophidian::circuit::Cell, std::pair<std::vector<Quad>, fixed_t>> mCellToQuads;
};

#endif // MAINCONTROLLER_H
