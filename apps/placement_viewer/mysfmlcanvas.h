#ifndef MYSFMLCANVAS_H
#define MYSFMLCANVAS_H

#include "qsfmlcanvas.h"
#include "application.h"

#include <unordered_map>

class mysfmlcanvas : public QSFMLCanvas
{
    application* m_app;

    sf::FloatRect m_image_size;
    sf::View m_view;
    sf::View m_minimap;

    uint8_t m_minimap_opacity;
    bool m_holding_inside_minimap;

    ///////////////////

    sf::VertexArray m_cells;
    sf::VertexArray m_selected;

    std::unordered_map<openeda::entity::entity,
    std::pair<std::size_t, std::size_t> > m_entity2index;

    void update_selected_vertex_array(
            const std::set<openeda::entity::entity> &selected);
    void update_cells_vertex_array(openeda::entity::entity cell);
    void create_vertex_array();

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
