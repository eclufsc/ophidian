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
};

}

#endif /* OPHIDIAN_APPS_UDDAC2016_SFML_MYSFMLCANVAS_H */
