#ifndef SFMLCANVAS_H
#define SFMLCANVAS_H

#include "qsfmlcanvas.h"
#include "canvas.h"
#include "spatial_index.h"
#include "fps.h"
#include "../geometry/geometry.h"

#include <unordered_set>

namespace ophidian {
namespace gui {

class circuit_canvas;
namespace canvas_states {
class state {
protected:
    circuit_canvas * m_canvas;
public:
    state(circuit_canvas * canvas) : m_canvas(canvas){}
    virtual ~state() {}
    virtual void update() {}
    virtual void wheelEvent(QWheelEvent * e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent * e) {}
    virtual void mouseMoveEvent(QMouseEvent *e) {}
    virtual void mouseReleaseEvent(QMouseEvent * e) {}
};

class blocked : public state {
public:
    blocked(circuit_canvas * canvas);
    virtual void update() {}
    virtual void wheelEvent(QWheelEvent * e){};
    virtual void keyPressEvent(QKeyEvent *e){};
    virtual void mousePressEvent(QMouseEvent * e) {}
    virtual void mouseMoveEvent(QMouseEvent *e) {}
    virtual void mouseReleaseEvent(QMouseEvent * e) {}
};

class idle : public state {
public:
    idle(circuit_canvas * canvas);
    void mousePressEvent(QMouseEvent * e);
};

class selected : public state {
    wire_quad m_wirequad;
    quad m_quad;
public:
    selected(circuit_canvas * canvas, quad the_quad);
    virtual ~selected();
    void mousePressEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent *e);
};

class dragging : public state {
    quad m_quad;
    sf::Vector2f m_initial;
    bool m_moved;
public:
    dragging(circuit_canvas * canvas, const sf::Vector2f & pos);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent * e);
};


}

class circuit_canvas : public QSFMLCanvas
{
protected:
    canvas m_canvas;
    spatial_index m_index;
    canvas_states::state * m_state;
    canvas_states::state * m_save_state;
    sf::View m_camera;
    fps m_fps;

    std::unordered_set< ophidian::gui::quad > m_non_movable_quads;
    void render(sf::RenderTarget & target);
public:
    circuit_canvas(QWidget *parent = 0);
    virtual ~circuit_canvas();

    void state(canvas_states::state * state);
    sf::Vector2f mapPixelToCamera(const sf::Vector2i & pixel) const;


    bool hasAnimation() const;
    void animate(batch_animation * animation);

    const drawable_batch<4>&  quadsBatch();


    virtual void select(quad the_quad);
    virtual void unselect(quad the_quad);

    void indexQuad(quad the_quad);
    void unindexQuad(quad the_quad);

    quad drawQuad(const geometry::point<double> & p1, const geometry::point<double> & p2, const geometry::point<double> & p3, const geometry::point<double> & p4);
    quad quadAt(const sf::Vector2f & point) const;
    bool hasQuad(const sf::Vector2f & point) const;
    void setFixed(quad the_quad);
    virtual void dropQuad(quad the_quad); // virtual, we may call some function when drop a quad, like place_cell()
    virtual void eraseQuad(quad the_quad);
    bool isFixed(quad the_quad) const;

    wire_quad drawRect(quad from_quad);
    wire_quad drawRect(const geometry::point<double> & p1, const geometry::point<double> & p2, const geometry::point<double> & p3, const geometry::point<double> & p4);


    // camera
    void cameraCenter(const sf::Vector2f & position);
    void moveCamera(const sf::Vector2f & delta);
    sf::Vector2f cameraSize() const;
    void cameraSize(const sf::Vector2f & new_size);
    void zoom(double value);

    void reset();

    void save_to_file(const std::string & filename);

    void OnUpdate();
    void wheelEvent(QWheelEvent * e);
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent * e);

    template <class T>
    void paint(T element, const sf::Color & color) {
        m_canvas.paint(element, color);
    }

    template <class T>
    void transform(T element, const sf::Transform & transformation) {
        m_canvas.transform(element, transformation);
    }

    template <class T>
    void erase(T element) {
        m_canvas.destroy(element);
    }


    void clear();
    void clear(const sf::Color & color);


};

}
}

#endif // SFMLCANVAS_H
