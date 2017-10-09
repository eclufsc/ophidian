#ifndef MYSFMLCANVAS_H
#define MYSFMLCANVAS_H

#include <QKeyEvent>
#include <QWheelEvent>
#include <QResizeEvent>

#include <omp.h>
#include <memory>

#include <ophidian/geometry/Models.h>
#include "qsfmlcanvas.h"
#include "canvas.h"
#include "../control/canvascontroller.h"

using namespace qsfml;

class MySFMLCanvas : public QSFMLCanvas
{
public:
    MySFMLCanvas(QWidget *parent = 0);
    virtual ~MySFMLCanvas();

    CanvasController * controller();
    Canvas * canvas();

    void OnInit();
    void OnUpdate();
    void wheelEvent(QWheelEvent * e);
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent * e);

    void centerViewOn(const ophidian::geometry::Point & p1);
    void viewSize(const ophidian::geometry::Point & size);

    /*Quad quadCreate(const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4);

    // ..DrawableBatch<4> & quads();
    const std::array<sf::Vertex, 4> & quadPoints(Quad quad) const;
    void quadUpdate(Quad quad, const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4);

    // void quadsAnimate(ophidian::gui::batch_animation *animation);

    template <class T>
    void destroy(T element);

    template <class T>
    void transform(T element, const sf::Transform & transformation);

    template <class T>
    void paint(T element, sf::Color color);
    */

private:
    CanvasController mController;
    Canvas mCanvas;
    sf::View mCameraView;
};

#endif // MYSFMLCANVAS_H
