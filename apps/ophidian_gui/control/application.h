#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <string>
#include <QObject>
#include <SFML/Graphics.hpp>

#include <ophidian/design/Design.h>
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/placement/PlacementMapping.h>

using namespace ophidian::design;

class Application : public QObject
{
    Q_OBJECT

public:
    Application();
    ~Application();

    void buildICCAD2017(std::string lef, std::string def, std::string verilog);
    void buildICCAD2015(std::string lef, std::string def, std::string verilog);
    ophidian::geometry::Point chipSize();

public slots:

signals:
    void changeCircuitBox(QString name, size_t die, size_t cells, size_t pins, size_t nets);

private:
    DesignBuilder * mBuilder{nullptr};
    Design * mDesign{nullptr};
};

#endif // APPLICATION_H
