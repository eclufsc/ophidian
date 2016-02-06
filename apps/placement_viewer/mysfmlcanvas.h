#ifndef MYSFMLCANVAS_H
#define MYSFMLCANVAS_H

#include "qsfmlcanvas.h"
#include "application.h"

#include "circuit.h"

#include <unordered_map>

class mysfmlcanvas : public QSFMLCanvas
{
    application* m_app;
    std::unique_ptr<circuit> m_circuit;

    sf::FloatRect m_image_size;
    sf::View m_view;
    sf::View m_minimap;

    uint8_t m_minimap_opacity;
    bool m_holding_inside_minimap;


    bool m_holding_click;

public:
    mysfmlcanvas(QWidget *parent = 0);

    void app(application* app);

    void OnInit();
    void OnUpdate();


    void wheelEvent(QWheelEvent * e);
    void resizeEvent(QResizeEvent * e);
    void keyPressEvent(QKeyEvent* e);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent * e);


    void update_view_position();
};

#endif // MYSFMLCANVAS_H
