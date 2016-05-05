#include "mysfmlcanvas.h"
#include <QDebug>

namespace uddac2016 {

mysfmlcanvas::mysfmlcanvas(QWidget *parent) :
    QSFMLCanvas(parent)
{
}

mysfmlcanvas::~mysfmlcanvas()
{

}


void mysfmlcanvas::OnInit()
{
    qDebug() << "init!!";

}

void mysfmlcanvas::OnUpdate()
{
    clear(sf::Color::White);
}

}
