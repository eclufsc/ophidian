#ifndef OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H
#define OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H

#include <string>
#include "application.h"
#include "canvas_controller.h"

#include "sfml/mysfmlcanvas.h"

namespace uddac2016
{

class MainWindow;
class controller
{
    MainWindow & m_mainwindow;
    application m_app;
    std::unique_ptr<canvas_controller> m_canvas_controller; // lazy init
public:
    controller(MainWindow & mainwindow);
    bool read_lefdef(const std::string & LEF, const std::string & DEF);
    void init_canvas_controller(mysfmlcanvas *canvas);
};

}

#endif // OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H
