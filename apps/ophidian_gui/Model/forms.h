#ifndef FORMS_H
#define FORMS_H

#include <ophidian/circuit/Netlist.h>

class Form
{
public:
    Form();
    ~Form();

    uint32_t mId;
};

class Line : public Form {
public:
    Line();
    explicit Line(ophidian::circuit::Cell &cell);

    ~Line();

    ophidian::circuit::Cell mCell;
};

class Quad : public Form {
public:
    Quad();
    explicit Quad(ophidian::circuit::Cell &cell);

    ~Quad();

    bool operator==(const Quad & o) const;

    ophidian::circuit::Cell mCell;
};

class WireQuad : public Line {
public:
    WireQuad();
    explicit WireQuad(ophidian::circuit::Cell &cell);

    ~WireQuad();

    std::array<Line, 4> mLines;
};


#endif // FORMS_H
