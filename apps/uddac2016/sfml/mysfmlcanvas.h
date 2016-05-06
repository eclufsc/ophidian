#ifndef OPHIDIAN_APPS_UDDAC2016_SFML_MYSFMLCANVAS_H
#define OPHIDIAN_APPS_UDDAC2016_SFML_MYSFMLCANVAS_H

#include "qsfmlcanvas.h"

#include "../gui/canvas.h"
#include "../canvas_controller.h"

namespace uddac2016 {

class mysfmlcanvas : public QSFMLCanvas {

    canvas_controller m_controller;
    ophidian::gui::canvas m_canvas;
    sf::Font m_opensans;
    sf::View m_camera_view;

public:
    mysfmlcanvas(QWidget *parent = 0);
    virtual ~mysfmlcanvas();

    canvas_controller * controller();

    void OnInit();
    void OnUpdate();
    void wheelEvent(QWheelEvent * e);
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent * e);

    void center_view_on(const ophidian::geometry::point<double> & p1);
    void view_size(const ophidian::geometry::point<double> & size);

    ophidian::gui::line line_create(const ophidian::geometry::point<double> & p1, const ophidian::geometry::point<double> & p2);
    ophidian::gui::quad quad_create(const ophidian::geometry::point<double> & p1, const ophidian::geometry::point<double> & p2, const ophidian::geometry::point<double> & p3, const ophidian::geometry::point<double> & p4);
    ophidian::gui::drawable_batch<4> & quads();

    void quads_animate(ophidian::gui::batch_animation *animation);



    template <class T>
    void destroy(T element)
    {
        m_canvas.destroy(element);
    }

    template <class T>
    void transform(T element, const sf::Transform & transformation)
    {
        m_canvas.transform(element, transformation);
    }

    template <class T>
    void paint(T element, sf::Color color) {
        m_canvas.paint(element, color);
    }

    void quad_update(ophidian::gui::quad element, const ophidian::geometry::point<double> & p1, const ophidian::geometry::point<double> & p2, const ophidian::geometry::point<double> & p3, const ophidian::geometry::point<double> & p4) {
        m_canvas.quad_update(element, p1, p2, p3, p4);
    }


};

}

#endif /* OPHIDIAN_APPS_UDDAC2016_SFML_MYSFMLCANVAS_H */
