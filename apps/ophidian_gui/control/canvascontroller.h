#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include <unordered_map>

#include <ophidian/geometry/Models.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/design/Design.h>
#include "./Model/forms.h"

class MySFMLCanvas;

class CanvasController
{
public:
    CanvasController(MySFMLCanvas * canvas);

    //void resetState();
    //void mainController(MainController * ctrl);
    //void state(CanvasState * state);

    //void createIndex(const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);

    //void createQuads(const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    //void updateQuads(ophidian::gui::drawable_batch<4> & quads, const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    //void animateQuads(ophidian::gui::batch_animation * animation);
    //void paintQuads(cell_painter & painter);


    //ophidian::gui::wire_quad create_wire_quad(const ophidian::gui::quad & quad);
    //void remove_wire_quad(const ophidian::gui::wire_quad &quad);
    //void move_wire_quad(const ophidian::gui::wire_quad &quad, const ophidian::geometry::point<double> & delta);


    /*void commit_quad_position(ophidian::gui::quad the_quad);
    std::vector<ophidian::gui::quad> quads_at(const ophidian::geometry::point<double> & p) const {
        return m_index.quads_containing(p);
    }*/

    // ... const std::array<sf::Vertex, 4> & quadPoints(ophidian::gui::quad the_quad) const;

    // ... void moveQuad(Quad the_quad, const ophidian::geometry::Point & delta);
    void mousePress(const ophidian::geometry::Point & p);
    void mouseMove(const ophidian::geometry::Point & p);
    void mouseRelease();
    // ... DrawableBatch<4> & quads();

private:
    // MainController mMainController;
    // quadToEntity e entityToQuad
    // quads?
    // state?
    MySFMLCanvas * mMySFMLCanvas;
    ophidian::design::Design * mDesign;
    //ophidian::entity_system::Property<ophidian::circuit::Cell, Quad> mQuads;

};

#endif // CANVASCONTROLLER_H
