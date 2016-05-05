#ifndef OPHIDIAN_APPS_UDDAC2016_SFML_MYSFMLCANVAS_H
#define OPHIDIAN_APPS_UDDAC2016_SFML_MYSFMLCANVAS_H

#include "qsfmlcanvas.h"

#include "../gui/canvas.h"

namespace uddac2016 {

class mysfmlcanvas : public QSFMLCanvas {

    ophidian::gui::canvas m_canvas;
    sf::Font m_opensans;

    sf::View m_camera_view;

public:
    mysfmlcanvas(QWidget *parent = 0);
    virtual ~mysfmlcanvas();
    void OnInit();
    void OnUpdate();

    void wheelEvent(QWheelEvent * e);
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *e);

    void center_view_on(const ophidian::geometry::point<double> & p1);
    void view_size(const ophidian::geometry::point<double> & size);

    ophidian::gui::line line_create(const ophidian::geometry::point<double> & p1, const ophidian::geometry::point<double> & p2);
    ophidian::gui::quad quad_create(const ophidian::geometry::point<double> & p1, const ophidian::geometry::point<double> & p2, const ophidian::geometry::point<double> & p3, const ophidian::geometry::point<double> & p4);

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


};

}

#endif /* OPHIDIAN_APPS_UDDAC2016_SFML_MYSFMLCANVAS_H */
