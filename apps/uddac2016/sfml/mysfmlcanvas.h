#ifndef MYSFMLCANVAS_H
#define MYSFMLCANVAS_H

#include "qsfmlcanvas.h"

namespace uddac2016 {

class mysfmlcanvas : public QSFMLCanvas {
public:
    mysfmlcanvas(QWidget *parent = 0);
    virtual ~mysfmlcanvas();


    void OnInit();
    void OnUpdate();
};

}

#endif
