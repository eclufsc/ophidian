#ifndef SVGBUILDER_H
#define SVGBUILDER_H


#include <QString>

class SVGBuilder
{
public:
    SVGBuilder(double width, double height);

    void insertRect(double x, double y, size_t width, size_t height, int r, int g, int b);

    std::string getSVG();

private:
    QString mHeader;
    QString mShapes;
};

#endif // SVGBUILDER_H
