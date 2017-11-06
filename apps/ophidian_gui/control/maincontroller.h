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
    void clear(WireQuad & wire);

    ophidian::geometry::Point chipBoundaries();

public slots:
    void buildICCAD2017(std::string lef, std::string def, std::string verilog);
    void buildICCAD2015(std::string lef, std::string def, std::string verilog);

    /*
    void resetState();
    void mainController(MainController * ctrl);
    void state(CanvasState * state);

    void createIndex(const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);

    void createQuads(const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    void updateQuads(ophidian::gui::drawable_batch<4> & quads, const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    void animateQuads(ophidian::gui::batch_animation * animation);
    void paintQuads(cell_painter & painter);

    ophidian::gui::wire_quad create_wire_quad(const ophidian::gui::quad & quad);
    void remove_wire_quad(const ophidian::gui::wire_quad &quad);
    void move_wire_quad(const ophidian::gui::wire_quad &quad, const ophidian::geometry::point<double> & delta);


    void commit_quad_position(ophidian::gui::quad the_quad);
    std::vector<ophidian::gui::quad> quads_at(const ophidian::geometry::point<double> & p) const {
        return m_index.quads_containing(p);
    }

     ... const std::array<sf::Vertex, 4> & quadPoints(ophidian::gui::quad the_quad) const;

     ... void moveQuad(Quad the_quad, const ophidian::geometry::Point & delta);
     ... DrawableBatch<4> & quads();
     */

signals:
    void on_circuit_labelsChanged(QString name, QString die, size_t cells, size_t pins, size_t nets);
    void on_selected_cellChanged(QString name, QString type,  double x, double y, int worstSlack);
    void allocQuad(Quad & quad, const ophidian::geometry::Point p1, const ophidian::geometry::Point p2, const ophidian::geometry::Point p3, const ophidian::geometry::Point p4);

private:
    void createQuads();
    // state?
    Canvas * mCanvas;
    SpatialIndex mIndex;

    ophidian::design::Design * mDesign{nullptr};
    ophidian::design::DesignBuilder * mBuilder{nullptr};
    ophidian::entity_system::Property<ophidian::circuit::Cell, std::vector<Quad>> mCellToQuads;
};

#endif // MAINCONTROLLER_H
