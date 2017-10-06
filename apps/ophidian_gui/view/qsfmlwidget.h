/* Copyright (c) 2015 Arty McLabin
 * SFML drawzone Qt widget-class for free use by you, fellow - OOP gamedevs.
 * Based on GNKW's "QSFMLCanvas" abstract class, and implements it as a convinient
 * ready-to-use widget class along other UI objects, like QOpenGLWidget.
 *** I do not claim to be the author of SFML binding into Qt, instead trying to make
 *** it more useful and convinient, less pointless (as some say).
 **  feel free to use and edit the code at this file, yet please, be thankful and
 **  don't forget to mention other's effort in your project as long as you are using
 **  their code parts. Good Luck!
 '*********************************************************************************/



#ifndef QSFMLWIDGET_H
#define QSFMLWIDGET_H

#include "qsfmlcanvas.h"
#include "control/application.h"

using namespace qsfml;
  
class QSFMLWidget : public QSFMLCanvas
{
public:
    QSFMLWidget(QWidget* parent, const QPoint& position=QPoint(0,0), const QSize& size=QSize(200,200));
    ~QSFMLWidget();

    void setApplication(apps::Application & app);

private:
    void OnInit();
    void OnUpdate();
    void wheelEvent(QWheelEvent * e);
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent * e);

    sf::View view;
    apps::Application * mApp;
    std::vector<sf::RectangleShape> * boxes{nullptr};

public slots:
    void update();

signals:
};

#endif // QSFMLWIDGET_H
