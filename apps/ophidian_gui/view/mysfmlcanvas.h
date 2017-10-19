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
#include "control/canvascontroller.h"
#include "control/application.h"

using namespace qsfml;

class MySFMLCanvas : public QSFMLCanvas
{
public:
    MySFMLCanvas(QWidget *parent = 0);
    virtual ~MySFMLCanvas();

    void setController(CanvasController & controller);

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

private slots:
    void update();

private:
    CanvasController * mController;
    Canvas mCanvas;
    sf::View mCameraView;
};

#endif // MYSFMLCANVAS_H
